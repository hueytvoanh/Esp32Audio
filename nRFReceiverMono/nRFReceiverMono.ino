/*
MIT License

Copyright (c) 2025 Jens Nickel, Elektor Labs 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/


// to separate the 2 transmission lines if you build 2 senders and 2 receivers
#define AUDIOCHANNEL_INDEX 1

// a packet is 16 mono samples = 32 bytes
#define PACKET_LENGTH_SAMPLES 16
#define PACKET_LENGTH_BYTES   32

// application software modules needed
#include "BoardPin.h" 
#include "Output.h"

// libraries needed (in libraries folder)
#include "Stream_I2S.h"   
#include "Radio_nRF24.h"


// buffers needed
uint8_t bBuffer[8];
int16_t mBuffer[PACKET_LENGTH_SAMPLES];
int16_t oBuffer[PACKET_LENGTH_SAMPLES];

// global variables for timing
uint32_t TimeOutReceiving = 1000;        // in microseconds
uint32_t dirtyResetSeconds = 1000;
uint32_t dirtyResetSamples = 32000000;   // after that many samples a reset is done 



void setup() {

  Serial.begin(230400);  // high speed so you can printout the times of all packets arriving without loosing too much time
  delay(500);

  Radio_Init();
  Radio_Start(RADIO_MODE_RECEIVER);  // start the Radio Module for receiving

  TimeOutReceiving = 900;      // in microseconds (when next packet does not arrive in that timeframe the I2S interface will be restarted)
  dirtyResetSeconds = 1000;    // after that many seconds a reset will be done 
  
  // these variables are defined in the Stream_I2S library
  AudioMode = AUDIO_MODE_PLAYER;  // interface will be started to play music

  samplebuffer_count = 4;   // that is a low value, for very low latency
  samplebuffer_length = 16;  // also a low value for low latency
  samplerate = 32000;
  audiochannelcount = 1;  // mon
  expandchannels = 1;     // mono data will be doubled to have a "stereo" output which is more stable with the ESP32 I2S interface
  //I2S_Start();
  delay(100);

  dirtyResetSamples = dirtyResetSeconds * samplerate;   // after that many samples a reset will be done 

  // initialize Output module
  Output_Init();
  Output_ShowEvent(EVENT_POWER_ON, 1500);  // visualize that the power is on, for 1500 ms

}


// variables for time out detection
uint32_t CurrentTime = 0;
uint32_t LastPacketTime = 0;
uint32_t PacketToPacketTime = 0;
uint8_t FirstTime = 0;
uint8_t FirstTimeRestart = 1;

uint8_t command = 0;

// variables needed for dirty reset
uint32_t radiocount = 0;
int16_t LowLimit = 300;
uint8_t LowLevel = 0;

// variables needed for outputting music 
int32_t TimeOutput = 0;
int32_t LastTimeOutput = -1000;
uint8_t PacketReceived = 0;

// variables for a ringbuffer, you can use this ringbuffer for an artificial delay
#define ringbufferLen 48
int16_t dBuffer[ringbufferLen] = {0};
int16_t rbpush = 0;
int16_t rbpop = 0;



void loop() { 

/*
  if( Radio_DataReceived() )   // a packet was received
  {
    radiocount++;  // count the packets received, for dirty reset

    CurrentTime = micros();
    PacketToPacketTime = CurrentTime - LastPacketTime;  // time between received packets
    LastPacketTime = CurrentTime;

    Serial.println(PacketToPacketTime);

    // time out detection (late or lost packets)
    if (PacketToPacketTime > TimeOutReceiving && FirstTime == 0) 
    {
      FirstTime = FirstTime + 1;  // this variable is needed to prevent sequential detection of late packets without any end

      if (FirstTimeRestart > 0)   // when we start the software we also have a late packet, this should not lead to a reset
      {
        FirstTimeRestart = FirstTimeRestart - 1;  // the software is now in normal operation
      }
      else  // we have a real time out
      {
        Output_ShowValue(OCHAN_STATUS, 1);  // visualize the timeout (switch LED on)
        
        //I2S_Restart(); // restart the I2S interface

        rbpush = 0;  // set the ringbuffer indices back to 0
        rbpop = 0;

        radiocount = 0; // we start again at packet number 0

        Output_ShowValue(OCHAN_STATUS, 0); // visualize the timeout (switch LED off)
      }
    }
    else
    {
      FirstTime = 0;  // the software is running with packets in time
    };
    
    
    // read out the radio module with 16 samples
    Radio_ReadSamplesInBuffer(mBuffer, PACKET_LENGTH_SAMPLES);

    // put the samples in the ringbuffer
    for (uint8_t i = 0; i < 16; i++)
    {
      dBuffer[rbpush] = mBuffer[i];
      rbpush++;
      if (rbpush == ringbufferLen) rbpush = 0;
    }
    
    // detection if a command message was received 
    memcpy(&bBuffer, &mBuffer, 8); // but the first 8 bytes to a separate byte buffer

    // have we received a command message instead of music samples?
    if (bBuffer[0] == 255 && bBuffer[1] == 255)
    {
      if (bBuffer[2] == 255 && (bBuffer[3] == AUDIOCHANNEL_INDEX))  // is it for us?
      {
        Output_ShowValue(OCHAN_STATUS, 1);  // visualize that a command message was received (switch LED on)
        
        command = bBuffer[4];   // type of the command     
        if (command == 2)  // command to switch radio channels
        {
          uint8_t RadioChannelIndex = bBuffer[5];  // the radio channel index (0..4) is in byte number 5
          Radio_SetChannel(RadioChannelIndex);     // set the new radio channel (see the Radio_nRF24 library which physical channels can be selected with this)
          Radio_Start(RADIO_MODE_RECEIVER);        // and re-start the radio
          delay(50);
        }

        Output_ShowValue(OCHAN_STATUS, 0);  // visualize that a command message was received (switch LED off)
      }
    }


    // LOW LEVEL DETECTION: silent or no music, so we can make out regular "dirty" reset here
    uint8_t LowLevel = 1;
    for (int16_t i = 0; i < PACKET_LENGTH_SAMPLES; ++i) {
      if ((mBuffer[i] < 0) && (mBuffer[i] * -1 > LowLimit)) LowLevel = 0; // one loud sample in the packet (positive value of the music signal)
      if ((mBuffer[i] > 0) && (mBuffer[i] > LowLimit)) LowLevel = 0;      // one loud sample in the packet (negative value of the music signal)
    }


    if ( (radiocount * PACKET_LENGTH_SAMPLES) > dirtyResetSamples && LowLevel == 1)   // if the music is silent and it is time for the regular "dirty" reset
    {
      radiocount = 0; // set the packet counter back to 0
      
      I2S_Reset();  // reset of the I2S interface (no restart, so no new configuration values)

      Output_ShowEvent(EVENT_DIRTY_RESET, 500); // show that the dirty reset occurred for 500 ms
    }


    Output_Refresh();  // visualize all the values, you have to regularly call this function to refresh the "user interface"

    PacketReceived = 1;  // flag that a packet was received
  }
  

  TimeOutput = micros();  // is it time to output the music signal?
  if (TimeOutput - LastTimeOutput > 120)  // we look every 240 ms. PLS NOTE: you may have to play with this value to get a stable timing
  {
    if (PacketReceived == 1)  // a packet has been received
    {
      // read out the ring buffer with the music samples
      for (uint8_t i = 0; i < 16; i++)
      {
        oBuffer[i] = dBuffer[rbpop];                                                                                     
        rbpop++;
        if (rbpop == ringbufferLen) rbpop = 0;
      }
      
      //Serial.println(TimeOutput - LastTimeOutput);

      //I2S_WriteSamplesFromBuffer(oBuffer, PACKET_LENGTH_SAMPLES);  // put the samples to the I2S interface to play them
      
      LastTimeOutput = TimeOutput;
      PacketReceived = 0;  // set the flag to 0 again
    }

  }
*/




/*
if( Radio_DataReceived() )   // a packet was received
  {
    radiocount++;  // count the packets received, for dirty reset

    CurrentTime = micros();
    PacketToPacketTime = CurrentTime - LastPacketTime;  // time between received packets
    LastPacketTime = CurrentTime;

    Serial.println(PacketToPacketTime);

    // time out detection (late or lost packets)
    if (PacketToPacketTime > TimeOutReceiving && FirstTime == 0) 
    {
      FirstTime = FirstTime + 1;  // this variable is needed to prevent sequential detection of late packets without any end

      if (FirstTimeRestart > 0)   // when we start the software we also have a late packet, this should not lead to a reset
      {
        FirstTimeRestart = FirstTimeRestart - 1;  // the software is now in normal operation
      }
      else  // we have a real time out
      {
        Output_ShowValue(OCHAN_STATUS, 1);  // visualize the timeout (switch LED on)
      }
    }




    Radio_ReadSamplesInBuffer(mBuffer, PACKET_LENGTH_SAMPLES);
    
    for (int i = 0; i < PACKET_LENGTH_SAMPLES; i++) {
        Serial.println(String(mBuffer[i]));
    }
    
  }

*/
if( Radio_DataReceived() )   // a packet was received
  {
   Serial.println("GOT DATA");
   char receivedText[32] = "";
   if (radio.available()) {
    radio.read(&receivedText, sizeof(receivedText));
   }

   delay(500);
  }





}

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
#define AUDIOCHANNEL_INDEX  1

#define PACKET_LENGTH_SAMPLES 16
#define PACKET_LENGTH_BYTES   32

// application software modules needed
#include "BoardPin.h" 
#include "Input.h"
#include "Output.h"   

// libraries needed (in libraries folder)
#include "Stream_I2S.h"  
#include "Radio_nRF24.h" 


// buffers needed, pre-filled with 0
uint8_t byteBuffer[PACKET_LENGTH_BYTES]     = {1};
uint8_t commandMessage[PACKET_LENGTH_BYTES] = {0};


void setup() {
  Serial.begin(115200);

  delay(200);
  
  memset(byteBuffer, '1', PACKET_LENGTH_BYTES);
  
  // initialize Input and Output module
  Input_Init();
  Output_Init();

  Radio_Init();
  Radio_Start(RADIO_MODE_SENDER);  // start the Radio Module for sending


  // these variables are defined in the Stream_I2S library
  AudioMode = AUDIO_MODE_SAMPLER;  // interface will be started to sample music

  samplebuffer_count = 2;   // that is the lowest value possible, for very low latency
  samplebuffer_length = 32;  // also a low value for low latency
  samplerate = 32000;
  audiochannelcount = 1;    // mono
  //I2S_Start();
  delay(100);

  // tell the Audiochannel index to the Output module to be visualized
  Output_SetValue(OCHAN_DEVICENUMBER, AUDIOCHANNEL_INDEX);

  if (RadioRunning == 1)
  {
    // tell the Output module that there isn't an error
    Output_SetValue(OCHAN_ERROR, 0);
    // visualize the Audiochannel index, the command will also visualize all the other values then
    Output_ShowValue(OCHAN_RADIOCHANNEL, RadioChannelIndex);
  }
  else
  {
    // visualize an error, error has the value 1
    Output_ShowValue(OCHAN_ERROR, 1);
  }

}



// helper function to prepare and send a command message packet
void Send_Command(uint8_t CommandAddress, uint8_t CommandByte, uint8_t ValueByte)
{
  // the first 3 bytes are then all = 255
  commandMessage[0] = 255;
  commandMessage[1] = 255;
  commandMessage[2] = 255;
  commandMessage[3] = CommandAddress;  // the Address of the sender which is in that case the Audiochannel index
  commandMessage[4] = CommandByte;     // the type of the command
  commandMessage[5] = ValueByte;       // the value of the command
  
  if (RadioRunning == 1) Radio_WriteBytesFromBuffer(commandMessage, PACKET_LENGTH_BYTES);  // send out the command message
}


// some variables to calculate sending times (not used at the moment)
uint32_t systime_send = 0;
uint32_t Rtime = 0;
uint32_t Ltime = 0;
uint32_t Dtime = 0;


void loop() { 

/*
  // Read 32 bytes from the I2S interface = 16 mono samples
  //I2S_ReadBytesInBuffer(byteBuffer, 32);

  // send out the bytes
  //if (RadioRunning == 1) Radio_WriteBytesFromBuffer(byteBuffer, PACKET_LENGTH_BYTES);
  delayMicroseconds(80);  // this delay is in principle not needed but in some other cases it leads to more stable timing

  Rtime = micros();
  Dtime = Rtime - Ltime;
  Ltime = Rtime;
  Serial.println(Dtime);   // you can print out the time from packet to packet 

  Input_Loop(); // poll the Input module

  int IChannelValueWasChanged = Input_GetChannelOfChangedValue();  // do we have some input (button presses in this case)
  
  if (IChannelValueWasChanged > -1) 
  {
    if (IChannelValueWasChanged == 1) // short press in this case
    {
      RadioChannelIndex++;  // switch to the next radio channel (see the Radio_nRF24 library which physical channels can be selected with this)
      if (RadioChannelIndex >= RADIOCHANNEL_INDEXMAX) RadioChannelIndex = 0;

      Send_Command(AUDIOCHANNEL_INDEX, 2, RadioChannelIndex);  // send out the command for switching the channel on the receiver side
      delay(30);

      Radio_SetChannel(RadioChannelIndex);  // set the new radio channel on the sender side
      Radio_Start(RADIO_MODE_SENDER);       // and re-start the radio
      delay(30);

      Output_ShowValue(OCHAN_RADIOCHANNEL, RadioChannelIndex);  // visualize the new radio channel
    }

    if (IChannelValueWasChanged == 0) // long press
    {
      // not used at the moment
    }
  
  } 

  //int data[3] = {111, 222, 333};
  // Send the 3 integers (6 bytes total)
  //radio.write(data, sizeof(data)); 
*/
  Radio_SetChannel(RadioChannelIndex);  // set the new radio channel on the sender side
  //Radio_Start(RADIO_MODE_SENDER); 


  char text[] = "Hello World";
  radio.write(&text, strlen(text) + 1);
  delay(3000);
}


// output channels to visualize something, but most are not used here
#define OCHAN_ERROR          0
#define OCHAN_DEVICENUMBER   1
#define OCHAN_STATUS         2
#define OCHAN_MODE           3
#define OCHAN_VOLUME         4
#define OCHAN_RADIOCHANNEL   5

#define EVENT_MESSAGE_RECEIVED       1


// we need this for timing if an event occurred which 
// should be visualized with priortity for a certain amount of time
int32_t Output_EventTime = 0;
int32_t Output_EventTimeOut = 0;
int32_t Output_Event = 0;

int Output_Value[8];  // we have 8 values which can be visualized


#include "SevenSegmentDisplay.h"


// this is called once at the beginning, to initiate all visualizing elements
void Output_Init()
{
  // in that case, we initiate a Seven Segment Display
  //SevenSegmentDisplay_Init(PIN_LEDDISPLAY_CLOCK, PIN_LEDDISPLAY_DATA);
};


// set a value to be visualized
void Output_SetValue(int Channel, int Value)
{
  Output_Value[Channel] = Value;
}

// call when an event occurred which should be visualized with priority for a certain amount of time
void Output_ShowEvent(int Event, int EventTimeOut)
{
  Output_EventTime = millis();
  Output_Event = Event;
  Output_EventTimeOut = EventTimeOut;

  // not used in this application
}



// call this regularly to visulize all the values and check if the time has ended for the event to be visualized
void Output_Refresh()
{
  
  if (Output_Event != 0)
  {
    if ((millis() - Output_EventTime) > Output_EventTimeOut)
    {
      // end of visualization of the Event
      //  not used in this application
        
      Output_Event = 0;

    }
  }
  else
  {
    if (Output_Value[OCHAN_ERROR] == 0)
    {
      // in normal mode, show the radio channel as 1, 3, 5, 7, 9 or 0, 2, 4, 6, 8 depending on the device number. Show this on the second figure of the display (10 *)
      //SevenSegmentDisplay_ShowNumber(1000 + 10 * (Output_Value[OCHAN_RADIOCHANNEL] * 2 + 2 - Output_Value[OCHAN_DEVICENUMBER] ) );
    }
    else
    {
      // in case there is some value on the Error-Channel, just show a 0 on the display
      //SevenSegmentDisplay_ShowNumber(0);
    }
  }   

}

// set a value to be visualized
// and visualize it immediately
void Output_ShowValue(int Channel, int Value)
{
  Output_Value[Channel] = Value;
  Output_Refresh();
}

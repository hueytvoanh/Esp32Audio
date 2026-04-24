
// output channels to visualize something, but most are not used here
#define OCHAN_ERROR          0
#define OCHAN_DEVICENUMBER   1
#define OCHAN_STATUS         2
#define OCHAN_MODE           3
#define OCHAN_VOLUME         4
#define OCHAN_RADIOCHANNEL   5

#define EVENT_POWER_ON               1
#define EVENT_DIRTY_RESET            2



// we need this for timing if an event occurred which 
// should be visualized with priortity for a certain amount of time
int32_t Output_EventTime = 0;
int32_t Output_EventTimeOut = 0;
int32_t Output_Event = 0;


int Output_Value[8];  // we have 8 values which can be visualized


// this is called once at the beginning, to initiate all visualizing elements
void Output_Init()
{
  // in that case, we initiate a LED which is active high
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW); 
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
  
  // an Event occurred so visualize this with a LED on
  digitalWrite(PIN_LED, HIGH); 
}



// call this regularly to visulize all the values and check if the time has ended for the event to be visualized
void Output_Refresh()
{
  
  if (Output_Event != 0)
  {
    if ((millis() - Output_EventTime) > Output_EventTimeOut)
    {
      // end of visualization of the Event
      digitalWrite(PIN_LED, LOW); 
        
      Output_Event = 0;

    }
  }
  else
  {
    // no prioritized event to show, so we can show in a normal mode all the values
    if (Output_Value[OCHAN_STATUS] == 1)
    {
      // Value on the Status-Channel is 1
      digitalWrite(PIN_LED, HIGH); 
    }
    else
    {
      // Value on the Status-Channel is 0
      digitalWrite(PIN_LED, LOW); 
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



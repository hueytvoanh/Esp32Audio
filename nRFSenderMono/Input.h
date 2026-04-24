

// we have just one button here, but it can be pressed short and long, so virtually, we have 2 buttons
// that is why we map it to 2 input channels

#define INPUT_VALUES_MAX   2

// our input channels we can poll regularly
int  Input_Value[INPUT_VALUES_MAX];
bool Input_Value_Changed[INPUT_VALUES_MAX];


// the Button library for one button takes over the timing
#include "Button.h"

void Input_Init()
{
  // init button library, the button is connected to ground so pin goes to low when pressed
  Button_Init(PIN_BUTTON, BUTTON_ACTIVE_LOW);

  // init of the input values
  for (int m = 0; m < INPUT_VALUES_MAX ; m++)
  {
    Input_Value_Changed[m] = false;
  }
}


// get the value of the input element - this is not used here
int Input_GetValue(int Channel)
{
  Input_Value_Changed[Channel] = false;
  
  return Input_Value[Channel];
}


// get the channel of the input element with changed value
int Input_GetChannelOfChangedValue()
{
  int Result = -1;
  for (int m = 0; m < INPUT_VALUES_MAX ; m++)
  {
    // go through all input elements - if the channel is found, break the loop
    if (Input_Value_Changed[m] == true) 
    {
      // we have the channel now, so the flag should be set back to inactive
      Input_Value_Changed[m] = false;
      Result = m;
      break;
    }
    
  }
  return Result;
}


// call this regularly
void Input_Loop()
{
  // poll the button
  int button = Button_Loop();

  // a long press occurred
  if (button == 2) // long press
  {
    // set a flag that the value of channel 0 changed
    Input_Value_Changed[0] = true;
  } 

  // a short press occurred
  if (button == 1) // short press
  {
    // set a flag that the value of channel 1 changed
    Input_Value_Changed[1] = true;
  }     
}

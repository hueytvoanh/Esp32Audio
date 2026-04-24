/*
 
Nordic from top, Antenna to the right

GND  VCC         brown red 
CE   CS          green violet
SCK  MOSI        blue  yellow 
MISO IR          orange      
*/

#define pCE        9      // green
#define pCS        10     // violet
#define pSCK       12     // blue
#define pMOSI      11     // yellow   
#define pMISO      13     // orange

// we dont need that in this application but the Radio_nRF24 needs some pin numbers for a second radio module to get compiled
#define pCE2        0    // weiß  green
#define pCS2        0    // grau  white
#define pSCK2       12    // lila  blue
#define pMOSI2      11    // blau  yellow 
#define pMISO2      13    // grün  brown 



// http://wiki.fluidnc.com/en/hardware/ESP32-S3_Pin_Reference
// https://user-images.githubusercontent.com/13697838/230209347-d2b07681-5a45-4b29-bb78-7eb0c7f4c498.png


// I2S2 Pmod module
#define I2S_MCK_IN  37  //  blue
#define I2S_WS_IN   38  //  green
#define I2S_SCK_IN  39  //  yellow
#define I2S_SD_IN   40  //  orange 
  
#define I2S_MCK_OUT 36  //  blue
#define I2S_WS_OUT  35  //  green
#define I2S_SCK_OUT 48  //  yellow
#define I2S_SD_OUT  47  //  orange 

// attach the Grove 7-Segment-Display at Grove connector 18/17
#define PIN_LEDDISPLAY_DATA   17 
#define PIN_LEDDISPLAY_CLOCK  18 

// attach the Button at Grove connector 5/4, and first pin 5, the other wire goes to GND
#define PIN_BUTTON  5


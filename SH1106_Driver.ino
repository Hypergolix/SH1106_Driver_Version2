#include <Wire.h>
#include "Fonts.h"
#include "Command.h"
#include "Graphics.h"

void setup() {
  Wire.begin();

  // INITIALIZE DISPLAY
  singleC_OP(0xae);                  // Command to turn display OFF
  dualCD_OP(0xd5, 0x80);             // Clock ratio 0x0f
  dualCD_OP(0xd3, 0x00);             // 0x00 = Top of screen? 0x10. Think this is visual and not affected by RAM. Display Offset - Double Byte
  singleC_OP(0x40);                  // 0x40 should be top, explains why page is offset weirdly. 0x48. Set start line << CHECK THIS. I think 0x40 should be start
  dualCD_OP(0xad, 0x8b);             // Built-In DC-DC Used - Double Byte
  singleC_OP(0x33);                  // Set Pump Voltage
  singleC_OP(0xc8);                  // Set scan direction
  singleC_OP(0xa1);                  // Segment remap
  
  // BELOW
  dualCD_OP(0xda, 0x12);             // Sequential, 0x12. COM HW CONFIG
  dualCD_OP(0xd9, 0xf1);             // 15 DCLK DisCharge. 1 DCLK PreCharge
  dualCD_OP(0xdb, 0x40);             // FIX THIS - VCOMH.64?
  // CHECK ABOVE

  dualCD_OP(0x81, 0xff);             // Contrast control
  singleC_OP(0xa4);                  // Command to turn display ON? 0xa5. Output RAM << CHECK THIS. Is this necessary? POR == 0xa4
  singleC_OP(0xa6);                  // Normal Mode. 0xa7 - inverse. Set normal display mode

  clrDisplay();                      // What would be useful to draw before the display is turned on?
  singleC_OP(0xaf);                  // Command to turn display ON
}

void loop() {
}

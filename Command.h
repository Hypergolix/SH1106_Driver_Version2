#define DEVICE_ID 0x3C
#define COMMAND 0x00
#define RAMCOMMAND 0x40
// A screen buffer - write to this first then send the array to the screens RAM
byte pageArray[128] = {0};                                  // This may be used to circumvent the page overflowing

// Rename some of these variables
void singleC_OP(byte firstParam) {                          // Single command OP
  Wire.beginTransmission(DEVICE_ID);                        //
  Wire.write(COMMAND);                                      //
  Wire.write(firstParam);                                   // The only command
  Wire.endTransmission();
}

void dualCD_OP(byte firstParam, byte secondParam) {         // Double command OP
  Wire.beginTransmission(DEVICE_ID);                        //
  Wire.write(COMMAND);                                      //
  Wire.write(firstParam);                                   // Usually addresses the register
  Wire.write(secondParam);                                  // DATA for the register
  Wire.endTransmission();
}

void RAM_OP(byte firstParam) {                              // RAM operation + data transfer
  Wire.beginTransmission(DEVICE_ID);                        //
  Wire.write(RAMCOMMAND);                                   // Addresses RAM
  Wire.write(firstParam);                                   // Usually addresses the register
  Wire.endTransmission();                                   //
}

void RAMS_OP() {
  Wire.beginTransmission(DEVICE_ID);                        // Single RAM OP 
  Wire.write(RAMCOMMAND);                                   // Addresses RAM
  Wire.endTransmission();
}

byte read_OP() {                                            // uint8_t byteNumber
  byte reading;
  RAMS_OP();
  Wire.requestFrom(DEVICE_ID, 1);                           // DUMMY READ
  byte dummy = Wire.read();
  RAMS_OP();
  Wire.requestFrom(DEVICE_ID, 1);                           // Request byteNumber amount of bytes
  reading = Wire.read();
  return reading;
}

// This one sets it to the leftmost column position
void columnSet() {                                          // Resets column to the first one. Have arguments for a distance from the left
  //Set Start Column
  singleC_OP(B00010000);                                    // 0000 - HIGHER
  singleC_OP(B00000010);                                    // 0010 - LOWER
}

// Sets the initial column Adress, takes a starting column as an argument
void incrementC(byte increment) {                           // Now takes one arg
  byte lower = increment & B00001111;                       // Works out Lower 4 bits - just formatting
  byte higher = (increment & B11110000) >> 4;               // Works out Higher 4 bits - just formatting
  // Set Start Column
  singleC_OP(B00010000 + higher);                           // 0000
  singleC_OP(B00000000 + lower);                            // 0010
}

// Takes the page as an argument. Always calls column set so that whenever setPage is called it will also begin from the leftmost column
void setPage(byte page) {                                   // Just sets the page and uses the first column
  singleC_OP(0xB0 + page);                                  // , byte data? 0xB0 + page
  // columnSet();                                           // REMOVE?? May cause issues
}

void clrDisplay() {
  // At the end of (128) 132, increment the page but also zero the column address
  for (uint8_t y = 0; y < 8; y++) {                         // Page
    setPage(y);                                             // Just zeroes page address. Is column zeroing necessary?
    for (uint8_t i = 0; i < 132; i++) {                     // Line. 128 in the future or just have a blanking time of 4px?
      RAM_OP(0x00);                                         // Clears the column
    }
  }
}

void writeSpace(uint8_t spaceSize, byte page) {             // Deals with writing spaces
  for (uint8_t y = 0; y < spaceSize; y++) {                 // Iterates for the number of spaces. I think this should be under 256, even with ASCII
    RAM_OP(0x00);                                           // Writes spaces
    pageTable[page]++;
  }
}

void writeLetter(uint16_t startPos, byte page, byte space) {// In the future allow for custom width letters
  byte column = 0x00;
  // * by 8 here instead
  for (uint8_t xPos = 0; xPos < 8; xPos++) {                // Height
    column = fontArray[startPos + xPos];                    // Start position + current column read

    if (column != 0) {                                      // When switching from an 8 wide to a lower width there is visual artifacting due to it not being overwritten.
      // Either have a case like && ifOverwrite which or a separate function that first blanks the whole square then writes over that. It does however mean we will get weird spacing
      // add to the tableArray here! also for space
      RAM_OP(column);                                       // Writes one column to the display
      pageTable[page]++;
    }
  }
  writeSpace(space, page);
  // Later write this to the pageArray and then print that in one go?
}

// Paramater for space width, in pixels. Default = 2. In future could have custom space width. Pass this in writeText instead?
// ADD numbers mode? Would use ascii here but i'm not adding all those characters
void writeText(byte column, byte page, String text) {
  byte space = 2;                                                         // Doesn't need to be hardcoded
  byte increment = 2 + column;                                            // 2 default to account for pixel difference
  setPage(page);                                                          // This one resets the column address, be aware of this
  incrementC(increment);                                                  // Just sets the start column address. Would it be better to only take one number as an argument?

  for (uint8_t stringPos = 0; stringPos < text.length(); stringPos++) {   // Iterates for length of the string
    uint16_t startPos = uint16_t(text.charAt(stringPos));                 // Convert character to byte(number);
    if (startPos > 32 && startPos < 91) {                                 // Parameter for all chars Sends the letter to function - More ASCII have been added
      startPos = (startPos - 33) * 8;
      writeLetter(startPos, page, space);
    } else if (startPos == 32) {                                          // If space
      writeSpace(2, page);                                                // 2 is the spacing
    } else {                                                              // If char not found
      writeLetter(464, page, space);
      // startPos = 464;                                                     // Coord for default char - Change this, default comes as last char
    }
  }
}

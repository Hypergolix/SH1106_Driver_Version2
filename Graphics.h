// UPDATE THIS TO BYTE ARRAY
/*
  bool RAMArray[64][128] = {0};                         // Row by column. This array represents the total Pixels, not RAM. Change name. Can be used for full screen graphics
  // bool testArray[8][128] = {0};                         // Init with 0s - not sure why this is here

  void drawGraphic() {                                  // Add left, top increment later
  for (int page = 0; page < 8; page++) {              // Iterates for each page
    setPage(page);
    columnSet();
    for (int i = 0; i < 128; i++) {                   // Line. Iterate for entire array, could optimse to only iterate for the size of graphic - would then need to account for column address
      byte column = B00000000;                        // Resets
      for (int yPos = 0; yPos < 8; yPos++) {          // Height
        column = column | ((RAMArray[yPos + (page * 8)][i]) << yPos); // OLD = (7 - yPos). NEW yPos
      }
      RAM_OP(column);                                 // Writes one column to the display
    }
  }
  }
*/
// colOffset is hardcoded. Could do with another array that deals with starting column addresses
// Read-Modify-Write, blinking cursor. Place this before other if including it
void RMW(byte columnCounter, byte pageCounter , byte colOffset = 0x00) {
  setPage(pageCounter);                               // Sets page address - Needs start/end address? (i + startPage)
  incrementC(colOffset + 2);                          // Sets column address (0) - first. + 2 to compensate for the RAM difference
  for (uint8_t y = 0; y < columnCounter; y++ ) {
    singleC_OP(0xe0);                                 // Read-Modify-Write - 0xE0
    byte readByte = read_OP();                        // Request 1 byte

    byte reverseByte = ~readByte;                     // In theory inverts the byte

    RAM_OP(reverseByte);

    // Could nest functions so that different modifications can be made.
    // IDEALLY this would keep reading and find the start and end of the word on the current page - Would not be the accurate length but good enough
    // Could add a buffer anyway as to add some padding to each end of the word
  }
  singleC_OP(0xee);                                   // END COMMAND
}

// Other draw fucntions here?

#include <Arduino.h>
/*************************************************** 
  This is an example for the Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

#define CLK   18 // SPI Clock, shared with SD card
#define MISO  16 // Input data, from VS1053/SD card
#define MOSI  19 // Output data, to VS1053/SD card
#define RST   20 // VS1053 reset pin (output)
#define CS    17 // VS1053 chip select pin (output)
#define XDCS  14 // VS1053 Data/command select pin (output)
#define SDCS  13 // Card chip select pin
#define DREQ  5 // VS1053 Data request, ideally an Interrupt pin
Adafruit_VS1053_FilePlayer mp3 = Adafruit_VS1053_FilePlayer(RST, CS, XDCS, DREQ, SDCS);


/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
void setup() {
    Serial.begin(9600);
    while (!Serial) {
      delay(1);
    }


    SPI.setRX(MISO);
    SPI.setTX(MOSI);
    SPI.setSCK(CLK); // Set default CS for SPI
    SPI.begin();
    
    Serial.println("Adafruit VS1053 Simple Test");

    if (!mp3.begin()) { // initialise the music player
      Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
      while (1);
    }
    Serial.println(F("VS1053 found"));
    
    if (!SD.begin(SDCS)) {
        Serial.println(F("SD failed, or not present"));
        while (1);  // don't do anything more
    }

    // list files
    printDirectory(SD.open("/"), 0);
    
    // Set volume for left, right channels. lower numbers == louder volume!
    mp3.setVolume(20,20);

    // Timer interrupts are not suggested, better to use DREQ interrupt!
    //mp3.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

    // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
    // audio playing
    mp3.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
    digitalPinToInterrupt(DREQ);


    // Play one file, don't return until complete
    Serial.println(F("Playing track 001"));
    mp3.playFullFile("/track001.mp3");
    // Play another file in the background, REQUIRES interrupts!
    Serial.println(F("Playing track 002"));
    mp3.startPlayingFile("/track002.mp3");
}

void loop() {
  // File is playing in the background
  if (mp3.stopped()) {
    Serial.println("Done playing music");
    while (1) {
      delay(10);  // we're done! do nothing...
    }
  }
  if (Serial.available()) {
    char c = Serial.read();
    
    // if we get an 's' on the serial console, stop!
    if (c == 's') {
      mp3.stopPlaying();
    }
    
    // if we get an 'p' on the serial console, pause/unpause!
    if (c == 'p') {
      if (! mp3.paused()) {
        Serial.println("Paused");
        mp3.pausePlaying(true);
      } else { 
        Serial.println("Resumed");
        mp3.pausePlaying(false);
      }
    }
  }

  delay(100);
}


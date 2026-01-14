#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

#define CLK   18 // SPI Clock, shared with SD card
#define MISO  16 // Input data, from VS1053/SD card
#define MOSI  19 // Output data, to VS1053/SD card
#define RST   20 // VS1053 reset pin (output)
#define CS    17 // VS1053 chip select pin (output)
#define XDCS  22 // VS1053 Data/command select pin (output)
#define SDCS  21 // Card chip select pin
#define DREQ  26 // VS1053 Data request, ideally an Interrupt pin
Adafruit_VS1053_FilePlayer mp3 = Adafruit_VS1053_FilePlayer(RST, CS, XDCS, DREQ, SDCS);

void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) break;
     for (uint8_t i = 0; i < numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
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
  delay(1000);

  SPI.setRX(MISO);
  SPI.setTX(MOSI);
  SPI.setSCK(CLK);
  SPI.begin();

  Serial.println("Adafruit VS1053 Library Test");
  if (!mp3.begin()) Serial.println("VS1053 NOT found");

  mp3.sineTest(0x44, 1500); 
 
  if (!SD.begin(SDCS)) Serial.println("SD NOT OK!");
  printDirectory(SD.open("/"), 0);

  /***** Two interrupt options! *******/ 
  // This option uses timer0, this means timer1 & t2 are not required
  // (so you can use 'em for Servos, etc) BUT millis() can lose time
  // since we're hitchhiking on top of the millis() tracker
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT);
  
  // This option uses a pin interrupt. No timers required! But DREQ
  // must be on an interrupt pin. For Uno/Duemilanove/Diecimilla
  // that's Digital #2 or #3
  // See http://arduino.cc/en/Reference/attachInterrupt for other pins
  // *** This method is preferred
  
  // digitalPinToInterrupt(5)
  // digitalPinToInterrupt(DREQ)
  // 5
  if (! mp3.useInterrupt(VS1053_FILEPLAYER_PIN_INT)) Serial.println("DREQ pin is not an interrupt pin");
  mp3.setVolume(1,100);
}

void loop() {
  // Sync
  Serial.println("Trying file 1");
  if(! mp3.playFullFile("/TRACK001.MP3")) {
    Serial.println("Could not open file 1");
    while (1);
  }

  // Async
  Serial.println("Trying file 2");
  if (! mp3.startPlayingFile("/TRACK002.MP3")) {
    Serial.println("Could not open file 2");
    while (1);
  }
  Serial.println("Started playing");
  while (mp3.playingMusic) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Done playing music");
}


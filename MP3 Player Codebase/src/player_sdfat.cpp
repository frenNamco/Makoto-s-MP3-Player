#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SdFat.h>

// VS1053 + SD SPI pins
#define CLK   18
#define MISO  16
#define MOSI  19

#define RST   20
#define CS    17
#define XDCS  22
#define SDCS  21
#define DREQ  26

// SdFat object
SdFat SD;

// VS1053 object
Adafruit_VS1053_FilePlayer mp3 =
    Adafruit_VS1053_FilePlayer(RST, CS, XDCS, DREQ, SDCS);

// Recursive directory print
void printDirectory(FsFile dir, int numTabs) {
  while (true) {
    FsFile entry = dir.openNextFile();
    if (!entry) break;

    for (uint8_t i = 0; i < numTabs; i++) Serial.print('\t');

    char name[64];
    entry.getName(name, sizeof(name));
    Serial.print(name);

    if (entry.isDir()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      Serial.print("\t\t");
      Serial.println(entry.size());
    }

    entry.close();
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(1);
  delay(1000);

  Serial.println("VS1053 + SdFat Test");

  // Configure SPI for Pico
  SPI.setRX(MISO);
  SPI.setTX(MOSI);
  SPI.setSCK(CLK);
  SPI.begin();

  // Initialize VS1053
  if (!mp3.begin()) {
    Serial.println("VS1053 NOT found");
    while (1);
  }

  // Initialize SD card with SdFat
  if (!SD.begin(SDCS)) {
    Serial.println("SD init failed!");
    while (1);
  }

  Serial.println("SD OK!");

  FsFile root = SD.open("/");
  printDirectory(root, 0);
  root.close();

  // Enable interrupt-driven playback
  if (!mp3.useInterrupt(VS1053_FILEPLAYER_PIN_INT)) {
    Serial.println("DREQ pin is not interrupt capable");
  }

  mp3.setVolume(20, 20);
}

void loop() {
  // Serial.println("Trying TRACK001.MP3");

  // if (!mp3.playFullFile("/TRACK001.MP3")) {
  //   Serial.println("Could not open TRACK001.MP3");
  //   while (1);
  // }

  Serial.println("Trying ミッドナイト・ランデブー.mp3");

  if (!mp3.startPlayingFile("/ミッドナイト・ランデブー.mp3")) {
    Serial.println("Could not open ミッドナイト・ランデブー.mp3");
    while (1);
  }

  Serial.println("Started playing");

  while (mp3.playingMusic) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nDone playing music");
}
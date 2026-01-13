#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <SdFat.h>

SdFat SD;  // Must be capital SD

#include <Adafruit_VS1053.h>

#define SD_CS   17
#define DREQ    6
#define XRST    7
#define XCS     8
#define XDCS    9
#define SCK     18
#define MOSI    19
#define MISO    16

Adafruit_VS1053 musicPlayer = 
    Adafruit_VS1053(MOSI, MISO, SCK, XRST, XCS, XDCS, DREQ);
    

void setup() {
    Serial.begin(115200);
    delay(5000); // Give serial time to initialize
    // Serial.println("Starting setup...");
    
    // Set up SPI pins for RP2040
    SPI.setRX(MISO);
    SPI.setTX(MOSI);
    SPI.setSCK(SCK);
    SPI.begin();

    if (!SD.begin(SD_CS,SD_SCK_MHZ(20))) {
        SD.initErrorPrint(&Serial);
        while (true) {
            Serial.println("SD Failed");
        }
    }
    
    if (!musicPlayer.begin()) {
        Serial.println("VS1053 not found");
        while(true) {
            Serial.println("Fuck");
        }
    } else {
        Serial.println("VS1053 found!");
        uint16_t val = musicPlayer.sciRead(VS1053_REG_STATUS);
        Serial.print("Status register: 0x");
        Serial.println(val, HEX);
    }
    
    Serial.println("VS1053 begin() succeeded!");
    delay(100); // Give it time to stabilize
    
    // Now test communication
    Serial.println("Testing VS1053 communication...");
    
    // Try to read the MODE register (should NOT be 0xFFFF)
    uint16_t mode = musicPlayer.sciRead(VS1053_REG_MODE);
    Serial.print("Mode register: 0x");
    Serial.println(mode, HEX);
    
    // Try to read the STATUS register
    uint16_t status = musicPlayer.sciRead(VS1053_REG_STATUS);
    Serial.print("Status register: 0x");
    Serial.println(status, HEX);
    
    // Try to read the VOLUME register
    uint16_t volume = musicPlayer.sciRead(VS1053_REG_VOLUME);
    Serial.print("Volume register: 0x");
    Serial.println(volume, HEX);
    
    // Set and read back volume to test write
    musicPlayer.sciWrite(VS1053_REG_VOLUME, 0x2020);
    delay(10);
    volume = musicPlayer.sciRead(VS1053_REG_VOLUME);
    Serial.print("Volume after write: 0x");
    Serial.println(volume, HEX);
    
    // Check chip version
    uint16_t chipver = musicPlayer.sciRead(VS1053_REG_STATUS);
    Serial.print("Chip version: ");
    Serial.println((chipver >> 4) & 0x0F);
    
}

void loop() {
    Serial.println("Working...");
    delay(1000);
}
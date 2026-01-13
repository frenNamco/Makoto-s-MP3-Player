#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

SdFat SD;
FsFile file;

#define XCS     8
#define XRST    7
#define DREQ    6
#define XDCS    9
#define SD_CS   17

void playMP3(const char* filename) {
    if (!file.open(filename, O_READ)) {
        Serial.println("File open failed");
        return;
    }
    
    uint8_t buffer[32];
    
    Serial.println("Playing (blind - no feedback)...");
    
    while (file.available()) {
        // Wait for chip to be ready
        while (!digitalRead(DREQ));
        
        // Read chunk from SD
        int bytesRead = file.read(buffer, sizeof(buffer));
        
        // Send to VS1003
        digitalWrite(XDCS, LOW);
        for (int i = 0; i < bytesRead; i++) {
            SPI.transfer(buffer[i]);
        }
        digitalWrite(XDCS, HIGH);
    }
    
    file.close();
    Serial.println("Playback complete");
}


void sendCommand(uint8_t address, uint16_t data) {
    while (!digitalRead(DREQ));
    digitalWrite(XCS, LOW);
    SPI.transfer(0x02); // Write command
    SPI.transfer(address);
    SPI.transfer(data >> 8);
    SPI.transfer(data & 0xFF);
    digitalWrite(XCS, HIGH);
}
void setup() {
    Serial.begin(115200);
    delay(2000);
    
    // Setup pins
    pinMode(XCS, OUTPUT);
    pinMode(XDCS, OUTPUT);
    pinMode(XRST, OUTPUT);
    pinMode(DREQ, INPUT);
    pinMode(SD_CS, OUTPUT);
    
    digitalWrite(XCS, HIGH);
    digitalWrite(XDCS, HIGH);
    digitalWrite(SD_CS, HIGH);
    
    // Reset VS1003
    digitalWrite(XRST, LOW);
    delay(100);
    digitalWrite(XRST, HIGH);
    delay(100);
    
    // Wait for DREQ
    while (!digitalRead(DREQ));
    
    // Setup SPI
    SPI.setRX(16);
    SPI.setTX(19);
    SPI.setSCK(18);
    SPI.begin();
    
    // Initialize SD
    if (!SD.begin(SD_CS, SD_SCK_MHZ(20))) {
        Serial.println("SD failed");
        while(1);
    }
    
    // Blindly send initialization commands (can't verify, but might work)
    sendCommand(0x00, 0x0804); // Mode: SM_SDINEW, SM_RESET
    delay(10);
    sendCommand(0x03, 0x6000); // Clock
    sendCommand(0x0B, 0x2020); // Volume
    
    Serial.println("Setup complete - attempting blind playback");
    
    // Try to play a file
    playMP3("test.mp3");
}



void loop() {}
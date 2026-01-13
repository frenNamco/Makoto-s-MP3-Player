#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

#define SD_CS   17
#define SCK     18
#define MOSI    19
#define MISO    16

SdFs sd;

void listDir(const char *path, int depth) {
    SdFile file;
    SdFile dir;
    
    if (!dir.open(path)) {
        Serial.println("Failed to open directory");
    } 

    char name[260];
    while (file.openNext(&dir,O_RDONLY)) {
        file.getName(name, sizeof(name));
        
        for (int i = 0; i < depth; i++) Serial.print("-");
        if (file.isDir()) {
            size_t pathLen = strlen(path) + strlen(name);
            char newPath[pathLen + 1];

            strcpy(newPath, path);
            if (depth > 0) strcat(newPath, "/");           
            strcat(newPath, name);


            Serial.println(newPath);

            listDir(newPath, depth + 1);

        } else {
            Serial.println(name);
        }
        
        file.close();
    }
    
}

void setup() {
    Serial.begin(115200);
    delay(2500);

    SPI.setRX(MISO);
    SPI.setTX(MOSI);
    SPI.setSCK(SCK);
    SPI.begin();

    if (!sd.begin(SD_CS,SD_SCK_MHZ(4))) {
        sd.initErrorPrint(&Serial);
        while (true) {
        }
    }
    
    Serial.println("SD Init Success");

    // int sdClockSpeed = 50;
    // bool sdInit = sd.begin(SD_CS,SD_SCK_MHZ(sdClockSpeed));

    // while (!sdInit) {
    //     Serial.println("SD Init Failed");
    //     if (sdClockSpeed > 0) {
    //         sdClockSpeed--;
    //     } else {
    //         sdClockSpeed = 50;
    //     }
    //     sdInit = sd.begin(SD_CS,SD_SCK_MHZ(sdClockSpeed));
    // }
    
    listDir("/", 0);
    // listDir("/Untitled Folder");
}

void loop() {
    // // Open file for writing (create if it doesn't exist, append to end)
    // if (file.open("test.txt", FILE_WRITE)) {
    //     file.println("Hello, world!");  // write a line
    //     file.close();                    // always close the file
    //     Serial.println("File written successfully");
    // } else {
    //     Serial.println("Failed to open file for writing");
    // }

    // delay(2000);  // optional delay
}
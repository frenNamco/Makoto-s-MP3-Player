#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SdFat.h>
#include <vector>
#include <cstring>

using namespace std;

#ifndef MusicPlayer_H
#define MusicPlayer_H

#define CLK   18
#define MISO  16
#define MOSI  19

#define RST   20
#define CS    17
#define XDCS  22
#define SDCS  21
#define DREQ  26

#define W_BUTTON_PIN  15
#define R_BUTTON_PIN  14
#define G_BUTTON_PIN  13
#define B_BUTTON_PIN  12
#define Y_BUTTON_PIN  11

extern SdFat SD;

class MusicPlayer {
    private:
    
    const size_t MAX_NAME_SIZE = 64;
    char currentDir[64] = "/";
    
    int wBtnPress, rBtnPress, gBtnPress, bBtnPress;
    
    public:
    Adafruit_VS1053_FilePlayer mp3 = Adafruit_VS1053_FilePlayer(RST, CS, XDCS, DREQ, SDCS);
    vector<char*> currentDirList;
    int playingItemIndex = 0;
    int selectedItemIndex = 0;

    MusicPlayer() {};

    bool setup() {
        // Button Initialization
        pinMode(W_BUTTON_PIN, INPUT_PULLUP);
        pinMode(R_BUTTON_PIN, INPUT_PULLUP);
        pinMode(G_BUTTON_PIN, INPUT_PULLUP);
        pinMode(B_BUTTON_PIN, INPUT_PULLUP);

        // SPI Initialization
        SPI.setRX(MISO);
        SPI.setTX(MOSI);
        SPI.setSCK(CLK);
        SPI.begin();

        if (!mp3.begin()) {
            Serial.println("VS1053 Error");
            return false;
        }

        
        if (!SD.begin(SDCS)) {
            Serial.println("SD Error");
            return false;
        }
        
        if (!mp3.useInterrupt(VS1053_FILEPLAYER_PIN_INT)) {
            Serial.println("DREQ pin is not interrupt capable");
        }

        mp3.setVolume(75, 75);

        return true;
    }

    void printCurrentDirectory() {
        FsFile dir = SD.open(currentDir);

        int i = 0;
        while(true) {
            FsFile entry = dir.openNextFile();
            if (!entry) break;

            if (i == selectedItemIndex) Serial.print("• ");

            char name[MAX_NAME_SIZE];
            entry.getName(name, MAX_NAME_SIZE);
            Serial.println(name);

            entry.close();
            i++;
        }

        dir.close();
    }

    void populateCurrentDirList() {
        for (char* name : currentDirList) delete[] name;
        currentDirList.clear();

        FsFile dir = SD.open(currentDir);

        while(true) {
            FsFile entry = dir.openNextFile();
            if (!entry) break;

            char* name = new char[MAX_NAME_SIZE];
            entry.getName(name, MAX_NAME_SIZE);
            currentDirList.push_back(name);

            entry.close();
        }

        dir.close();
    }

    bool checkWhiteButtonPress() {
        wBtnPress = !(digitalRead(W_BUTTON_PIN));

        return wBtnPress;
    }

    bool checkRedButtonPress() {
        rBtnPress = !(digitalRead(R_BUTTON_PIN));

        return rBtnPress;
    }

    bool checkGreenButtonPress() {
        gBtnPress = !(digitalRead(G_BUTTON_PIN));

        return gBtnPress;
    }

    bool checkBlueButtonPress() {
        bBtnPress = !(digitalRead(B_BUTTON_PIN));

        return bBtnPress;
    }

    void selectedItemPath(char *path) {
        strcat(path, currentDir);
        strcat(path, currentDirList[selectedItemIndex]);
    };
};

#endif // !MusicPlayer_H
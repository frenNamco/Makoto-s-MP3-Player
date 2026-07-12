#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SdFat.h>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <LittleFS.h>
#include <ArduinoJson.h>

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

//TODO: Change pin names to their func not color
#define W_BUTTON_PIN    15
#define R_BUTTON_PIN    14
#define G_BUTTON_PIN    13
#define B_BUTTON_PIN    12
#define Y1_BUTTON_PIN   11
#define Y2_BUTTON_PIN   10

extern SdFat SD;

class MusicPlayer {
    private:

    Adafruit_VS1053_FilePlayer mp3 = Adafruit_VS1053_FilePlayer(RST, CS, XDCS, DREQ, SDCS);

    static constexpr size_t MAX_NAME_SIZE = 64;
    char currentDir[MAX_NAME_SIZE] = "/";
    const char* parentDir = "";
    JsonDocument foldersDoc;
    String foldersFilePath = "/folders.json";
    
    int wBtnPress, rBtnPress, gBtnPress, bBtnPress, y1BtnPress, y2BtnPress;
    
    int currentVolume = 75;
    int volumeChange = 5;
    
    unordered_map<int, unsigned long> previousSwitchTime;
    unordered_map<int, int> currentButtonState;
    const unsigned long delay = 100;
    
    public:
    
    vector<const char*> currentDirList;
    bool isWaitingForMusic = true;
    bool isPlayingMusic = false;

    //TODO: Instead of checking indices compare paths to confirm if you're playing or pausing
    int playingItemIndex = 0;
    int selectedItemIndex = 0;
    const char* selectedItemPath = "";

    MusicPlayer() {};

    bool setup();
    void updateStatus();

    void populateCurrentDirList();
    void formatCurrentDirList();
    void printCurrentDirectory();

    void addFoldertoJSON(FsFile dir, const char* parentDir);
    void saveJSON();
    void testJSON();

    //TODO: Make a function that gets the current directory whenever a directory item is clicked

    
    bool checkButtonPress(int buttonPin);


    //TODO: Needs different implementation to go with new JSON layout
    void getItemPath(char *path) {
        strcat(path, currentDir);
        strcat(path, currentDirList[selectedItemIndex]);
    }


    void playAndPause();
    void playSong();
    void endSong();
    void goUpDirList();
    void goDownDirList();
    void increaseVolume();
    void decreaseVolume();

};

#endif // !MusicPlayer_H
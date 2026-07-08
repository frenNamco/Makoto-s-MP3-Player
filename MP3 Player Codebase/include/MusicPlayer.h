#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SdFat.h>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <cstdio>
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

#define W_BUTTON_PIN    15
#define R_BUTTON_PIN    14
#define G_BUTTON_PIN    13
#define B_BUTTON_PIN    12
#define Y1_BUTTON_PIN   11
#define Y2_BUTTON_PIN   10

extern SdFat SD;

class MusicPlayer {
    private:
    
    static constexpr size_t MAX_NAME_SIZE = 64;
    char currentDir[MAX_NAME_SIZE] = "/";
    
    int wBtnPress, rBtnPress, gBtnPress, bBtnPress, y1BtnPress, y2BtnPress;

    int currentVolume = 75;
    int volumeChange = 5;

    unordered_map<int, unsigned long> previousSwitchTime;
    unordered_map<int, int> currentButtonState;
    const unsigned long delay = 100;

    JsonDocument doc;
    JsonArray folders = doc["folders"].to<JsonArray>();
    
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
        pinMode(Y1_BUTTON_PIN, INPUT_PULLUP);
        pinMode(Y2_BUTTON_PIN, INPUT_PULLUP);

        previousSwitchTime[W_BUTTON_PIN] = 0;
        previousSwitchTime[R_BUTTON_PIN] = 0;
        previousSwitchTime[G_BUTTON_PIN] = 0;
        previousSwitchTime[B_BUTTON_PIN] = 0;
        previousSwitchTime[Y1_BUTTON_PIN] = 0;
        previousSwitchTime[Y2_BUTTON_PIN] = 0;

        currentButtonState[W_BUTTON_PIN] = 0;
        currentButtonState[R_BUTTON_PIN] = 0;
        currentButtonState[G_BUTTON_PIN] = 0;
        currentButtonState[B_BUTTON_PIN] = 0;
        currentButtonState[Y1_BUTTON_PIN] = 0;
        currentButtonState[Y2_BUTTON_PIN] = 0;

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

        if(!LittleFS.begin()) {
            Serial.println("Mounting failure; formatting");
            LittleFS.format();
            LittleFS.begin();
        }
        
        if (!mp3.useInterrupt(VS1053_FILEPLAYER_PIN_INT)) {
            Serial.println("DREQ pin is not interrupt capable");
        }

        mp3.setVolume(currentVolume, currentVolume);

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

    void addFoldertoJSON(FsFile dir, const char* parentDir) {
        // Create a folder JSON object for the current director and add it to the folders array
        JsonObject folder = folders.add<JsonObject>();  

        // Get the name of the current directory, concat "/" at the end, and add it to the folder object as its name
        char directoryName[MAX_NAME_SIZE];
        dir.getName(directoryName, sizeof(directoryName));
        snprintf(directoryName, sizeof(directoryName), "%s%s", directoryName, "/");
        folder["name"] = String(directoryName);

        // Add the parent directory for the folder
        folder["parent_dir"] = String(parentDir);

        // Add an array called items to store the items of the current directory
        JsonArray items = folder["items"].to<JsonArray>();

        // Loop that goes through all items of the current directory
        while (true) {
            // Open an item in the current directory and break if there are no more items
            FsFile item = dir.openNextFile();
            if (!item) break;

            // Get its name
            char name[MAX_NAME_SIZE];
            item.getName(name, sizeof(name));

            // If an item is a directory, concat "/" at the end and add it as a folder to the folders array
            if (item.isDir()) {
                snprintf(name, sizeof(name), "%s%s", name, "/");
                addFoldertoJSON(item, directoryName);
            }

            // Add the name of the item to the array
            items.add(name);

            item.close();
        }

    }

    void saveJSON() {
        File pathsJSONFile = LittleFS.open("/paths.json", "w");
        if (!pathsJSONFile) return;
        serializeJson(doc, pathsJSONFile);
        pathsJSONFile.close();
        doc.clear();
        folders = doc["folders"].to<JsonArray>();
    }

    void testJSON() {
        File pathsJSONFile = LittleFS.open("/paths.json", "r");

        if (!pathsJSONFile) {
            Serial.println("failed to open file");
            return;
        }

        DeserializationError err = deserializeJson(doc, pathsJSONFile);
        pathsJSONFile.close();

        if (err) {
            Serial.print("Parse failed: ");
            Serial.println(err.c_str());
        }

        serializeJsonPretty(doc, Serial);
        Serial.println();
    }

    bool checkButtonPress(int buttonPin) {
        int reading = !(digitalRead(buttonPin)); // Get reading from digital pin
        
        // get the current time and check if the button switched after a delay has passed
        unsigned long currentTime = millis();
        if (currentTime - previousSwitchTime[buttonPin] >= delay) {    
            // check if the button has switched states    
            if (reading != currentButtonState[buttonPin]) {
                currentButtonState[buttonPin] = reading; // save the reading as the current button state
                previousSwitchTime[buttonPin] = currentTime;   

                // check if the reading indicates a button press
                if (reading == true) {
                    return true;  
                } 
            }
        }

        return false;
    }

    // bool checkWhiteButtonPress() {
    //     wBtnPress = !(digitalRead(W_BUTTON_PIN));
    // 
    //     return wBtnPress;
    // }
    //
    // bool checkRedButtonPress() {
    //     rBtnPress = !(digitalRead(R_BUTTON_PIN));
    //
    //     return rBtnPress;
    // }
    //
    // bool checkGreenButtonPress() {
    //     gBtnPress = !(digitalRead(G_BUTTON_PIN));
    //
    //     return gBtnPress;
    // }
    //
    // bool checkBlueButtonPress() {
    //     bBtnPress = !(digitalRead(B_BUTTON_PIN));
    //
    //     return bBtnPress;
    // }
    //
    // bool checkYel1ButtonPress() {
    //     y1BtnPress = !(digitalRead(Y1_BUTTON_PIN));
    //
    //     return y1BtnPress;
    // }
    //
    // bool checkYel2ButtonPress() {
    //     y2BtnPress = !(digitalRead(Y2_BUTTON_PIN));
    //
    //     return y2BtnPress;
    // }

    void selectedItemPath(char *path) {
        strcat(path, currentDir);
        strcat(path, currentDirList[selectedItemIndex]);
    }

    void decreaseVolume() {
        currentVolume += volumeChange;
        
        Serial.printf("Current volume: %d", currentVolume);
        Serial.println("");
        mp3.setVolume(currentVolume, currentVolume);
    }

    void increaseVolume() {
        currentVolume -= volumeChange;
        
        Serial.printf("Current volume: %d", currentVolume);
        Serial.println("");
        mp3.setVolume(currentVolume, currentVolume);
    }

};

#endif // !MusicPlayer_H
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SdFat.h>
#include <vector>
#include <cstring>

// VS1053 + SD SPI pins
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

using namespace std;

// SdFat object
SdFat SD;
char currentDir[64] = "/";
int selectedItem = 0;

// VS1053 object
Adafruit_VS1053_FilePlayer mp3 = Adafruit_VS1053_FilePlayer(RST, CS, XDCS, DREQ, SDCS);

const size_t MAX_NAME_SIZE = 64;
vector<char*> currentDirList;

// Recursive directory print
void printDirectory(FsFile dir, int numTabs) {
  int i = 0;

  while (true) {
    FsFile entry = dir.openNextFile();
    if (!entry) break;

    for (uint8_t i = 0; i < numTabs; i++) Serial.print('\t');

    if (i == selectedItem) Serial.print("• ");

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
    i++;
  }
}

void populateCurrentDirList(FsFile dir) {
  for (char* name : currentDirList) delete[] name;
  currentDirList.clear();

  while (true) {
    FsFile entry = dir.openNextFile();
    if (!entry) break;

    char* name = new char[MAX_NAME_SIZE];
    entry.getName(name, MAX_NAME_SIZE);
    currentDirList.push_back(name);

    entry.close();
  }
}

void checkButtonPress(int &wBtnPress, int &rBtnPress, int &gBtnPress, int &bBtnPress) {
  wBtnPress = !(digitalRead(W_BUTTON_PIN));
  rBtnPress = !(digitalRead(R_BUTTON_PIN));
  gBtnPress = !(digitalRead(G_BUTTON_PIN));
  bBtnPress = !(digitalRead(B_BUTTON_PIN));
}

void selectedItemPath(char *path) {
  strcat(path, currentDir);
  strcat(path, currentDirList[selectedItem]);
}

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(1);
  delay(1000);

  // Button initialization
  pinMode(W_BUTTON_PIN, INPUT_PULLUP);
  pinMode(R_BUTTON_PIN, INPUT_PULLUP);
  pinMode(G_BUTTON_PIN, INPUT_PULLUP);
  pinMode(B_BUTTON_PIN, INPUT_PULLUP);

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

  FsFile root = SD.open(currentDir);
  printDirectory(root, 0);
  populateCurrentDirList(root);
  root.close();

  

  // Enable interrupt-driven playback
  if (!mp3.useInterrupt(VS1053_FILEPLAYER_PIN_INT)) {
    Serial.println("DREQ pin is not interrupt capable");
  }

  mp3.setVolume(20, 20);
}

int wBtnPress = 0;
int rBtnPress = 0;
int gBtnPress = 0;
int bBtnPress = 0;

bool musicStarted = false;
bool currentTrackPaused = false;

void loop() {
  char path[128];
  selectedItemPath(path);
  Serial.println(path);

  Serial.print("Trying ミッドナイト・ランデブー.mp3");

  
  if (!musicStarted) {
    if (!mp3.startPlayingFile(path)) {
      Serial.print("Could not open ");
      Serial.println(currentDirList[selectedItem]);
      while (1);
    } else {
      musicStarted = true;
      currentTrackPaused = false;
    }
  }
  
  Serial.println("Started playing");
  
  while (mp3.playingMusic || musicStarted) {
    checkButtonPress(wBtnPress, rBtnPress, gBtnPress, bBtnPress);

    // Pause button func
    if (wBtnPress) {
      Serial.println("White button press");

      mp3.pausePlaying(!currentTrackPaused);

      currentTrackPaused = !currentTrackPaused;

      delay(50);
    }

    if (rBtnPress) {
      Serial.println("Red button press");
      delay(50);
    }

    if (gBtnPress) {
      Serial.println("Green button press");

      if (selectedItem >= currentDirList.size() - 1) {
        selectedItem = 0;
      } else {
        selectedItem++;
      }

      Serial.println(selectedItem);

      FsFile dir = SD.open(currentDir);
      printDirectory(dir, 0);
      dir.close();

      delay(50);
    }

    if (bBtnPress) {
      Serial.println("Blue button press");

      if (selectedItem <= 0) {
        selectedItem = currentDirList.size() - 1;
      } else {
        selectedItem--;
      }

      Serial.println(selectedItem);

      FsFile dir = SD.open(currentDir);
      printDirectory(dir, 0);
      dir.close();

      delay(50);
    }
  }

  Serial.println("\nDone playing music");
}
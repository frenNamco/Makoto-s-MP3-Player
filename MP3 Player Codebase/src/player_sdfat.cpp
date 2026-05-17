#include <MusicPlayer.h>

using namespace std;

SdFat SD;
MusicPlayer player;


void setup() {
  Serial.begin(9600);
  while (!Serial) delay(1);
  delay(1000);

  
  Serial.println("VS1053 + SdFat Test");

  if (!player.setup()) {
    while(1);
  }

  player.printCurrentDirectory();
  player.populateCurrentDirList();
}


void loop() {
  while(player.mp3.stopped() && !(player.mp3.paused())) {

    if (player.checkWhiteButtonPress()) { // Pause button func
      Serial.println("White button press");
      delay(250);

    } else if (player.checkRedButtonPress()) {
      Serial.println("Red button press");
      char path[128] = "";
      player.selectedItemPath(path);
      if (!player.mp3.startPlayingFile(path)) {
        Serial.print("Could not open ");
        Serial.println(path);
        while (1);
      }

      delay(250);
    } else if (player.checkGreenButtonPress()) { // Goes down the list of items
      Serial.println("Green button press");

      if (player.selectedItemIndex >= player.currentDirList.size() - 1) {
        player.selectedItemIndex = 0;
      } else {
        player.selectedItemIndex++;
      }

      Serial.println(player.selectedItemIndex);
      player.printCurrentDirectory();

      delay(250);
    } else if (player.checkBlueButtonPress()) { // Goes up the list of items
      Serial.println("Blue button press");

      if (player.selectedItemIndex <= 0) {
        player.selectedItemIndex = player.currentDirList.size() - 1;
      } else {
        player.selectedItemIndex--;
      }

      Serial.println(player.selectedItemIndex);
      player.printCurrentDirectory();

      delay(250);
    }


  }

  
  while (player.mp3.playingMusic || player.mp3.paused()) {
    
    if (player.checkWhiteButtonPress()) { // Pause button func
      Serial.println("White button press");
      player.mp3.pausePlaying(!player.mp3.paused());
      delay(250);
    } else if (player.checkRedButtonPress()) {
      Serial.println("Red button press");

      player.mp3.stopPlaying();
      delay(1000);
      char path[128] = "";
      player.selectedItemPath(path);
      if (!player.mp3.startPlayingFile(path)) {
        Serial.print("Could not open ");
        Serial.println(path);
        while (1);
      }
      delay(250);
    } else if (player.checkGreenButtonPress()) { // Goes down the list of items
      Serial.println("Green button press");

      if (player.selectedItemIndex >= player.currentDirList.size() - 1) {
        player.selectedItemIndex = 0;
      } else {
        player.selectedItemIndex++;
      }

      Serial.println(player.selectedItemIndex);
      player.printCurrentDirectory();

      

      delay(250);
    } else if (player.checkBlueButtonPress()) { // Goes up the list of items
      Serial.println("Blue button press");

      if (player.selectedItemIndex <= 0) {
        player.selectedItemIndex = player.currentDirList.size() - 1;
      } else {
        player.selectedItemIndex--;
      }

      Serial.println(player.selectedItemIndex);
      player.printCurrentDirectory();


      delay(250);
    }
  }

  Serial.println("\nDone playing music");
}
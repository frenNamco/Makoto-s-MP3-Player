#include <MusicPlayer.h>

using namespace std;

SdFat SD;
MusicPlayer player;


void setup() {
  Serial.begin(115200);
  while (!Serial) delay(1);
  delay(1000);

  
  Serial.println("VS1053 + SdFat Test");

  if (!player.setup()) {
    while(1);
  }

  FsFile root = SD.open("/"); 
  player.addFoldertoJSON(root, "");
  root.close();

  player.saveJSON();
  player.testJSON();

  player.populateCurrentDirList();
  player.printCurrentDirectory();
}


void loop() {
  //TODO: Reformat this whole fucking thing I don't like it
  while(player.mp3.stopped() && !(player.mp3.paused())) {

    if (player.checkButtonPress(W_BUTTON_PIN)) { // Pause button func
      Serial.println("White button press");
      

    } else if (player.checkButtonPress(R_BUTTON_PIN)) {
      Serial.println("Red button press");
      char path[128] = "";
      player.selectedItemPath(path);
      if (!player.mp3.startPlayingFile(path)) {
        Serial.print("Could not open ");
        Serial.println(path);
        while (1);
      }

      
    } else if (player.checkButtonPress(G_BUTTON_PIN)) { // Goes down the list of items
      Serial.println("Green button press");

      if (player.selectedItemIndex >= player.currentDirList.size() - 1) {
        player.selectedItemIndex = 0;
      } else {
        player.selectedItemIndex++;
      }

      Serial.println(player.selectedItemIndex);
      player.printCurrentDirectory();

      
    } else if (player.checkButtonPress(B_BUTTON_PIN)) { // Goes up the list of items
      Serial.println("Blue button press");

      if (player.selectedItemIndex <= 0) {
        player.selectedItemIndex = player.currentDirList.size() - 1;
      } else {
        player.selectedItemIndex--;
      }

      Serial.println(player.selectedItemIndex);
      player.printCurrentDirectory();

      
    } else if (player.checkButtonPress(Y1_BUTTON_PIN)) {
      Serial.println("Yellow button 1 press");
      player.increaseVolume();

      
    } else if (player.checkButtonPress(Y2_BUTTON_PIN)) {
      Serial.println("Yellow button 2 press");
      player.decreaseVolume();

      
    } 

  }

  
  while (player.mp3.playingMusic || player.mp3.paused()) {
    
    if (player.checkButtonPress(W_BUTTON_PIN)) { // Pause button func
      Serial.println("White button press");
      player.mp3.pausePlaying(!player.mp3.paused());
      
    } else if (player.checkButtonPress(R_BUTTON_PIN)) {
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
      
    } else if (player.checkButtonPress(G_BUTTON_PIN)) { // Goes down the list of items
      Serial.println("Green button press");

      if (player.selectedItemIndex >= player.currentDirList.size() - 1) {
        player.selectedItemIndex = 0;
      } else {
        player.selectedItemIndex++;
      }

      Serial.println(player.selectedItemIndex);
      player.printCurrentDirectory();

      

      
    } else if (player.checkButtonPress(B_BUTTON_PIN)) { // Goes up the list of items
      Serial.println("Blue button press");

      if (player.selectedItemIndex <= 0) {
        player.selectedItemIndex = player.currentDirList.size() - 1;
      } else {
        player.selectedItemIndex--;
      }

      Serial.println(player.selectedItemIndex);
      player.printCurrentDirectory();


      
    } else if (player.checkButtonPress(Y1_BUTTON_PIN)) {
      Serial.println("Yellow button 1 press");
      player.increaseVolume();

      
    } else if (player.checkButtonPress(Y2_BUTTON_PIN)) {
      Serial.println("Yellow button 2 press");
      player.decreaseVolume();

      
    } 
  }

  Serial.println("\nDone playing music");
}
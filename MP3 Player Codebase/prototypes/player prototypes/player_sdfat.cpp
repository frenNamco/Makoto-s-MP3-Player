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
  //TODO: Still too many lines of code
  
  while(player.currentState == MusicPlayer::State::IDLE) {
    player.updateState();

    if (player.checkButtonPress(W_BUTTON_PIN)) { // Pause button func
      Serial.println("White button press");
      

    } else if (player.checkButtonPress(R_BUTTON_PIN)) {
      Serial.println("Red button press");
      player.playSong();
      
    } else if (player.checkButtonPress(G_BUTTON_PIN)) { // Goes down the list of items
      Serial.println("Green button press");
      player.goDownDirList();
      
    } else if (player.checkButtonPress(B_BUTTON_PIN)) { // Goes up the list of items
      Serial.println("Blue button press");
      player.goUpDirList();
      
    } else if (player.checkButtonPress(Y1_BUTTON_PIN)) {
      Serial.println("Yellow button 1 press");
      player.increaseVolume();
      
    } else if (player.checkButtonPress(Y2_BUTTON_PIN)) {
      Serial.println("Yellow button 2 press");
      player.decreaseVolume();

      
    } 

  }

  
  while (player.currentState == MusicPlayer::State::PLAYING) {
    player.updateState();
    if (player.checkButtonPress(W_BUTTON_PIN)) { // Pause button func
      Serial.println("White button press");
      player.playAndPause();
      
    } else if (player.checkButtonPress(R_BUTTON_PIN)) {
      Serial.println("Red button press");
      player.endSong();
      delay(1000);
      player.playSong();
      
    } else if (player.checkButtonPress(G_BUTTON_PIN)) { // Goes down the list of items
      Serial.println("Green button press");
      player.goDownDirList();

    } else if (player.checkButtonPress(B_BUTTON_PIN)) { // Goes up the list of items
      Serial.println("Blue button press");
      player.goUpDirList();

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
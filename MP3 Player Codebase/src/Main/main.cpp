#include <MusicPlayer.h>

using namespace std;

SdFat SD;
MusicPlayer player;

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(1); 
    delay(1000);

    Serial.println("MP3 Player Start");

    if(!player.setup()) {
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
    // For some reason async playing works best when constrained to a while loop
    // don't ask me why
    while (player.currentState == MusicPlayer::State::IDLE) player.runPlayerFunc();

    while(player.currentState == MusicPlayer::State::PLAYING) player.runPlayerFunc();

}
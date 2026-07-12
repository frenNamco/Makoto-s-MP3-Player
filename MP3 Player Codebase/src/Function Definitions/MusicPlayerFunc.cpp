#include <MusicPlayer.h>

bool MusicPlayer::setup() {
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

void MusicPlayer::updateStatus() {
    isWaitingForMusic = mp3.stopped() && !(mp3.paused());
    isPlayingMusic = mp3.playingMusic || mp3.paused();
}

void MusicPlayer::playAndPause() {
    mp3.pausePlaying(!mp3.paused());
}

//TODO: Fix this so it works with how you current save music
//HIGH Priority
//Requires a way to get the full path of a folder
void MusicPlayer::playSong() {
    char path[128] = "";
    getItemPath(path);

    if (!mp3.startPlayingFile(path)) {
        Serial.print("Could not open ");
        Serial.println(path);
        while (1);
    }
}

void MusicPlayer::endSong() {
    mp3.stopPlaying();
}

void MusicPlayer::goUpDirList() {
    if (selectedItemIndex <= 0) {
        selectedItemIndex = currentDirList.size() - 1;
    } else {
        selectedItemIndex--;
    }

    Serial.println(selectedItemIndex);
    printCurrentDirectory();
}

void MusicPlayer::goDownDirList() {
    if (selectedItemIndex >= currentDirList.size() - 1) {
        selectedItemIndex = 0;
    } else {
        selectedItemIndex++;
    }

    Serial.println(selectedItemIndex);
    printCurrentDirectory();
}

//TODO: Change the way that the volume is set
//Map a percent (0 - 100) to a register value (255 to 0)
//LOW Priority
void MusicPlayer::increaseVolume() {
    currentVolume -= volumeChange;
    
    Serial.printf("Current volume: %d", currentVolume);
    Serial.println("");
    mp3.setVolume(currentVolume, currentVolume);
}

void MusicPlayer::decreaseVolume() {
    currentVolume += volumeChange;
    
    Serial.printf("Current volume: %d", currentVolume);
    Serial.println("");
    mp3.setVolume(currentVolume, currentVolume);
}

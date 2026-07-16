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

void MusicPlayer::updateState() {
    if (mp3.stopped() && !(mp3.paused())) {
        currentState = MusicPlayer::State::IDLE;
    } else if (mp3.playingMusic || mp3.paused()) {
        currentState = MusicPlayer::State::PLAYING;
    }
}

void MusicPlayer::changeState(State newState) {
    currentState = newState;
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

void MusicPlayer::runPlayerFunc() {
    updateState();

    if (checkButtonPress(W_BUTTON_PIN)) {
        Serial.println("White button press");
        if (currentState == State::PLAYING) playAndPause();

    } else if (checkButtonPress(R_BUTTON_PIN)) {
        Serial.println("Red button press");
        if (currentState == State::PLAYING) {
            endSong();
            delay(1000);

        }

        playSong();

    } else if (checkButtonPress(G_BUTTON_PIN)) {
        Serial.println("Green button press");
        goDownDirList();
    } else if (checkButtonPress(B_BUTTON_PIN)) {
        Serial.println("Blue button press");
        goUpDirList();
    } else if (checkButtonPress(Y1_BUTTON_PIN)) {
        Serial.println("Yellow button 1 press");
        increaseVolume();
    } else if (checkButtonPress(Y2_BUTTON_PIN)) {
        Serial.println("Yellow button 2 press");
        decreaseVolume();
    }
}
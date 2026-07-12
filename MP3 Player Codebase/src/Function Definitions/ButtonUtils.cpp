//TODO: Seperate the class function definitions for the button utils over here

#include <MusicPlayer.h>

bool MusicPlayer::checkButtonPress(int buttonPin) {
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


#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   32
#define SCREEN_RESET    -1
#define SCREEN_ADDRESS  0x3C

#define FWD_BUTTON  14
#define BCK_BUTTON  15

Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,-1);

unsigned long startTime, currentTime;
const unsigned long scrollTime = 14;

int textScroll(String text, int cursorPosition, unsigned long currentTime, unsigned long scrollTime) {
    int textLength = map(text.length(),0,21,0,SCREEN_WIDTH); 


    if (textLength >= SCREEN_WIDTH && (currentTime - startTime >= scrollTime)) {
        cursorPosition--;
        
        String emptyText = " ";
        int emptyTextLength = emptyText.length();
        int textEnd = cursorPosition + textLength + emptyTextLength;
        if ((textEnd <= SCREEN_WIDTH) && (textEnd >= 0)) {
            display.setCursor(textEnd, 0);
            display.print(text);
        } else if (textEnd <= 0) {
            cursorPosition = textEnd;
        }

    
        startTime = currentTime;
    } else if (textLength <= SCREEN_WIDTH) {
        cursorPosition = 0;
    }

    return cursorPosition;
}

void setup() {
    Serial.begin(115200);

    startTime = millis();

    pinMode(FWD_BUTTON, INPUT_PULLUP);
    pinMode(BCK_BUTTON, INPUT_PULLUP);

    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.setTextWrap(false);
    // Display static text
    display.println("Hello, world!!!");
    display.display(); 
}

const String message[5]= {"some text", 
    "hello world but made like a gajillion times long", 
    "some more text", 
    "fuck that stupid fat ass motherfucking stupid squirrel", 
    "balls"

};
int cursorPosition = 0;
int messageIndex = 0;
String longString = "small";

void loop() {
    display.clearDisplay();
    currentTime = millis();

    int fwdButtonPressed = !(digitalRead(FWD_BUTTON));
    int bckButtonPressed = !(digitalRead(BCK_BUTTON));

    cursorPosition = textScroll(message[messageIndex], cursorPosition, currentTime, scrollTime);

    if (fwdButtonPressed) {
        messageIndex++;

        if (messageIndex > 4) {
            messageIndex = 0;
        }
        
        delay(500);
    }
    
    if (bckButtonPressed) {
        messageIndex--;

        if (messageIndex < 0) {
            messageIndex = 4;
        }

        delay(500);
    }

    display.setCursor(cursorPosition,0);
    display.print(message[messageIndex]);


    display.setCursor(0,10);
    display.setCursor(0,20);
    display.print("Cursor Position: ");
    display.println(cursorPosition);

    display.display();
}
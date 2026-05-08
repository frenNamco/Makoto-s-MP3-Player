#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
// #include <SdFat.h>

#include <bubbles.h>

#define SDA 4
#define SCL 5

#define SD_CS   17
#define SD_SCK  18
#define SD_MOSI 19
#define SD_MISO 16

#define FWD_BUTTON  14
#define BCK_BUTTON  15
#define RGT_BUTTON  13
#define LFT_BUTTON  12

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);

// SdFat sd;


void printPixelLocation(int pxlX, int pxlY) {
    Serial.print("(");
    Serial.print(pxlX);
    Serial.print(", ");
    Serial.print(pxlY);
    Serial.println(")");
}

void setup() {
    if (u8g2.begin()) Serial.print("Shit didn't work");
    delay(500);
    Serial.println("nvm it did work");

    pinMode(FWD_BUTTON, INPUT_PULLUP);
    pinMode(BCK_BUTTON, INPUT_PULLUP);
    pinMode(RGT_BUTTON, INPUT_PULLUP);
    pinMode(LFT_BUTTON, INPUT_PULLUP);
    
    u8g2.drawXBMP(0, 0, BUBBLES_WIDTH, BUBBLES_HEIGHT, epd_bitmap_bubbles);
    u8g2.sendBuffer();
    delay(500);
    u8g2.clearBuffer();
}

int pxlX, pxlY;

void loop() {
    int fwdButtonPressed = !(digitalRead(FWD_BUTTON));
    int bckButtonPressed = !(digitalRead(BCK_BUTTON));
    int rgtButtonPressed = !(digitalRead(RGT_BUTTON));
    int lftButtonPressed = !(digitalRead(LFT_BUTTON));

    if (bckButtonPressed) {
        pxlY++;

        if (pxlY > u8g2.getDisplayHeight()) {
            pxlY = 0;
        }
    
        printPixelLocation(pxlX, pxlY);
    }
    
    if (fwdButtonPressed) {
        pxlY--;

        if (pxlY < 0) {
            pxlY = u8g2.getDisplayHeight();
        }

        printPixelLocation(pxlX, pxlY);
    }

    if (rgtButtonPressed) {
        pxlX++;

        if (pxlX > u8g2.getDisplayWidth()) {
            pxlX= 0;
        }

        printPixelLocation(pxlX, pxlY);
    }

    if (lftButtonPressed) {
        pxlX--;

        if (pxlX < 0) {
            pxlX = u8g2.getDisplayWidth();
        }

        printPixelLocation(pxlX, pxlY);
    }

    u8g2.clearBuffer();

    u8g2.drawPixel(pxlX,pxlY);
    u8g2.sendBuffer();   
}
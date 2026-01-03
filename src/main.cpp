#include <Arduino.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <GxEPD2_BW.h> // библиотека для черно-белых дисплеев

#include "image/img.h"

#define LED_PIN 2
#define BUTTON_PIN 19 // любой свободный GPIO

bool eyesState = false;
bool isEyesBaseDrawn = false;

bool ledState = false;
bool lastButtonState = HIGH;

// Выбираем вашу модель e-Paper (2.13", BW, B72) — подойдет Waveshare 2.13" HAT
GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT>
display(GxEPD2_213_B74(/*CS=*/22, /*DC=*/21, /*RST=*/4, /*BUSY=*/17));

void blink() {
    digitalWrite(LED_PIN, HIGH); // включить
    delay(5000);
    digitalWrite(LED_PIN, LOW); // выключить
    delay(5000);
}

void draw(short scale, short xOffset, short yOffset, const short y[], int ySize, const short x[], uint16_t color) {
    int xIndex = 0;
    for (int i = 0; i < ySize; i++) {
        for (int j = 0; j < y[i * 2 + 1]; j++) {
            display.fillRect(xOffset + x[xIndex] * scale, yOffset + y[i * 2] * scale, scale, scale, color);
            xIndex++;
        }
    }
}

void drawDynamicCuteFace() {
    short scale = 8;
    short xOffset = 0;
    short yOffset = 0;
    // TODO move to header
    draw(scale, xOffset, yOffset, eyesLeftY, 2, eyesLeftX,
         eyesState ? GxEPD_BLACK : GxEPD_WHITE);
    draw(scale, xOffset, yOffset, eyesRightY, 2, eyesRightX,
         eyesState ? GxEPD_WHITE : GxEPD_BLACK);
    eyesState = !eyesState;
    if (!isEyesBaseDrawn) {
        draw(scale, xOffset, yOffset, eyesBaseY, 8, eyesBaseX, GxEPD_BLACK);
        isEyesBaseDrawn = true;
    }
    display.display(true);
    display.hibernate();
}

void handleButton() {
    bool buttonState = !digitalRead(BUTTON_PIN);

    // ловим момент нажатия
    if (buttonState == HIGH && lastButtonState == LOW) {
        ledState = !ledState; // переключаем состояние
        digitalWrite(LED_PIN, ledState);
        drawDynamicCuteFace();
        delay(100); // антидребезг (простой)
    }

    lastButtonState = buttonState;
}

void setup() {
    Serial.begin(115200);
    Serial.println("e-Paper demo started");

    // Диод
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Кнопка
    pinMode(BUTTON_PIN, INPUT);

    // Дисплей
    display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
    display.setRotation(1);
    display.fillScreen(GxEPD_WHITE);
    display.display(false);
}

void loop() {
    // blink();
    handleButton();
    // delay(5000);
}

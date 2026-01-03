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

int heartsMode = 0;

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

void drawHearts1() {
    short scale = 4;
    short xOffset = 0;
    short yOffset = 0;
    short heart1Y[] = {
        8, 18,
        9, 30,
        10, 33,
        11, 33,
        12, 27,
        13, 21,
        14, 9,
        17, 12,
        18, 18,
        19, 16,
        20, 15,
        21, 13,
        22, 13,
        23, 6,
    };
    short heart1X[] = {
        13, 14, 15, 19, 20, 21, 26, 27, 28, 32, 33, 34, 39, 40, 41, 45, 46, 47,
        12, 13, 14, 15, 16, 18, 19, 20, 21, 22, 25, 26, 27, 28, 29, 31, 32, 33, 34, 35, 38, 39, 40, 41, 42, 44, 45, 46,
        47, 48,
        12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 38, 39, 40, 41, 42, 43,
        44, 45, 46, 47, 48,
        12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 38, 39, 40, 41, 42, 43,
        44, 45, 46, 47, 48,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 26, 27, 28, 29, 30, 31, 32, 33, 34, 39, 40, 41, 42, 43, 44, 45, 46, 47,
        14, 15, 16, 17, 18, 19, 20, 27, 28, 29, 30, 31, 32, 33, 40, 41, 42, 43, 44, 45, 46,
        16, 17, 18, 29, 30, 31, 42, 43, 44,
        19, 20, 21, 25, 26, 27, 33, 34, 35, 39, 40, 41,
        18, 19, 20, 21, 22, 24, 25, 26, 27, 28, 32, 33, 35, 36, 38, 39, 41, 42,
        18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 32, 36, 37, 38, 42,
        18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 32, 33, 41, 42,
        19, 20, 21, 22, 23, 24, 25, 26, 27, 33, 34, 40, 41,
        20, 21, 22, 23, 24, 25, 26, 34, 35, 36, 38, 39, 40,
        22, 23, 24, 36, 37, 38,
    };
    short heart1Size = 14;
    draw(scale, xOffset, yOffset, heart1Y, heart1Size, heart1X, GxEPD_BLACK);
    display.display(true);
}

void drawHearts2() {
    short scale = 4;
    short xOffset = 0;
    short yOffset = 0;
    short heart2Y[] = {
        6, 18,
        7, 30,
        8, 33,
        9, 33,
        10, 27,
        11, 21,
        12, 9,
        17, 18,
        18, 28,
        19, 27,
        20, 26,
        21, 22,
        22, 20,
        23, 9,
    };
    short heart2X[] = {
        11, 12, 13, 17, 18, 19, 26, 27, 28, 32, 33, 34, 41, 42, 43, 47, 48, 49,
        10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 25, 26, 27, 28, 29, 31, 32, 33, 34, 35, 40, 41, 42, 43, 44, 46, 47, 48,
        49, 50,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 40, 41, 42, 43, 44, 45,
        46, 47, 48, 49, 50,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 40, 41, 42, 43, 44, 45,
        46, 47, 48, 49, 50,
        11, 12, 13, 14, 15, 16, 17, 18, 19, 26, 27, 28, 29, 30, 31, 32, 33, 34, 41, 42, 43, 44, 45, 46, 47, 48, 49,
        12, 13, 14, 15, 16, 17, 18, 27, 28, 29, 30, 31, 32, 33, 42, 43, 44, 45, 46, 47, 48,
        14, 15, 16, 29, 30, 31, 44, 45, 46,
        11, 12, 13, 17, 18, 19, 26, 27, 28, 32, 33, 34, 41, 42, 43, 47, 48, 49,
        10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 25, 26, 27, 28, 29, 31, 32, 33, 34, 35, 40, 41, 43, 44, 46, 47, 49, 50,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 40, 44, 45, 46, 50,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 40, 41, 49, 50,
        11, 12, 13, 14, 15, 16, 17, 18, 19, 26, 27, 28, 29, 30, 31, 32, 33, 34, 41, 42, 48, 49,
        12, 13, 14, 15, 16, 17, 18, 27, 28, 29, 30, 31, 32, 33, 42, 43, 44, 46, 47, 48,
        14, 15, 16, 29, 30, 31, 44, 45, 46,
    };
    short heart2Size = 14;
    draw(scale, xOffset, yOffset, heart2Y, heart2Size, heart2X, GxEPD_BLACK);
    display.display(true);
}

void drawHearts3() {
    short scale = 4;
    short xOffset = 0;
    short yOffset = 0;
    short heart3Y[] = {
        6, 24,
        7, 40,
        8, 44,
        9, 44,
        10, 36,
        11, 28,
        12, 12,
        17, 24,
        18, 38,
        19, 38,
        20, 37,
        21, 31,
        22, 27,
        23, 12,
    };
    short heart3X[] = {
        3, 4, 5, 9, 10, 11, 18, 19, 20, 24, 25, 26, 33, 34, 35, 39, 40, 41, 48, 49, 50, 54, 55, 56,
        2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 17, 18, 19, 20, 21, 23, 24, 25, 26, 27, 32, 33, 34, 35, 36, 38, 39, 40, 41, 42,
        47, 48, 49, 50, 51, 53, 54, 55, 56, 57,
        2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
        2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
        3, 4, 5, 6, 7, 8, 9, 10, 11, 18, 19, 20, 21, 22, 23, 24, 25, 26, 33, 34, 35, 36, 37, 38, 39, 40, 41, 48, 49, 50,
        51, 52, 53, 54, 55, 56,
        4, 5, 6, 7, 8, 9, 10, 19, 20, 21, 22, 23, 24, 25, 34, 35, 36, 37, 38, 39, 40, 49, 50, 51, 52, 53, 54, 55,
        6, 7, 8, 21, 22, 23, 36, 37, 38, 51, 52, 53,
        3, 4, 5, 9, 10, 11, 18, 19, 20, 24, 25, 26, 33, 34, 35, 39, 40, 41, 48, 49, 50, 54, 55, 56,
        2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 17, 18, 19, 20, 21, 23, 24, 25, 26, 27, 32, 33, 34, 35, 36, 38, 39, 40, 41, 42,
        47, 48, 50, 51, 53, 54, 56, 57,
        2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 47, 51, 52, 53, 57,
        2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 47, 48, 56, 57,
        3, 4, 5, 6, 7, 8, 9, 10, 11, 18, 19, 20, 21, 22, 23, 24, 25, 26, 33, 34, 35, 36, 37, 38, 39, 40, 41, 48, 49, 55,
        56,
        4, 5, 6, 7, 8, 9, 10, 19, 20, 21, 22, 23, 24, 25, 34, 35, 36, 37, 38, 39, 40, 49, 50, 51, 53, 54, 55,
        6, 7, 8, 21, 22, 23, 36, 37, 38, 51, 52, 53,
    };
    short heart3Size = 14;
    draw(scale, xOffset, yOffset, heart3Y, heart3Size, heart3X, GxEPD_BLACK);
    display.display(true);
}

void drawHearts() {
    display.fillScreen(GxEPD_WHITE);
    switch (heartsMode) {
        case 0:
            drawHearts2();
            heartsMode = 1;
            break;
        case 1:
            drawHearts3();
            heartsMode = 2;
            break;
        case 2:
            drawHearts1();
            heartsMode = 0;
            break;
    }
}

void handleButton() {
    bool buttonState = !digitalRead(BUTTON_PIN);

    // ловим момент нажатия
    if (buttonState == HIGH && lastButtonState == LOW) {
        ledState = !ledState; // переключаем состояние
        digitalWrite(LED_PIN, ledState);
        // drawDynamicCuteFace();
        drawHearts();
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

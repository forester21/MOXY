#include <Arduino.h>
#include <GxEPD2_BW.h> // библиотека для черно-белых дисплеев
#include <HTTPClient.h>

#include "image/img.h"
#include "image/img_time.h"
#include "image/img_temp.h"

#include "wifi/wifi.h"
#include "temperature/Temperature.h"

#define LED_PIN 2
#define BUTTON_PIN 19 // любой свободный GPIO

// Выбираем вашу модель e-Paper (2.13", BW, B72) — подойдет Waveshare 2.13" HAT
GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT> display(GxEPD2_213_B74(/*CS=*/22, /*DC=*/21, /*RST=*/4, /*BUSY=*/17));

// Кнопка
bool lastButtonState = HIGH;

// Обновление времени
int lastMinute = 0;
unsigned long lastTimeCheck = 0;
const unsigned long TIME_CHECK_INTERVAL = 1000; // проверяем время раз в секунду

// Обновление экрана
unsigned long lastScreenUpdate = 0;
const unsigned long SCREEN_UPDATE_INTERVAL = 60UL * 1000UL; // обновляем экран раз в 1 минуту
int displayMode = 0;
constexpr int DISPLAY_MODES_COUNT = 3;

// Обновление температуры
unsigned long lastTempUpdate = 0;
const unsigned long TEMP_UPDATE_INTERVAL = 15UL * 60UL * 1000UL; // обновляем температуру каждые 15 минут

void draw(short scale, short xOffset, short yOffset, const short y[], int ySize, const short x[], uint16_t color) {
    int xIndex = 0;
    for (int i = 0; i < ySize; i++) {
        for (int j = 0; j < y[i * 2 + 1]; j++) {
            display.fillRect(xOffset + x[xIndex] * scale, yOffset + y[i * 2] * scale, scale, scale, color);
            xIndex++;
        }
    }
}

bool eyesState = false;
bool isEyesBaseDrawn = false;

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

void drawHearts(int heartsState) {
    short scale = 5;
    short xOffset = -25;
    short yOffset = -15;
    display.fillScreen(GxEPD_WHITE);
    draw(scale, xOffset, yOffset, heartsBaseY, 14, heartsBaseX, GxEPD_BLACK);
    for (int i = 0; i <= heartsState; i++) {
        if (i % 2 == 0) {
            draw(scale, xOffset + i % 6 / 2 * scale * 15, yOffset + i / 6 * scale * 11, heartFillingHalfY,
                 heartFillingHalfSize, heartFillingHalfX, GxEPD_BLACK);
        } else {
            // TODO заменить full на правую половину
            draw(scale, xOffset + i % 6 / 2 * scale * 15, yOffset + i / 6 * scale * 11, heartFillingFullY,
                 heartFillingFullSize, heartFillingFullX, GxEPD_BLACK);
        }
    }
    display.display(true);
}

void drawSmallNumber(short scale, short xOffset, short yOffset, int number) {
    switch (number) {
        case 0:
            draw(scale, xOffset, yOffset, time0Y, time0Size, time0X, GxEPD_BLACK);
            break;
        case 1:
            draw(scale, xOffset, yOffset, time1Y, time1Size, time1X, GxEPD_BLACK);
            break;
        case 2:
            draw(scale, xOffset, yOffset, time2Y, time2Size, time2X, GxEPD_BLACK);
            break;
        case 3:
            draw(scale, xOffset, yOffset, time3Y, time3Size, time3X, GxEPD_BLACK);
            break;
        case 4:
            draw(scale, xOffset, yOffset, time4Y, time4Size, time4X, GxEPD_BLACK);
            break;
        case 5:
            draw(scale, xOffset, yOffset, time5Y, time5Size, time5X, GxEPD_BLACK);
            break;
        case 6:
            draw(scale, xOffset, yOffset, time6Y, time6Size, time6X, GxEPD_BLACK);
            break;
        case 7:
            draw(scale, xOffset, yOffset, time7Y, time7Size, time7X, GxEPD_BLACK);
            break;
        case 8:
            draw(scale, xOffset, yOffset, time8Y, time8Size, time8X, GxEPD_BLACK);
            break;
        case 9:
            draw(scale, xOffset, yOffset, time9Y, time9Size, time9X, GxEPD_BLACK);
            break;
    }
}

void drawBigNumber(short scale, short xOffset, short yOffset, int number) {
    switch (number) {
        case 0:
            draw(scale, xOffset, yOffset, temp0Y, temp0Size, temp0X, GxEPD_BLACK);
            break;
        case 1:
            draw(scale, xOffset, yOffset, temp1Y, temp1Size, temp1X, GxEPD_BLACK);
            break;
        case 2:
            draw(scale, xOffset, yOffset, temp2Y, temp2Size, temp2X, GxEPD_BLACK);
            break;
        case 3:
            draw(scale, xOffset, yOffset, temp3Y, temp3Size, temp3X, GxEPD_BLACK);
            break;
        case 4:
            draw(scale, xOffset, yOffset, temp4Y, temp4Size, temp4X, GxEPD_BLACK);
            break;
        case 5:
            draw(scale, xOffset, yOffset, temp5Y, temp5Size, temp5X, GxEPD_BLACK);
            break;
        case 6:
            draw(scale, xOffset, yOffset, temp6Y, temp6Size, temp6X, GxEPD_BLACK);
            break;
        case 7:
            draw(scale, xOffset, yOffset, temp7Y, temp7Size, temp7X, GxEPD_BLACK);
            break;
        case 8:
            draw(scale, xOffset, yOffset, temp8Y, temp8Size, temp8X, GxEPD_BLACK);
            break;
        case 9:
            draw(scale, xOffset, yOffset, temp9Y, temp9Size, temp9X, GxEPD_BLACK);
            break;
    }
}

const char *weatherUrl =
        "https://api.open-meteo.com/v1/forecast?latitude=55.998227&longitude=37.210115&current=temperature_2m";

Temperature temperature(weatherUrl, LED_PIN);

void drawTemp() {
    short scale = 8;
    short xOffset = 0;
    short yOffset = 0;
    display.fillScreen(GxEPD_WHITE);
    int firstNumber = abs(temperature.get()) / 10;
    int secondNumber = abs(temperature.get()) % 10;
    draw(scale, xOffset, yOffset, tempDegreeY, tempDegreeSize, tempDegreeX, GxEPD_BLACK);
    int additionalOffset = 0;
    drawBigNumber(scale, xOffset + 13 * scale, yOffset, secondNumber);
    if (firstNumber > 0) {
        drawSmallNumber(scale, xOffset + additionalOffset + 7 * scale, yOffset, firstNumber);
    } else {
        additionalOffset = additionalOffset + 6 * scale;
    }
    if (temperature.get() < 0) {
        draw(scale, xOffset + additionalOffset, yOffset, tempMinusY, tempMinusSize, tempMinusX, GxEPD_BLACK);
    } else if (temperature.get() > 0) {
        draw(scale, xOffset + additionalOffset, yOffset, tempPlusY, tempPlusSize, tempPlusX, GxEPD_BLACK);
    }

    display.display(true);
}

void drawTime() {
    short scale = 8;
    short xOffset = 5;
    short yOffset = 0;
    struct tm timeinfo;
    //TODO sync daily
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
    }
    lastMinute = timeinfo.tm_min;
    display.fillScreen(GxEPD_WHITE);
    drawSmallNumber(scale, xOffset, yOffset, timeinfo.tm_hour / 10);
    drawSmallNumber(scale, xOffset + 6 * scale, yOffset, timeinfo.tm_hour % 10);
    draw(scale, xOffset, yOffset, delimiterY, delimiterSize, delimiterX, GxEPD_BLACK);
    drawSmallNumber(scale, xOffset + 15 * scale, yOffset, timeinfo.tm_min / 10);
    drawSmallNumber(scale, xOffset + 21 * scale, yOffset, timeinfo.tm_min % 10);
    display.display(true);
}

void drawByState() {
    switch (displayMode) {
        case 0:
            drawTemp();
            break;
        case 1:
            drawTime();
            break;
        case 2:
            drawHearts(rand() % 12);
            break;
    }
}

void drawNextScreen() {
    displayMode = (displayMode + 1) % DISPLAY_MODES_COUNT;
    drawByState();
}

void handleButton() {
    bool buttonState = !digitalRead(BUTTON_PIN);

    // ловим момент нажатия
    if (buttonState == HIGH && lastButtonState == LOW) {
        drawNextScreen();
        lastScreenUpdate = millis();
        // drawDynamicCuteFace();
        // delay(100); // антидребезг (простой)
        // display.hibernate();
    }

    lastButtonState = buttonState;
}

void initDisplay() {
    display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
    display.setRotation(1);
    display.fillScreen(GxEPD_WHITE);
    display.display(false);
}

void setupTime() {
    // GMT +3 = 3 * 3600
    const long gmtOffset_sec = 3 * 3600;
    const int daylightOffset_sec = 0;
    configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");
    Serial.println("Waiting for time sync...");
    delay(2000);
}


void setup() {
    Serial.begin(115200);

    // Диод
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Кнопка
    pinMode(BUTTON_PIN, INPUT);

    // Дисплей
    initDisplay();

    // BLE
    // setupBle();

    //WIFI
    setupWiFi();

    // Настройка времени
    setupTime();

    temperature.fetch();
}

void loop() {
    unsigned long now = millis();
    handleButton();

    if (now - lastScreenUpdate >= SCREEN_UPDATE_INTERVAL) {
        lastScreenUpdate = now;
        drawNextScreen();
    }

    // temperature update
    if (now - lastTempUpdate >= TEMP_UPDATE_INTERVAL) {
        lastTempUpdate = now;
        temperature.fetch();
    }

    // clock
    if (displayMode == 1) {
        // Проверка времени по таймеру
        if (now - lastTimeCheck >= TIME_CHECK_INTERVAL) {
            lastTimeCheck = now;
            struct tm timeinfo;
            getLocalTime(&timeinfo);
            if (lastMinute != timeinfo.tm_min) {
                drawTime();
            }
        }
    }
}

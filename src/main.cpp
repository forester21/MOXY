#include <Arduino.h>
#include <GxEPD2_BW.h> // библиотека для черно-белых дисплеев
#include <HTTPClient.h>

#include "image/img.h"
#include "image/img_time.h"
#include "image/img_temp.h"

#include "wifi/wifi.h"
#include "temperature/Temperature.h"

#include <S8_UART.h>
#include <HardwareSerial.h>

#include <Wire.h>
#include <SensirionI2cSht4x.h>

//////////////////////////////////////////////////////////////////

// esp32
// #define LED_PIN 2
// #define BUTTON_PIN 19
// #define S8_RX_PIN 32
// #define S8_TX_PIN 33
// #define DISPLAY_CS_PIN 22
// #define DISPLAY_DC_PIN 21
// #define DISPLAY_RST_PIN 4
// #define DISPLAY_BUSY_PIN 17
// HardwareSerial S8Serial(2);

// esp32-c3
#define LED_PIN 8
#define BUTTON_PIN 21
#define S8_RX_PIN 10
#define S8_TX_PIN 20
#define DISPLAY_CS_PIN 3
#define DISPLAY_DC_PIN 2
#define DISPLAY_RST_PIN 1
#define DISPLAY_BUSY_PIN 0
#define SDA_PIN 6
#define SCL_PIN 4
HardwareSerial S8Serial(1);

//////////////////////////////////////////////////////////////////

// Выбираем вашу модель e-Paper (2.13", BW, B72) — подойдет Waveshare 2.13" HAT
// GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT> display(GxEPD2_213_B74(DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, DISPLAY_BUSY_PIN));

GxEPD2_BW<GxEPD2_290_BS, GxEPD2_290_BS::HEIGHT> display(GxEPD2_290_BS(DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, DISPLAY_BUSY_PIN));

SensirionI2cSht4x sht4x;

S8_UART s8(S8Serial);
S8_sensor sensor;

// Кнопка
bool lastButtonState = HIGH;

// Обновление времени
int lastMinute = 0;
unsigned long lastTimeCheck = 0;
const unsigned long TIME_CHECK_INTERVAL = 1000; // проверяем время раз в секунду

// Обновление экрана
int lastHeartsState = 0;
unsigned long lastDisplayUpdate = 0;
const unsigned long DISPLAY_UPDATE_INTERVAL = 60UL * 1000UL;
int displayMode = 0;
int screenRefreshCounter = 0;
const int FULL_REFRESH_AFTER = 100; // полное обновление экрана после N частичных обновлений

// Обновление температуры на улице
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

void displayFullRefreshIfRequired() {
    if (screenRefreshCounter > FULL_REFRESH_AFTER) {
        display.fillScreen(GxEPD_WHITE);
        display.display(false);
        screenRefreshCounter = 0;
    }
}

void displayRefresh() {
    display.display(true);
    screenRefreshCounter++;
}

bool eyesState = false;
bool isEyesBaseDrawn = false;

void drawDynamicCuteFace() {
    displayFullRefreshIfRequired();
    // short scale = 8;
    // short yOffset = 0;
    // short xOffset = 0;

    short scale = 9;
    short xOffset = 10;
    short yOffset = -10;

    draw(scale, xOffset, yOffset, eyesLeftY, 2, eyesLeftX,
         eyesState ? GxEPD_BLACK : GxEPD_WHITE);
    draw(scale, xOffset, yOffset, eyesRightY, 2, eyesRightX,
         eyesState ? GxEPD_WHITE : GxEPD_BLACK);
    eyesState = !eyesState;
    if (!isEyesBaseDrawn) {
        draw(scale, xOffset, yOffset, eyesBaseY, 8, eyesBaseX, GxEPD_BLACK);
        isEyesBaseDrawn = true;
    }
    displayRefresh();
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

void drawHearts(bool forceRedraw = true) {
    int co2ppm = s8.get_co2();

    // Serial.print("CO2: ");
    // Serial.print(co2ppm);
    // Serial.println(" ppm");

    int highestLevel = 1600;
    int lowLevel = 400;

    // 0 to 11
    int heartsState = 0;
    if (co2ppm > highestLevel) {
        heartsState = 0;
    } else if (co2ppm > lowLevel) {
        heartsState = 10 - (co2ppm - lowLevel) / ((highestLevel - lowLevel) / 12);
    } else {
        heartsState = 11;
    }
    if (!forceRedraw && heartsState == lastHeartsState) {
        return;
    }
    lastHeartsState = heartsState;

    // mini display
    // short scale = 5;
    // short xOffset = -25;
    // short yOffset = -15;

    // maxi big
    short scale = 6;
    short xOffset = -35;
    short yOffset = -25;

    display.fillScreen(GxEPD_WHITE);
    draw(scale, xOffset, yOffset, heartsBaseY, 14, heartsBaseX, GxEPD_BLACK);
    if (heartsState >= 0) {
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
    }
    displayRefresh();
}

void drawPPM() {
    int co2ppm = s8.get_co2();

    Serial.println("Co2: " + String(co2ppm) + " ppm");

    // short numberScale = 8;
    short numberScale = 9;
    int numberOffset = 0;
    display.fillScreen(GxEPD_WHITE);
    if (co2ppm >= 1000) {
        drawSmallNumber(numberScale, 0, 0, co2ppm / 1000);
        numberOffset = 6 * numberScale;
    }
    drawSmallNumber(numberScale, numberOffset, 0, (co2ppm / 100) % 10);
    drawSmallNumber(numberScale, numberOffset + 6 * numberScale, 0, (co2ppm / 10) % 10);
    drawSmallNumber(numberScale, numberOffset + 12 * numberScale, 0, co2ppm % 10);
    draw(3, numberOffset + 19 * numberScale, 3, ppmY, ppmSize, ppmX, GxEPD_BLACK);
    displayRefresh();
}

const char *weatherUrl =
        "https://api.open-meteo.com/v1/forecast?latitude=55.998227&longitude=37.210115&current=temperature_2m";

Temperature temperature(weatherUrl, LED_PIN);

void drawTemp() {
    // short scale = 8;
    // short xOffset = 0;

    short scale = 9;
    short xOffset = 20;

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

    displayRefresh();
}

void drawTime() {
    // short scale = 8;
    // short xOffset = 5;

    short scale = 9;
    short xOffset = 15;

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
    displayRefresh();
}

void getIndoorTemp() {
    float temp;
    float humidity;

    int16_t error = sht4x.measureHighPrecision(temp, humidity);

    if (error) {
        Serial.print("SHT40 error: ");
        Serial.println(error);
    } else {
        Serial.print("T=");
        Serial.print(temp);
        Serial.print(" °C  H=");
        Serial.print(humidity);
        Serial.println(" %");
    }

}

constexpr int DISPLAY_MODES_COUNT = 4;
void drawByState(bool forceRedraw = true) {
    displayFullRefreshIfRequired();
    switch (displayMode) {
        case 0:
            drawTemp();
            break;
        case 1:
            drawTime();
            break;
        case 2:
            drawHearts(forceRedraw);
            break;
        case 3:
            drawPPM();
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

void setupSenseAir() {
    // Инициализация UART для S8
    S8Serial.begin(9600, SERIAL_8N1, S8_TX_PIN, S8_RX_PIN);

    Serial.println("SenseAir S8 init...");
}

void setupSHT4x() {
    Wire.begin(SDA_PIN, SCL_PIN);
    //TODO мб удалить
    Wire.setClock(50000);

    sht4x.begin(Wire, 0x44);

    Serial.println("SHT4x init done");
}

void setup() {
    Serial.begin(115200);
    delay(5000);
    Serial.println("Started serial");

    // Дисплей
    initDisplay();
    drawDynamicCuteFace();

    // Датчик температуры и влажности
    setupSHT4x();

    // Датчик CO2
    delay(1000);
    setupSenseAir();
    drawDynamicCuteFace();

    // Диод
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Кнопка
    pinMode(BUTTON_PIN, INPUT);

    // BLE
    // setupBle();

    //WIFI
    setupWiFi();
    drawDynamicCuteFace();

    // Настройка времени
    setupTime();
    drawDynamicCuteFace();

    unsigned long now = millis();
    temperature.fetch();
    lastTempUpdate = now;

    drawByState();
}

boolean calibrationStarted = false;
boolean calibrationEnabled = false;

// https://forum.airgradient.com/t/manually-calibrating-the-s8-co2-sensor/33/4
void calibrateS8() {
    if (!calibrationEnabled) {
        return;
    }
    Serial.println("Starting manual calibration...");

    if (calibrationStarted) {
        sensor.ack = s8.get_acknowledgement();
        if (sensor.ack & S8_MASK_CO2_BACKGROUND_CALIBRATION) {
            Serial.println("Manual calibration finished!");
            calibrationEnabled = false;
            calibrationStarted = false;
        } else {
            Serial.println("Calibration in progress...");
        }
    } else if (s8.manual_calibration()) {
        calibrationStarted = true;
        Serial.println("Manual calibration started successfully!");
    } else {
        Serial.println("Error setting manual calibration!");
    }
}

void loop() {
    unsigned long now = millis();
    handleButton();

    if (now - lastTempUpdate >= TEMP_UPDATE_INTERVAL) {
        lastTempUpdate = now;
        temperature.fetch();
        if (displayMode == 0) {
            drawByState();
        }
    }

    if (displayMode == 1) {
        // Проверка времени по таймеру
        if (now - lastTimeCheck >= TIME_CHECK_INTERVAL) {
            lastTimeCheck = now;
            struct tm timeinfo;
            getLocalTime(&timeinfo);
            if (lastMinute != timeinfo.tm_min) {
                drawByState();
            }
        }
    }

    if (displayMode == 2 || displayMode == 3) {
        // Проверка времени по таймеру
        if (now - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
            lastDisplayUpdate = now;
            drawByState(false);
        }
    }
}
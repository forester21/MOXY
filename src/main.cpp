#include <Arduino.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <GxEPD2_BW.h> // библиотека для черно-белых дисплеев

#include "image/img.h"

// #include <BLEDevice.h>
// #include <BLEServer.h>
// #include <BLEUtils.h>

#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcdefab-1234-5678-1234-abcdefabcdef"

// BLECharacteristic *pCharacteristic;

#define LED_PIN 2
#define BUTTON_PIN 19 // любой свободный GPIO

bool eyesState = false;
bool isEyesBaseDrawn = false;

bool isHeartsBaseDrawn = false;
short heartsState = 0;

bool ledState = false;
bool lastButtonState = HIGH;

int heartsMode = 0;

int displayMode = 0;
constexpr int DISPLAY_MODES_COUNT = 3;

// Выбираем вашу модель e-Paper (2.13", BW, B72) — подойдет Waveshare 2.13" HAT
GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT>
display(GxEPD2_213_B74(/*CS=*/22, /*DC=*/21, /*RST=*/4, /*BUSY=*/17));


// class LedCallback : public BLECharacteristicCallbacks {
//     void onWrite(BLECharacteristic *pCharacteristic) override {
//         std::string value = pCharacteristic->getValue();
//
//         if (!value.empty()) {
//             if (value[0] == '1') {
//                 digitalWrite(LED_PIN, HIGH);
//             } else if (value[0] == '0') {
//                 digitalWrite(LED_PIN, LOW);
//             }
//         }
//     }
// };
//
// void setupBle() {
//     BLEDevice::init("ESP32_LED");
//
//     BLEServer *pServer = BLEDevice::createServer();
//     BLEService *pService = pServer->createService(SERVICE_UUID);
//
//     pCharacteristic = pService->createCharacteristic(
//       CHARACTERISTIC_UUID,
//       BLECharacteristic::PROPERTY_READ |
//       BLECharacteristic::PROPERTY_WRITE
//     );
//
//     pCharacteristic->setCallbacks(new LedCallback());
//     pCharacteristic->setValue("0");
//
//     pService->start();
//
//     BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
//     pAdvertising->start();
// }

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

void drawHearts(int heartsState) {
    short scale = 5;
    short xOffset = -25;
    short yOffset = -15;
    // if (!isHeartsBaseDrawn) {
        display.fillScreen(GxEPD_WHITE);
        draw(scale, xOffset, yOffset, heartsBaseY, 14, heartsBaseX, GxEPD_BLACK);
        // isHeartsBaseDrawn = true;
    // }
    for (int i = 0; i <= heartsState; i++) {
        if (i % 2 == 0) {
            draw(scale, xOffset + i % 6 / 2 * scale * 15, yOffset + i / 6 * scale * 11, heartFillingHalfY, heartFillingHalfSize, heartFillingHalfX, GxEPD_BLACK);
        } else {
            // TODO заменить full на правую половину
            draw(scale, xOffset + i % 6 / 2 * scale * 15, yOffset + i / 6 * scale * 11, heartFillingFullY, heartFillingFullSize, heartFillingFullX, GxEPD_BLACK);
        }
    }
    display.display(true);
}

void drawTemp() {
    short scale = 8;
    short xOffset = 0;
    short yOffset = 0;
    display.fillScreen(GxEPD_WHITE);
    draw(scale, xOffset, yOffset, tempExampleY, tempExampleSize, tempExampleX, GxEPD_BLACK);
    display.display(true);
}

void drawTime() {
    short scale = 8;
    short xOffset = 5;
    short yOffset = 0;
    display.fillScreen(GxEPD_WHITE);
    draw(scale, xOffset, yOffset, timeExampleY, timeExampleSize, timeExampleX, GxEPD_BLACK);
    display.display(true);
}

void handleButton() {
    bool buttonState = !digitalRead(BUTTON_PIN);

    // ловим момент нажатия
    if (buttonState == HIGH && lastButtonState == LOW) {
        // drawDynamicCuteFace();
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
        // delay(100); // антидребезг (простой)
        displayMode = (displayMode + 1) % DISPLAY_MODES_COUNT;
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

void setup() {
    Serial.begin(115200);
    Serial.println("e-Paper demo started");

    // Диод
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Кнопка
    pinMode(BUTTON_PIN, INPUT);

    // Дисплей
    initDisplay();

    // BLE
    // setupBle();
}

void loop() {
    // blink();
    handleButton();
    // delay(5000);
}

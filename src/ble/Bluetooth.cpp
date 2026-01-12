//
// Created by EGOR FORESTER on 12.01.2026.
//

#include "Bluetooth.h"

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcdefab-1234-5678-1234-abcdefabcdef"
#define LED_PIN 2

BLECharacteristic *pCharacteristic;

class LedCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override {
        std::string value = pCharacteristic->getValue();

        if (!value.empty()) {
            if (value[0] == '1') {
                digitalWrite(LED_PIN, HIGH);
            } else if (value[0] == '0') {
                digitalWrite(LED_PIN, LOW);
            }
        }
    }
};

void setupBle() {
    BLEDevice::init("ESP32_LED");

    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE
    );

    pCharacteristic->setCallbacks(new LedCallback());
    pCharacteristic->setValue("0");

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->start();
}
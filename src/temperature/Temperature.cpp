#include "Temperature.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <math.h>

#include "wifi/wifi.h"

Temperature::Temperature(const char* url, int ledPin) : _url(url), _temp(0), _ledPin(ledPin) {}

int Temperature::get() const {
    return _temp;
}

void Temperature::fetch() {
    if (_url == nullptr) return;

    if (_ledPin >= 0) digitalWrite(_ledPin, HIGH);
    checkWiFiConnection();

    HTTPClient http;
    String url = String(_url);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, http.getString());

        _temp = round(doc["current"]["temperature_2m"].as<float>());
    }

    http.end();
    if (_ledPin >= 0) digitalWrite(_ledPin, LOW);
}

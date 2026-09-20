#include "Temperature.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <math.h>

#include "wifi/wifi.h"

Temperature::Temperature(const char *url, int ledPin) : _url(url), _temp(0), _maxTemp(0), _ledPin(ledPin) {
}

int Temperature::getCurrentTemp() const {
    return _temp;
}

int Temperature::getMaxDailyTemp() const {
    return _maxTemp;
}

void Temperature::fetch() {
    if (_url == nullptr) return;

    // if (_ledPin >= 0) digitalWrite(_ledPin, HIGH);
    checkWiFiConnection();

    HTTPClient http;
    String url = String(_url);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, http.getString());

        _temp = round(doc["current"]["temperature_2m"].as<float>());
        JsonArray hourlyTemps = doc["hourly"]["temperature_2m"].as<JsonArray>();

        if (!hourlyTemps.isNull() && hourlyTemps.size() > 0) {
            float maxTemp = hourlyTemps[0].as<float>();

            for (JsonVariant value: hourlyTemps) {
                float temp = value.as<float>();
                if (temp > maxTemp) {
                    maxTemp = temp;
                }
            }

            _maxTemp = round(maxTemp);
        }
    }

    http.end();
    // if (_ledPin >= 0) digitalWrite(_ledPin, LOW);
}

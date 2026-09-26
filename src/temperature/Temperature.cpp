#include "Temperature.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <math.h>

#include "wifi/wifi.h"
#include "secrets.h"

Temperature::Temperature(int ledPin) : _temp(0), _maxTemp(0), _ledPin(ledPin) {
}

int Temperature::getCurrentTemp() const {
    return _temp;
}

int Temperature::getMaxDailyTemp() const {
    return _maxTemp;
}

void Temperature::fetch() {
    // if (_ledPin >= 0) digitalWrite(_ledPin, HIGH);
    checkWiFiConnection();

    HTTPClient http;
    String url = String(WEATHER_SERVER_URL) + "?latitude=55.998227&longitude=37.210115&current=temperature_2m&timezone=Europe/Moscow&forecast_days=1&hourly=temperature_2m";
    http.begin(url);
    http.addHeader("X-Api-Key", WEATHER_API_KEY);

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

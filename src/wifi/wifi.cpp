#include "wifi.h"

#include <WiFi.h>

const char *ssid = "forester2g";
const char *password = "zcbm0000";

void setupWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected");
}

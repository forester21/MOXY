#pragma once

#include <Arduino.h>

class Temperature {
public:
    // url: weather API URL, ledPin: optional pin used to show network activity (pass -1 to disable)
    explicit Temperature(const char* url = nullptr, int ledPin = -1);
    int get() const;
    void fetch();

private:
    const char* _url;
    int _temp;
    int _ledPin;
};

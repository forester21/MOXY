#include "img.h"
#include <Arduino.h>

const short eyesLeftY[] PROGMEM = {
    5, 4,
    6, 4,
};
const short eyesLeftX[] PROGMEM = {
    5, 6, 22, 23,
    5, 6, 22, 23,
};
const short eyesRightY[] PROGMEM = {
    5, 4,
    6, 4,
};
const short eyesRightX[] PROGMEM = {
    7, 8, 24, 25,
    7, 8, 24, 25,
};
const short eyesBaseY[] PROGMEM = {
    4, 8,
    5, 4,
    6, 4,
    7, 12,
    8, 12,
    9, 10,
    10, 7,
    11, 6,
};
const short eyesBaseX[] PROGMEM = {
    5, 6, 7, 8, 22, 23, 24, 25,
    4, 9, 21, 26,
    4, 9, 21, 26,
    4, 5, 6, 7, 8, 9, 21, 22, 23, 24, 25, 26,
    4, 5, 6, 7, 8, 9, 21, 22, 23, 24, 25, 26,
    5, 6, 7, 8, 11, 19, 22, 23, 24, 25,
    11, 12, 14, 15, 16, 18, 19,
    12, 13, 14, 16, 17, 18,
};

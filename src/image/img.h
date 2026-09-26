#pragma once

extern const short eyesLeftY[];
extern const short eyesLeftX[];
extern const short eyesRightY[];
extern const short eyesRightX[];
extern const short eyesBaseY[];
extern const short eyesBaseX[];
extern const short heartsBaseY[];
extern const short heartsBaseX[];
extern const short heartFillingFullY[];
extern const short heartFillingFullX[];
extern const short heartFillingFullSize;
extern const short heartFillingHalfY[];
extern const short heartFillingHalfX[];
extern const short heartFillingHalfSize;

const short ppmY[] = {
    2, 11,
    3, 7,
    4, 9,
    5, 2,
 };
const short ppmX[] = {
    2, 3, 4, 6, 7, 8, 10, 11, 12, 13, 14,
    2, 4, 6, 8, 10, 12, 14,
    2, 3, 4, 6, 7, 8, 10, 12, 14,
    2, 6,
 };
const short ppmSize = 4;

const short ppm2Y[] = {
    0, 4,
    1, 2,
    2, 3,
    4, 4,
    5, 2,
    6, 3,
    8, 3,
    9, 1,
    10, 3,
    11, 1,
    12, 3,
 };
const short ppm2X[] = {
    0, 1, 2, 3,
    1, 3,
    1, 2, 3,
    0, 1, 2, 3,
    1, 3,
    1, 2, 3,
    1, 2, 3,
    3,
    1, 2, 3,
    3,
    1, 2, 3,
 };
const short ppm2Size = 11;
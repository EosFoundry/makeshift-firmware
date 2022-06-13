#pragma once
#ifndef ILI9341_H_
#define ILI9341_H_

#include <Arduino.h>

#include <ILI9341_T4.h>
#include <tgx.h>

#include <widget.hpp>

inline namespace mkshft_display {
using namespace tgx;
const uint8_t CS_PIN = 9;
const uint8_t TOUCH_CS_PIN = 1;
const uint8_t TOUCH_IRQ_PIN = 0;
const uint8_t DC_PIN = 10;
const uint8_t RST_PIN = 255;
const uint8_t SDI_PIN = 11;
const uint8_t SDO_PIN = 12;
const uint8_t SCK_PIN = 13;

const uint32_t SPI_SPEED = 40000000;

const uint16_t LX = 320;
const uint16_t LY = 240;

void init();
void calibrateTouch();
void test();

} // namespace mkshft_display

#endif // ILI9341_H_

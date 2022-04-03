#ifndef LED_H_
#define LED_H_

#include <Adafruit_NeoPixel.h>
#include <list>

namespace mkshft_led
{

    const uint8_t LED_PIN = 6;
    const uint8_t LED_ON = 0;
    
    const bool ROW = 0;
    const bool COL = 1;

    const uint8_t RowSz = 4;
    const uint8_t ColSz = 4;

    const uint32_t refreshDelta = 1;
    const uint32_t frames = 1;

    // Lookup table for strip number given row + col
    const uint8_t MatrixLookup[RowSz][ColSz] = {
        {3, 2, 1, 0},
        {4, 5, 6, 7},
        {11, 10, 8, 9},
        {12, 13, 14, 15},
    };
    
    // Lookup table for row + col given strip number
    const uint8_t StripLookup[RowSz*ColSz][2] = {
        {0,3}, {0,2}, {0,1}, {0,0},
        {1,0}, {1,1}, {1,2}, {1,3},
        {2,3}, {2,2}, {2,1}, {2,0},
        {3,0}, {3,1}, {3,2}, {3,3}
    };

    struct Color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    struct ColorEvent
    {
        Color color;
        uint32_t lengthFrames;
        bool adjusted;
    };

    struct Pixel
    {
        std::list<ColorEvent> colorQueue;
        std::list<ColorEvent>::iterator activeEvent;
        uint32_t framesLeft;
        bool loop;
        bool on;
    };

    extern Pixel ledMatrix[RowSz][ColSz];

    void init();
    void updateState();


    void clear(uint8_t row, uint8_t col);

    void colorPixel(uint8_t row, uint8_t col,
                    uint8_t r, uint8_t g, uint8_t b);

    void applyToMatrix(void (*apply)(uint8_t, uint8_t));

    void createFadeSequence(uint32_t frames, Color start, Color end);

    void printPixel(uint8_t n);
    void printPixel(uint8_t r, uint8_t c);

    const uint8_t gamma8[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
        2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
        5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
        10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
        17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
        25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
        37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
        51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
        69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
        90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
        115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
        144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
        177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
        215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255};
}

#endif // LED_H_

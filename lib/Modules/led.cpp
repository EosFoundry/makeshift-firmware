#include <led.hpp>

namespace mkshft_led
{
    Adafruit_NeoPixel strip(RowSz * ColSz, LED_PIN, NEO_GRB + NEO_KHZ800);
    Pixel ledMatrix[RowSz][ColSz];

    void updateState()
    {
        for (int r = 0; r < RowSz; r++)
        {
            for (int c = 0; c < ColSz; c++)
            {
                if (ledMatrix[r][c].on)
                {

                    // printPixel(r, c);
                    if (c == 2)
                    {
                        colorPixel(
                            r, c,
                            ledMatrix[r][c].currentColor->r,
                            ledMatrix[r][c].currentColor->g,
                            ledMatrix[r][c].currentColor->b);
                    }
                    else
                    {
                        colorPixelAdjusted(
                            r, c,
                            ledMatrix[r][c].currentColor->r,
                            ledMatrix[r][c].currentColor->g,
                            ledMatrix[r][c].currentColor->b);
                    }

                    if (ledMatrix[r][c].framesLeft == 0)
                    {
                        ledMatrix[r][c].currentColor++;
                        if (ledMatrix[r][c].currentColor != ledMatrix[r][c].colorQueue.end())
                        {
                            ledMatrix[r][c].framesLeft = ledMatrix[r][c].currentColor->lengthFrames - 1;
                            Serial.println("got end of colorQueue");
                        }
                        else
                        {
                            ledMatrix[r][c].currentColor++;
                        }
                    }
                    else
                    {
                        ledMatrix[r][c].framesLeft--;
                    }
                }
            }
        }
        strip.show();
    }

    void colorPixelAdjusted(uint8_t row, uint8_t col,
                            uint8_t r, uint8_t g, uint8_t b)
    {
        strip.setPixelColor(
            MatrixLookup[row][col],
            gamma8[r],
            gamma8[g],
            gamma8[b]);
    }

    void colorPixel(uint8_t row, uint8_t col,
                    uint8_t r, uint8_t g, uint8_t b)
    {
        strip.setPixelColor(MatrixLookup[row][col], r, g, b);
    }

    void clear(uint8_t r, uint8_t c)
    {
        ledMatrix[r][c].on = false;
        ledMatrix[r][c].colorQueue.clear();
    }

    void init()
    {
        pinMode(LED_ON, OUTPUT);
        pinMode(LED_PIN, OUTPUT);
        digitalWrite(LED_ON, HIGH);

        strip.begin();
        strip.show();
        delay(1000);

        colorPixel(2, 1, 25, 0, 0);
        strip.show();
        delay(1000);
        Serial.println("pushing events to colorQueue");
        for (uint16_t j = 0; j < 256; j++)
        {
            ColorEvent e = {j, 0, j, frames};
            ledMatrix[1][1].colorQueue.push_back(e);
            ColorEvent e2 = {j, 0, j, frames};
            ledMatrix[1][2].colorQueue.push_back(e2);
        }

        for (uint16_t j = 255; j != 0; j--)
        {
            ColorEvent e = {j, 0, j, frames};
            ledMatrix[1][1].colorQueue.push_back(e);
            ColorEvent e2 = {j, 0, j, frames};
            ledMatrix[1][2].colorQueue.push_back(e2);
        }

        ledMatrix[1][1].currentColor = ledMatrix[1][1].colorQueue.begin();
        ledMatrix[1][2].currentColor = ledMatrix[1][2].colorQueue.begin();
        ledMatrix[1][1].on = true;
        ledMatrix[1][2].on = true;

        for (int i = 0; i < 4; i++)
        {
            colorPixel(2, i, 25, 0, 25);
        }
        strip.show();
        delay(1000);

        strip.clear();
        strip.show();
    }
}
#include <led.hpp>

namespace mkshft_led
{
    Adafruit_NeoPixel strip(RowSz *ColSz, LED_PIN, NEO_GRB + NEO_KHZ800);
    Pixel ledMatrix[RowSz][ColSz];
    int r, g, b;
    bool adjusted;

    void updateState()
    {
        // // loop iterates through rows then columns of LEDs
        // for (uint8_t row = 0; row < RowSz; row++)
        // {
        //     for (uint8_t col = 0; col < ColSz; col++)
        //     {
        //         adjusted = ledMatrix[row][col].currentColor->adjusted;
        //         r = ledMatrix[row][col].currentColor->r & ledMatrix[row][col].on;
        //         g = ledMatrix[row][col].currentColor->g & ledMatrix[row][col].on;
        //         b = ledMatrix[row][col].currentColor->b & ledMatrix[row][col].on;
        //         // sets rgb with bitwise operations to go fast
        //         r = (adjusted & gamma8[r]) | (~adjusted & r);
        //         g = (adjusted & gamma8[g]) | (~adjusted & g);
        //         b = (adjusted & gamma8[b]) | (~adjusted & b);

        //         printPixel(row, col);
        //         colorPixel(row, col, r, g, b);

        //         advance frames of LEDs that are ON
        //                 if (ledMatrix[row][col].on)
        //                 {
        //                     if (ledMatrix[row][col].framesLeft == 0)
        //                     {
        //                         ledMatrix[row][col].currentColor++;
        //                         if (ledMatrix[row][col].currentColor != ledMatrix[row][col].colorQueue.end())
        //                         {
        //                             ledMatrix[row][col].framesLeft = ledMatrix[row][col].currentColor->lengthFrames - 1;
        //                             Serial.println("got end of colorQueue");
        //                         }
        //                         else
        //                         {
        //                             ledMatrix[row][col].currentColor++;
        //                             // switch off if looking is disabled
        //                             ledMatrix[row][col].on = ledMatrix[row][col].loop & ledMatrix[row][col].on;
        //                         }
        //                     }
        //                     else
        //                     {
        //                         ledMatrix[row][col].framesLeft--;
        //                     }
        //                 }
        //     }
        // }
        // strip.show();
    }

    void init()
    {
        Serial.println("LED Initialization");
        pinMode(LED_ON, OUTPUT);
        pinMode(LED_PIN, OUTPUT);
        digitalWrite(LED_ON, HIGH);
        delay(2000);

        strip.begin();
        strip.show();
        delay(10);
        Serial.println("Clearing LED matrix queues");

        applyToMatrix(clear);
        applyToMatrix(printPixel);

        // strip.show();
        // delay(1000);
        // Serial.println("pushing events to colorQueue");
        // for (uint16_t j = 0; j < 256; j++)
        // {
        //     ColorEvent e = {j, 0, j, frames, true};
        //     ledMatrix[1][1].colorQueue.push_back(e);
        //     ColorEvent e2 = {j, 0, j, frames, false};
        //     ledMatrix[1][2].colorQueue.push_back(e2);
        // }

        // for (uint16_t j = 255; j != 0; j--)
        // {
        //     ColorEvent e = {j, 0, j, frames, true};
        //     ledMatrix[1][1].colorQueue.push_back(e);
        //     ColorEvent e2 = {j, 0, j, frames, false};
        //     ledMatrix[1][2].colorQueue.push_back(e2);
        // }

        // ledMatrix[1][1].currentColor = ledMatrix[1][1].colorQueue.begin();
        // ledMatrix[1][2].currentColor = ledMatrix[1][2].colorQueue.begin();
        // ledMatrix[1][1].on = true;
        // ledMatrix[1][1].loop = true;
        // ledMatrix[1][2].on = true;

        // for (int i = 0; i < 4; i++)
        // {
        //     colorPixel(2, i, 25, 0, 25);
        // }
        // strip.show();
        // delay(1000);

        // strip.clear();
        // strip.show();
    }

    void colorPixel(uint8_t row, uint8_t col, uint8_t r, uint8_t g, uint8_t b)
    {
        strip.setPixelColor(MatrixLookup[row][col], r, g, b);
    }

    void applyToMatrix(void (*apply)(uint8_t, uint8_t))
    {
        for (uint8_t n = 0; n < (RowSz * ColSz); n++)
        {
            (*apply)(StripLookup[n][ROW], StripLookup[n][COL]);
        }
    }

    void clear(uint8_t row, uint8_t col)
    {
        ledMatrix[row][col].on = false;
        ledMatrix[row][col].loop = false;
        ledMatrix[row][col].colorQueue.clear();
        ledMatrix[row][col].currentColor = ledMatrix[row][col].colorQueue.begin();
        ledMatrix[row][col].framesLeft = 0;
    }

    void printPixel(uint8_t n)
    {
        printPixel(StripLookup[n][ROW], StripLookup[n][COL]);
    }

    void printPixel(uint8_t r, uint8_t c)
    {
        Serial.print("pixel: ");
        Serial.print(r);
        Serial.print(",");
        Serial.print(c);
        Serial.print(" | ");
        Serial.print("framesLeft: ");
        Serial.print(ledMatrix[r][c].framesLeft);
        Serial.print(" | ");
        Serial.print("lengthFrames: ");
        Serial.print(ledMatrix[r][c].currentColor->lengthFrames);
        Serial.print(" | ");
        Serial.print("rgb: ");
        Serial.print(ledMatrix[r][c].currentColor->r);
        Serial.print(",");
        Serial.print(ledMatrix[r][c].currentColor->g);
        Serial.print(",");
        Serial.println(ledMatrix[r][c].currentColor->b);
    }
}
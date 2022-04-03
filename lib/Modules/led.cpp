#include <led.hpp>
#define DEBUG 1

namespace mkshft_led {
Adafruit_NeoPixel strip(RowSz *ColSz, LED_PIN, NEO_GRB + NEO_KHZ800);
Pixel ledMatrix[RowSz][ColSz];
int r, g, b;
bool adjusted;

void updateState() {
  // loop iterates through rows then columns of LEDs
  for (uint8_t row = 0; row < RowSz; row++) {
    for (uint8_t col = 0; col < ColSz; col++) {
      adjusted = ledMatrix[row][col].activeEvent->adjusted;
      r = ledMatrix[row][col].activeEvent->color.r * ledMatrix[row][col].on;
      g = ledMatrix[row][col].activeEvent->color.g * ledMatrix[row][col].on;
      b = ledMatrix[row][col].activeEvent->color.b * ledMatrix[row][col].on;

#ifdef DEBUG
      Serial.print("rgb calc: ");
      Serial.print(r);
      Serial.print(",");
      Serial.print(g);
      Serial.print(",");
      Serial.print(b);
#endif
      // sets rgb with bitwise operations to go fast
      r = (adjusted * gamma8[r]) | (!adjusted * r);
      g = (adjusted * gamma8[g]) | (!adjusted * g);
      b = (adjusted * gamma8[b]) | (!adjusted * b);

#ifdef DEBUG
      Serial.print(" | adjustment: ");
      Serial.print(adjusted);
      Serial.print(" | ");
      Serial.print(r);
      Serial.print(",");
      Serial.print(g);
      Serial.print(",");
      Serial.print(b);
      Serial.print(" | ");
      printPixel(row, col);
#endif

      colorPixel(row, col, r, g, b);

      // advance frames of LEDs that are ON
      if (ledMatrix[row][col].on) {
        if (ledMatrix[row][col].framesLeft == 0) {
          ledMatrix[row][col].activeEvent++;
          if (ledMatrix[row][col].activeEvent !=
              ledMatrix[row][col].colorQueue.end()) {
            ledMatrix[row][col].framesLeft =
                ledMatrix[row][col].activeEvent->lengthFrames - 1;
            // Serial.println("got end of colorQueue");
          } else {
            ledMatrix[row][col].activeEvent++;
            // switch off if looking is disabled
            ledMatrix[row][col].on =
                ledMatrix[row][col].loop && ledMatrix[row][col].on;
          }
        } else {
          ledMatrix[row][col].framesLeft--;
        }
      }
    }
  }
  strip.show();
}

void init() {
  Serial.println("LED Initialization");
  pinMode(LED_ON, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_ON, HIGH);
  delay(10);

  strip.begin();
  strip.show();
  delay(10);
  Serial.println("Clearing LED matrix queues");
  applyToMatrix(clear);
  applyToMatrix(printPixel);
  Serial.println("ED matrix queues");
  strip.show();
  delay(10);
  Serial.println("pushing events to colorQueue");
  {
    uint8_t j = 0;
    do {
      ColorEvent e = {j, 0, j, frames, true};
      ledMatrix[1][1].colorQueue.push_back(e);
      ColorEvent e2 = {j, 0, j, frames, false};
      ledMatrix[1][2].colorQueue.push_back(e2);
      j++;
    } while (j != 255);

    j = 255;
    do {
      ColorEvent e = {j, 0, j, frames, true};
      ledMatrix[1][1].colorQueue.push_back(e);
      ColorEvent e2 = {j, 0, j, frames, false};
      ledMatrix[1][2].colorQueue.push_back(e2);
      j--;
    } while (j != 0);

    ledMatrix[1][1].activeEvent = ledMatrix[1][1].colorQueue.begin();
    ledMatrix[1][2].activeEvent = ledMatrix[1][2].colorQueue.begin();
    ledMatrix[1][1].on = true;
    ledMatrix[1][1].loop = true;
    ledMatrix[1][2].on = true;
    ledMatrix[1][2].loop = false;
  }

  for (int i = 0; i < 4; i++) {
    colorPixel(2, i, 25, 0, 25);
  }
  strip.show();
  delay(1000);

  strip.clear();
  strip.show();
}

void colorPixel(uint8_t row, uint8_t col, uint8_t r, uint8_t g, uint8_t b) {
  strip.setPixelColor(MatrixLookup[row][col], r, g, b);
}

void applyToMatrix(void (*apply)(uint8_t, uint8_t)) {
  // unrolling nested loop iteration to iterating over the strip using lookup
  for (uint8_t n = 0; n < (RowSz * ColSz); n++) {
    (*apply)(StripLookup[n][ROW], StripLookup[n][COL]);
  }
}

void clear(uint8_t row, uint8_t col) {
  ledMatrix[row][col].on = false;
  ledMatrix[row][col].loop = false;
  ledMatrix[row][col].colorQueue.clear();
  ledMatrix[row][col].activeEvent = ledMatrix[row][col].colorQueue.begin();
  ledMatrix[row][col].framesLeft = 0;
}

void printPixel(uint8_t n) {
  printPixel(StripLookup[n][ROW], StripLookup[n][COL]);
}

void printPixel(uint8_t r, uint8_t c) {
  Serial.print("pixel: ");
  Serial.print(r);
  Serial.print(",");
  Serial.print(c);
  Serial.print(" | ");
  Serial.print("framesLeft: ");
  Serial.print(ledMatrix[r][c].framesLeft);
  Serial.print(" | ");
  Serial.print("lengthFrames: ");
  Serial.print(ledMatrix[r][c].activeEvent->lengthFrames);
  Serial.print(" | ");
  Serial.print("rgb: ");
  Serial.print(ledMatrix[r][c].activeEvent->color.r);
  Serial.print(",");
  Serial.print(ledMatrix[r][c].activeEvent->color.g);
  Serial.print(",");
  Serial.println(ledMatrix[r][c].activeEvent->color.b);
}
} // namespace mkshft_led

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
      // Serial.print("rgb calc: ");
      // Serial.print(r);
      // Serial.print(",");
      // Serial.print(g);
      // Serial.print(",");
      // Serial.print(b);
#endif
      // sets rgb with bitwise operations to go fast
      r = (adjusted * gamma8[r]) | (!adjusted * r);
      g = (adjusted * gamma8[g]) | (!adjusted * g);
      b = (adjusted * gamma8[b]) | (!adjusted * b);

#ifdef DEBUG
      // Serial.print(" | adjustment: ");
      // Serial.print(adjusted);
      // Serial.print(" | ");
      // Serial.print(r);
      // Serial.print(",");
      // Serial.print(g);
      // Serial.print(",");
      // Serial.print(b);
      // Serial.print(" | ");
      // printPixel(row, col);
#endif

      colorPixel(row, col, r, g, b);

      // advance frames for LEDs that are ON
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
  // setup pinmodes
  pinMode(LED_ON, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_ON, HIGH);
  delay(10);

  // matrix initialization to zero
  strip.begin();
  strip.show();
  delay(10);
  Serial.println("Clearing LED matrix queues");
  applyToMatrix(clear);
  applyToMatrix(printPixel);
  Serial.println("LED matrix queues cleared");
  strip.show();
  delay(10);

  // do any LED POST
  post();

  // LED testing block
  {
    Serial.println("pushing events to colorQueue");
    uint8_t j = 0;
    do {
      ColorEvent e = {{j, 0, j}, frames, true};
      ledMatrix[1][1].colorQueue.push_back(e);
      ColorEvent e2 = {{j, 0, j}, frames, false};
      ledMatrix[1][2].colorQueue.push_back(e2);
      j++;
    } while (j != 255);

    j = 255;

    do {
      ColorEvent e = {{j, 0, j}, frames, true};
      ledMatrix[1][1].colorQueue.push_back(e);
      ColorEvent e2 = {{j, 0, j}, frames, false};
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


  strip.clear();
  strip.show();
}

void post() {
  // TODO: finish testing
  Serial.println("Testing color deltas");
  Color test1 = {0, 0, 0};
  Color test2 = {255, 255, 255};
  Serial.print("t1: ");
  printColor(test1);
  Serial.print(" | t2: ");
  printColor(test2);
  ColorDelta delta12 = test1 - test2;
  ColorDelta delta21 = test2 - test1;

  Serial.print(" | d12: ");
  printColor(delta12);
  Serial.print(" | d21: ");

  printColor(delta21);
  Serial.println();

  // color indicators
  for (uint8_t i = 125; i != 255; i--) {
    const Color color = {gamma8[i], 0, gamma8[i]};
    for (int n = 0 ; n < (RowSz * ColSz) ; n++) {
      colorPixel(StripLookup[n][ROW], StripLookup[n][COL], color);
    }
    strip.show();
    delay(10);
  }
  delay(1000);

  Serial.println("Testing fade sequence");
  ledMatrix[1][0].colorQueue = createFadeSequence(150, {45,0,45}, {100, 155, 155});
  ledMatrix[1][0].colorQueue.splice(
    ledMatrix[1][0].colorQueue.end(),
    createFadeSequence(150, {100, 155, 155},{45,0,45}) );
  ledMatrix[1][0].on = true;
  ledMatrix[1][0].loop = true;
}

void colorPixel(uint8_t row, uint8_t col, uint8_t r, uint8_t g, uint8_t b) {
  strip.setPixelColor(MatrixLookup[row][col], r, g, b);
}
void colorPixel(uint8_t row, uint8_t col, Color c) {
  colorPixel(row, col, c.r, c.g, c.b);
}

std::list<ColorEvent> createFadeSequence(uint32_t frames, Color start, Color end) {
  // TODO: implement modulo arithemetic
  std::list<ColorEvent> newQ;
  float r, g, b;
  uint8_t arr, gee, bee;
  float ratioR, ratioG, ratioB;
  ColorDelta delta = end - start;
  newQ.clear();
  printColor(delta);
  Serial.println();

  r = (float)start.r;
  g = (float)start.g;
  b = (float)start.b;

  ratioR = (float)delta.r/frames;
  ratioG = (float)delta.g/frames;
  ratioB = (float)delta.b/frames;

  Serial.print("float ratios: ");
  Serial.print(ratioR);
  Serial.print(",");
  Serial.print(ratioG);
  Serial.print(",");
  Serial.print(ratioB);
  Serial.println();

  newQ.push_front({start, 1, true});
  while( frames != 0 )  {
    r = r + ratioR;
    g = g + ratioG;
    b = b + ratioB;
    arr = (uint8_t)round(r);
    gee = (uint8_t)round(g);
    bee = (uint8_t)round(b);

    newQ.push_back({{arr, gee, bee}, 1, true});
    frames --;
  }
  newQ.push_back({end, 1, true});
  return newQ;
}

ColorDelta operator-(const Color first, const Color second) {
  ColorDelta delta;
  delta.r = first.r - second.r;
  delta.b = first.b - second.b;
  delta.g = first.g - second.g;
  return delta;
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

void printColor(ColorDelta d) {
  Serial.print(d.r);
  Serial.print(",");
  Serial.print(d.g);
  Serial.print(",");
  Serial.print(d.b);
}

void printColor(Color c) {
  Serial.print(c.r);
  Serial.print(",");
  Serial.print(c.g);
  Serial.print(",");
  Serial.print(c.b);
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
  printColor(ledMatrix[r][c].activeEvent->color);
  Serial.println();
}
} // namespace mkshft_led

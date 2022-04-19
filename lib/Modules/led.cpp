#include <led.hpp>
#define DEBUG 1
#define ROW StripLookup[n][0]
#define COL StripLookup[n][1]

namespace mkshft_led {
Adafruit_NeoPixel strip(RowSz *ColSz, LED_PIN, NEO_GRB + NEO_KHZ800);
ColorSequence pixelOffSequence;
Pixel ledMatrix[RowSz][ColSz];
int r, g, b;
bool adjusted;
bool decrement;

void updatePixelSequence() {
  for (uint8_t n = 0; n < StripSz; n++) {
    if (ledMatrix[ROW][COL].awaitAdvanceSequence == true) {
      if (ledMatrix[ROW][COL].awaitRestart) {
        ledMatrix[ROW][COL].awaitRestart = false;
        ledMatrix[ROW][COL].activeSequence = ledMatrix[ROW][COL].risingEdgeSequence;
      } else {
        ledMatrix[ROW][COL].activeSequence = ledMatrix[ROW][COL].fallingEdgeSequence;
      }
      if (ledMatrix[ROW][COL].awaitStopLoop) {
      }
    }
  }
}

void updateState() {
  // loop iterates through rows then columns of LEDs
  for (uint8_t n = 0; n < StripSz; n++) {
    adjusted = ledMatrix[ROW][COL].activeEvent->adjusted;
    r = ledMatrix[ROW][COL].activeEvent->color.r;
    g = ledMatrix[ROW][COL].activeEvent->color.g;
    b = ledMatrix[ROW][COL].activeEvent->color.b;

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
    // printPixel(ROW, COL);
#endif

    colorStripPixel(ROW, COL, r, g, b);
  }
  strip.show();

  for (uint8_t n = 0; n < StripSz; n++) {
    // check if pixel is frozen
    if (ledMatrix[ROW][COL].awaitAdvanceSequence == false) {
      // end of current color event
      if (ledMatrix[ROW][COL].framesLeft == 0) {
        // advance to the next event
        ledMatrix[ROW][COL].activeEvent++;

        // check if the end of sequence has been reached
        if (ledMatrix[ROW][COL].activeEvent ==
            ledMatrix[ROW][COL].activeSequence.events.end()) {
          if (ledMatrix[ROW][COL].activeSequence.loop) {
            // restart sequence if it loops
            ledMatrix[ROW][COL].activeEvent++;
          } else { // pass to editing logic to figure out next sequence
            ledMatrix[ROW][COL].awaitAdvanceSequence = true;
            continue;
          }
        }

        // get frame count from new event
        ledMatrix[ROW][COL].framesLeft =
            ledMatrix[ROW][COL].activeEvent->lengthFrames - 1;
      } else {
        // count down frames if not the last frame
        ledMatrix[ROW][COL].framesLeft--;
      }
    }
  }
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
  applyToMatrix(clearPixelQueue);
  applyToMatrix(printPixel);
  Serial.println("LED matrix queues cleared");
  strip.show();
  delay(10);

  pixelOffSequence.events.push_front({OFF, 10, false});
  pixelOffSequence.loop = true;

  // LED power-on-self-test
  // post();

  // Set up default light-ups
  Serial.println("setting default light-ups");
  int row, col;
  Color cStart, cEnd;
  cStart = {0, 0, 0};
  cEnd = {165, 45, 165};
  for (int n = 0; n < StripSz; n++) {
    row = ROW;
    col = COL;
    ColorSequence cQuence;
    ColorSequence cQuenceTwo;
    cQuence = createFadeSequence(5, cStart, cEnd);
    cQuenceTwo = createFadeSequence(500, cEnd, cStart, true, false);

    while (!cQuenceTwo.events.empty()) {
      cQuence.events.push_back(cQuenceTwo.events.front());
      cQuenceTwo.events.pop_front();
    }
    ledMatrix[row][col].sequenceQueue.push_back(cQuence);
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
  for (uint8_t i = 0; i != 125; i++) {
    const Color color = {gamma8[i], 0, gamma8[i]};
    for (int n = 0; n < StripSz; n++) {
      colorStripPixel(ROW, COL, color);
    }
    strip.show();
    delay(1);
  }
  for (uint8_t i = 125; i != 255; i--) {
    const Color color = {gamma8[i], 0, gamma8[i]};
    for (int n = 0; n < StripSz; n++) {
      colorStripPixel(ROW, COL, color);
    }
    strip.show();
    delay(10);
  }
  Serial.println("end of POST");
  delay(1000);
}


void colorStripPixel(uint8_t row, uint8_t col, uint8_t r, uint8_t g, uint8_t b) {
  strip.setPixelColor(MatrixLookup[row][col], r, g, b);
}

void colorStripPixel(uint8_t row, uint8_t col, Color c) {
  colorStripPixel(row, col, c.r, c.g, c.b);
}

ColorSequence createFadeSequence(uint32_t frames, Color start, Color end) {
  return createFadeSequence(frames, start, end, true, false);
}

ColorSequence createFadeSequence(uint32_t frames, Color start, Color end, bool adjusted, bool isLooping) {
  ColorSequence sequence;
  std::list<ColorEvent> newQ;
  float rFlt, gFlt, bFlt;
  uint8_t arr, gee, bee, prevR, prevG, prevB;
  float ratioR, ratioG, ratioB;
  ColorDelta delta = end - start;
  newQ.clear();
  // printColor(delta);
  // Serial.println();

  rFlt = (float)start.r;
  gFlt = (float)start.g;
  bFlt = (float)start.b;

  ratioR = (float)delta.r/frames;
  ratioG = (float)delta.g/frames;
  ratioB = (float)delta.b/frames;

  // Serial.print("float ratios: ");
  // Serial.print(ratioR);
  // Serial.print(",");
  // Serial.print(ratioG);
  // Serial.print(",");
  // Serial.print(ratioB);
  // Serial.println();

  newQ.push_front({start, 1, adjusted});
  uint32_t eventFrames = 1;
  prevR = start.r;
  prevG = start.g;
  prevB = start.b;
  while( frames != 0 )  {
    rFlt = rFlt + ratioR;
    gFlt = gFlt + ratioG;
    bFlt = bFlt + ratioB;
    arr = (uint8_t)round(rFlt);
    gee = (uint8_t)round(gFlt);
    bee = (uint8_t)round(bFlt);


    if (arr == prevR && gee == prevG && bee == prevB) {
      eventFrames++;
    } else {
      // printColor((Color){arr, gee, bee});
      // Serial.print(" | frames: ");
      // Serial.print(eventFrames);
      // Serial.println();
      newQ.push_back({{prevR, prevG, prevB}, eventFrames, adjusted});
      eventFrames = 1;
    }
    prevR = arr;
    prevG = gee;
    prevB = bee;
    frames --;
  }
  newQ.push_back({end, 1, adjusted});
  sequence.events = newQ;
  sequence.loop = isLooping;
  return sequence;
}

ColorDelta operator-(const Color first, const Color second) {
  ColorDelta delta;
  delta.r = first.r - second.r;
  delta.b = first.b - second.b;
  delta.g = first.g - second.g;
  return delta;
}

void applyToMatrix(void (*apply)(uint8_t, uint8_t)) {
  // unrolling nested loop by iterating over the strip using lookup
  for (uint8_t n = 0; n < StripSz; n++) {
    (*apply)(ROW, COL);
  }
}

void clearPixelQueue(uint8_t row, uint8_t col) {
  ledMatrix[row][col].awaitAdvanceSequence = true;
  ledMatrix[row][col].sequenceQueue.clear();
  ledMatrix[row][col].sequenceQueue.push_front(pixelOffSequence);
  ledMatrix[row][col].activeSequence = pixelOffSequence;
  ledMatrix[row][col].activeEvent = ledMatrix[row][col].activeSequence.events.begin();
  ledMatrix[row][col].framesLeft = 0;
  ledMatrix[row][col].awaitRestart = false;
  ledMatrix[row][col].awaitStopLoop = false;
  ledMatrix[row][col].awaitAdvanceSequence = false;
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
  printPixel(ROW, COL);
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
  Serial.print(" | ");
  Serial.print("loop: ");
  Serial.print(ledMatrix[r][c].activeSequence.loop);
  Serial.print(" | ");
  Serial.print("await: ");
  Serial.print(ledMatrix[r][c].awaitRestart);
  Serial.println();
}
} // namespace mkshft_led

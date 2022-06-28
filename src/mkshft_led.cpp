#include <mkshft_led.hpp>
#define DEBUG 1
#define ROW StripLookup[n][0]
#define COL StripLookup[n][1]

namespace mkshft_ledMatrix {
Adafruit_NeoPixel strip(RowSz *ColSz, LED_PIN, NEO_GRB + NEO_KHZ800);
Pixel ledMatrix[RowSz][ColSz];
int r, g, b;
bool adjusted;
bool decrement;

void updateState() {
  for (uint8_t n = 0; n < StripSz; n++) {
    ledMatrix[ROW][COL].advanceFrame();
#ifdef DEBUG
    // ledMatrix[ROW][COL].printToSerial();
#endif
  }
}

void showMatrix() {
  // loop iterates through rows then columns of LEDs
  for (uint8_t n = 0; n < StripSz; n++) {
    colorStripPixel(ROW, COL, ledMatrix[ROW][COL].getFrame());
  }
  strip.show();
}

void init() {
  // setup pinmodes
  // pinMode(LED_ON, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  // digitalWrite(LED_ON, HIGH);
  delay(10);

  // matrix initialization to zero
  strip.begin();
  strip.show();
  delay(10);
  Serial.println("LED_MATRIX:: Initializing Pixel matrix");

  for (int n = 0; n < StripSz; n++) {
    ledMatrix[ROW][COL].setMatrixCoord(ROW, COL);
    // ledMatrix[ROW][COL].printToSerial();
  }
  Serial.println("LED_MATRIX:: Pixel matrix initialized");
  delay(10);


  // LED power-on-self-test
  // post();

  // Set up default light-ups
  Serial.println("LED_MATRIX:: setting default light sequences");
  int row, col;
  Color  cPeak = {165, 30, 155};
  for (int n = 0; n < StripSz; n++) {
    row = ROW;
    col = COL;
    ColorSequence cQuence[2];
    ColorSequence cQuenceTwo[2];
    cQuence[0] = createFadeSequence(15, ColorOFF, cPeak, false, false);
    cQuence[0].loop = false;
    cQuence[1].events.clear();
    cQuence[1].events.push_front({cPeak, 10, false});
    cQuence[1].loop = 1;

    cQuenceTwo[0] = createFadeSequence(18, cPeak, ColorOFF, false, false);
    cQuenceTwo[0].loop = false;
    cQuenceTwo[1].events.clear();
    cQuenceTwo[1].events.push_front(EventOFF);
    cQuenceTwo[1].loop = 1;

    // Serial.println("cQuence 0");
    // printSequence(cQuence[0]);
    // delay(10);
    // Serial.println("cQuence 1");
    // printSequence(cQuence[1]);
    // delay(10);
    // Serial.println("cQuenceTwo 0");
    // printSequence(cQuenceTwo[0]);
    // delay(10);
    // Serial.println("cQuenceTwo 1");
    // printSequence(cQuenceTwo[1]);
    // delay(10);

    ledMatrix[ROW][COL].setSequence(Pixel::RISE, cQuence[0], cQuence[1]);
    ledMatrix[ROW][COL].setSequence(Pixel::FALL, cQuenceTwo[0], cQuenceTwo[1]);
  }
  Serial.println("LED_MATRIX:: defaults setup successfully");

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

void colorStripPixel(uint8_t row, uint8_t col, uint8_t r, uint8_t g,
                     uint8_t b) {
  strip.setPixelColor(MatrixLookup[row][col], r, g, b);
}

void colorStripPixel(uint8_t row, uint8_t col, Color c) {
  colorStripPixel(row, col, c.r, c.g, c.b);
}



void applyToMatrix(void (*apply)(uint8_t, uint8_t)) {
  // unrolling nested loop by iterating over the strip using lookup
  for (uint8_t n = 0; n < StripSz; n++) {
    (*apply)(ROW, COL);
  }
}

} // namespace mkshft_ledMatrix

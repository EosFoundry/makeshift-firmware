#include <mkshft_display.hpp>
inline namespace mkshft_display {
// Display driver with the pins
ILI9341_T4::ILI9341Driver tft(CS_PIN, DC_PIN, SCK_PIN, SDI_PIN, SDO_PIN,
                              RST_PIN, TOUCH_CS_PIN, TOUCH_IRQ_PIN);
// 2 diff buffers with about 6K memory each
ILI9341_T4::DiffBuffStatic<6800> diff1;
ILI9341_T4::DiffBuffStatic<6800> diff2;

// our framebuffers
DMAMEM uint16_t internal_fb[LX * LY];
uint16_t fb[LX * LY];

// The canvas object that drawing functions operate on
tgx::Image<RGB565> canvas(fb, LX, LY);

void init() {
  Serial.println("Initializing serial communication to display...");
  tft.output(&Serial); // send debug info to serial port.

  while (!tft.begin(SPI_SPEED)) {
    Serial.println("Initialization error...");
    delay(1000);
  }

  Serial.println("Attaching internal framebuffer...");
  tft.setFramebuffers(internal_fb);

  Serial.println("Attaching differential buffers...");
  tft.setDiffBuffers(&diff1, &diff2);

  Serial.println("Setting diff gap to");
  tft.setDiffGap(10);

  Serial.println("Setting vsync spacing");
  tft.setVSyncSpacing(2);

  Serial.println("Setting refresh rate");
  tft.setRefreshRate(120);

  Serial.println("Setting screen rotation");
  tft.setRotation(3); // landscape

  Serial.println("Clearing screen");
  tft.clear(0);

  // delay(1);

  tft.update(fb);
}

long int lastRenderTime = 0;
long int currRenderTime = 0;
int renderDelta = 0;
bool fullUpdate = false;

/**
 * The update function
 */
void update() {
  tft.update(fb, fullUpdate);
  currRenderTime = millis();
  renderDelta = currRenderTime - lastRenderTime;
  if (renderDelta > FULL_UPDATE_PERIOD_MS) {
    fullUpdate = true;
  } else {
    fullUpdate = false;
  }
  lastRenderTime = currRenderTime;
}

void calibrateTouch() {
  int touch_calib[4] = {0, 0, 0, 0};

  tft.calibrateTouch(touch_calib);      // uncomment this line to run touch
  tft.setTouchCalibration(touch_calib); // set touch calibration
}

} // namespace mkshft_display

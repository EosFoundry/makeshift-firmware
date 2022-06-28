#define DEBUG

// std library
#include <queue>

// External libraries
#include <Arduino.h>
#include <TeensyID.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <functional>

// MakeShift libraries
#include <mkshft_core.hpp>
#include <mkshft_display.hpp>
#include <mkshft_ui.hpp>
#include <mkshft_led.hpp>

#include <mkshft_ctrl.hpp>

#ifdef DEBUG
#endif

// HID definitions
// #define VENDOR_ID               0x16BF
// #define PRODUCT_ID              0x047f
// #define RAWHID_USAGE_PAGE       0xFFAB  // recommended: 0xFF00 to 0xFFFF
// #define RAWHID_USAGE            0x0200  // recommended: 0x0100 to 0xFFFF

// #define RAWHID_RX_SIZE          64      // receive packet size
// #define RAWHID_RX_INTERVAL      8       // max # of ms between receive
// packets

// Hardware definitions

const long readInputPeriodUs =
    1000L; // microseconds between dial + button scanning cycle
const long visualRenderPeriodUs =
    26667L; // microseconds between updates to visual elements
uint8_t serialNumber[4];

/*
 * Packet counter to keep input and output on pace
 */
unsigned int packetCount = 0;

IntervalTimer readInputTimer;
IntervalTimer visualRenderTimer;

// State tracking
core::state_t stateCurr;
core::state_t statePrev;

// Loop-exclusive variables
uint8_t stateDelta = 0;
bool stateChanged = false;
uint8_t row, col;

Layout *baseLayout;
LoadingBar *testBar;

// volatile std::queue<dkEvent::Event> eventQueue;

// Helper functions
void visualUpdate();
void testWidgets();

void setup() {
  teensySN(serialNumber);
#ifdef DEBUG
  delay(1000);
#endif

#ifdef MKSHFT_CTRL_H_
  mkshft_ctrl::init(serialNumber);
  delay(1000);
#endif

#ifdef CORE_H_
  core::init();
  delay(1000);
#endif

#ifdef LED_H_
  mkshft_ledMatrix::init();
  delay(1000);
#endif

#ifdef ILI9341_H_
  mkshft_display::init();
  mkshft_ui::setDefaultCanvas(&canvas);
  delay(1000);
#endif

  readInputTimer.begin(core::updateState, readInputPeriodUs);
  visualRenderTimer.begin(visualUpdate, visualRenderPeriodUs);

  // testWidgets();

  // TODO - initialize data sizes for each module in memory

  baseLayout = new Layout();
  testBar = new LoadingBar(baseLayout);

  mkshft_ui::link(baseLayout, testBar);
  baseLayout->setColors(RGB32(130, 20, 144), RGB32(20, 20, 20));
  testBar->setBorderWidth(4);
  testBar->setFillColor(tgx::RGB32_Red);
  testBar->setBackgroundColor(tgx::RGB32_Black);
  // testBar->setBorderColor(tgx::RGB32_Black);
  baseLayout->render();
  delay(1000);
}

void loop() {
  // delay(10);
  statePrev = stateCurr;
  stateCurr = core::getState();

  // check button states
  for (int i = 0; i < core::szButtonArray; i++) {
    // Serial.print("Button ");
    // Serial.print(i);
    // Serial.print(" state check ");
    // Serial.print(mkshft_ledMatrix::ledMatrix[row][col].triggeredSeqIdx);
    // Serial.println();
    row = core::ButtonLookup[i][0];
    col = core::ButtonLookup[i][1];
    if (statePrev.button[i] != stateCurr.button[i]) {
      Pixel::edge_t edge;
      if (stateCurr.button[i] == core::ON) {
        edge = Pixel::RISE;
      } else {
        edge = Pixel::FALL;
      }
      mkshft_ledMatrix::ledMatrix[row][col].triggeredSeqIdx = edge;
      stateChanged = true;
    }
    // if (stateCurr.button[15] == true) {
    //   Serial.println("bye bye!");
    //   Serial.end();
    // }
  }
  // check dial states
  for (int i = 0; i < core::szDialArray; i++) {
    if (statePrev.dial[i] != stateCurr.dial[i]) {
      stateChanged = true;

      if (i == 1) { // update on just dial #2
        // multiply by 100 first to reduce scaling error
        float progressPercent = (float)stateCurr.dial[i] * 100.0f;

        // pull upper bound for now - negative dials become weirdness
        progressPercent =
            progressPercent / (float)core::dialBounds[core::MAX][i];

        int progress = round(progressPercent);

        testBar->setProgress(progress);
        baseLayout->render();
      }
    }
  }
  // send updates
  if (stateChanged == true) {
    mkshft_ctrl::sendState(stateCurr);
    // core::printStateToSerial(core::getState());
  }
  stateChanged = false;
  mkshft_display::update();
}

void visualUpdate() {
#ifdef LED_H_
  mkshft_ledMatrix::updateState();
  mkshft_ledMatrix::showMatrix();
#endif
}

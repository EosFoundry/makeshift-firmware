#define DEBUG

// std library
#include <queue>

// External libraries
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <TeensyID.h>

// MakeShift libraries
#include <core.hpp>
#include <led_matrix.hpp>
#include <ILI9341.hpp>
#include <mkshft_ctrl.hpp>

#ifdef DEBUG
#include <debug.hpp>
#endif

// HID definitions
// #define VENDOR_ID               0x16BF
// #define PRODUCT_ID              0x047f
// #define RAWHID_USAGE_PAGE       0xFFAB  // recommended: 0xFF00 to 0xFFFF
// #define RAWHID_USAGE            0x0200  // recommended: 0x0100 to 0xFFFF

// #define RAWHID_RX_SIZE          64      // receive packet size
// #define RAWHID_RX_INTERVAL      8       // max # of ms between receive packets

// Hardware definitions

const long readInputPeriodUs = 1000L; // microseconds between dial + button scanning cycle
const long visualRenderPeriodUs = 16667L; // microseconds between updates to visual elements
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

// volatile std::queue<dkEvent::Event> eventQueue;

// Helper functions
void render();


void setup()
{
  teensySN(serialNumber);
#ifdef DEBUG
  delay(1000);
#endif

#ifdef MKSHFT_CTRL_H_
  mkshft_ctrl::init(serialNumber);
#endif

  delay(1000);

#ifdef CORE_H_
  core::init();
#endif

#ifdef LED_H_
  mkshft_ledMatrix::init();
#endif

#ifdef ILI9341_H_
  mkshft_display::init();
#endif

  readInputTimer.begin(core::updateState, readInputPeriodUs);
  visualRenderTimer.begin(render, visualRenderPeriodUs);

  delay(100);

  mkshft_display::test();

  // TODO - initialize data sizes for each module in memory
}


void loop()
{
  // delay(10);
  statePrev = stateCurr;
  stateCurr = core::getState();

  // check button states
  for (int i = 0; i < core::szButtonArray; i++)
  {
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
  for (int i = 0; i < core::szDialArray; i++)
  {
    if (statePrev.dial[i] != stateCurr.dial[i])
    {
      stateChanged = true;
    }
  }
  // send updates
  if (stateChanged == true)
  {
    core::sendState();
    // core::printStateToSerial(core::getState());
  }
  stateChanged = false;
}

void render()
{
#ifdef LED_H_
  mkshft_ledMatrix::updateState();
  mkshft_ledMatrix::showMatrix();
#endif
}

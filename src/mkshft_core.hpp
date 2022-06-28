#ifndef CORE_H_
#define CORE_H_

#include <Arduino.h>
#include <Encoder.h>


inline namespace core {
const uint8_t deviceID = 0x02;
const uint8_t pinI2c_SDA = 18;
const uint8_t pinI2c_SCL = 19;

const uint8_t pinMatrixPoll_A = 2;
const uint8_t pinMatrixPoll_B = 3;
const uint8_t pinMatrixPoll_C = 4;
const uint8_t pinMatrixPoll_D = 5;

const uint8_t pinMatrixScan_0 = 23;
const uint8_t pinMatrixScan_1 = 22;
const uint8_t pinMatrixScan_2 = 21;
const uint8_t pinMatrixScan_3 = 20;

const uint8_t pinTouch_IRQ = 0;
const uint8_t pinTouch_CS = 1;
const uint8_t pinScreen_DC = 10;
const uint8_t pinScreen_CS = 9;
const uint8_t pinTouchScreen_SDI = 11;
const uint8_t pinTouchScreen_SDO = 12;
const uint8_t pinTouchScreen_CLK = 13;

const uint8_t pinEncoder_0_A = 18;
const uint8_t pinEncoder_0_B = 19;

const uint8_t pinEncoder_1_A = 16;
const uint8_t pinEncoder_1_B = 17;

const uint8_t pinEncoder_2_A = 14;
const uint8_t pinEncoder_2_B = 15;

const uint8_t pinEncoder_3_A = 6;
const uint8_t pinEncoder_3_B = 7;

const uint8_t szButtonArray = 16;
const uint8_t szDialArray = 4;
const uint8_t szMatrixPollArray = 4;
const uint8_t szMatrixScanArray = 4;

const uint8_t szSerialPorts = 1;

const uint8_t dialAddressModulo = 16;

typedef bool buttonState_t;
typedef int dialState_t;

enum input_t { serial, button, dial };

enum buttonEdge_t { ON = 1, OFF = 0 };

enum bound_t { MIN = 0, MAX = 1 };

const uint8_t ButtonLookup[szButtonArray][2] = {

    {0, 0}, {0, 1}, {0, 2}, {0, 3},

    {1, 0}, {1, 1}, {1, 2}, {1, 3},

    {2, 0}, {2, 1}, {2, 3}, {2, 2},

    {3, 0}, {3, 1}, {3, 2}, {3, 3}};

/**
 * This array maps the addresses betwee 0x00 and 0x14 to the buttons and
 * dials - 16 buttons + 4 dials = 20, or 0x14 in HEX
 *
 * This table encodes the addresses of the buttons and dials.
 */
const input_t
    inputAddressTypeTable[szButtonArray + szDialArray + szSerialPorts] = {
        input_t::button, input_t::button, input_t::button, input_t::button,

        input_t::button, input_t::button, input_t::button, input_t::button,

        input_t::button, input_t::button, input_t::button, input_t::button,

        input_t::button, input_t::button, input_t::button, input_t::button,

        input_t::dial,   input_t::dial,   input_t::dial,   input_t::dial,

        input_t::serial};

/**
 * This constant defines the scanning period in milliseconds.
 */
const long timer1IntervalMs = 4L;

const uint8_t pollPins[szMatrixPollArray] = {pinMatrixPoll_A, pinMatrixPoll_B,
                                             pinMatrixPoll_C, pinMatrixPoll_D};
const uint8_t scanPins[szMatrixScanArray] = {pinMatrixScan_0, pinMatrixScan_1,
                                             pinMatrixScan_2, pinMatrixScan_3};

/**
 * This struct defines a snapshot of the hardware state,
 */
struct state_t {
  bool button[szButtonArray];
  uint8_t buttonEdgeEventQueue[szButtonArray];
  uint16_t buttonExtended[szButtonArray];
  int dial[szDialArray];
  uint8_t dialRelative[szDialArray];
  uint32_t snapShotTime;
};

struct item_t {
  uint8_t *data;
  uint8_t size;
};

/**
 * These variables define the bounds for absolute dial measurement, they are
 * not constant to allow the front end to adjust absolute sensitivity.
 *
 * dialBounds[0][n] is the lower bound, dialBounds[1][n] is the upper.
 */
extern int dialBounds[2][szDialArray];

/**
 * The rollover setting will determine if the dial rolls from -31 -> 32 and
 * from 32 -> -31 or remain value locked at -31 or 32 when reaches the limit
 */
extern bool dialRollover[szDialArray];

/**
 * Boolean representation of button states, set after debouncing routine
 * verifies state.
 */
extern volatile bool buttonState[szButtonArray];

/**
 * retains the last edge registered by the button
 */
extern volatile uint8_t buttonEdgeEventQueue[szButtonArray];

/**
 * This array stores a 16-bit long running sample of button state in binary
 * for the purposes of a software debouncing algorithm.
 *
 * The most recent read is stored in LSB (Least Significant Bit) to match
 * the output from digitalReadFast(), and history is shifted left.
 *
 * On state is stored as 0, off is stored as 1 - this is done to simplify
 * the use of bitwise OR, gotta go fast.
 *
 * i.e. fully off button history would look like 0x1111111111111111
 *
 * see core::scanButtons() for details
 */
extern volatile uint16_t buttonExtendedState[szButtonArray];

/**
 * This array stores the absolute state of each dial as integer, on the
 * IMXRT1062, this is a 32bit int which should cover even the most precise
 * dials for a while yet.
 */
extern volatile int dialState[szDialArray];

/**
 * This array stores the relative state of each dial, obtained by taking a
 * delta as new dial state is recorded.
 */
extern volatile uint8_t dialStateRelative[szDialArray];

/**
 *
 */
extern Encoder *dials;

/**
 * Loops over keyswitch matrix, debouncing and updating button state along
 * the way. The debounce code ensures a minimum time for a button to be held
 * down in order to be 'certified on'.
 *
 * See link below for overview of how a switch matrix works:
 * https://www.baldengineer.com/arduino-keyboard-matrix-tutorial.html
 *
 */
void scanButtons();

/**
 * This function reads the dials and updates the absolute dial states
 *
 */
void readDials();

/**
 * Updates state by calling hardware read functions, no logic is put in here
 * to keep the update process as fast as possible, logic updates are read and
 * then acted on in the main.cpp loop
 */
void updateState();

state_t getState();

void init();

int getBound(bound_t, int inputAddress, int *boundValue);

// Debugging support functions
void printStateToSerial(state_t states, bool ext);
void printStateToSerial(state_t states);

#ifdef DEBUG
void corePrint(char *msg);
void corePrintln(char *msg);
void test();
#endif

} // namespace core

#endif // CORE_H_

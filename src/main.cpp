#define DEBUG 1
// std library
#include <queue>

// External libraries
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// MakeShift libraries
#include <core.hpp>
#include <led.hpp>
#include <dkEvent.hpp>

#ifdef DEBUG
void printButtonState(core::state_t states, byte buttonAddress)
{
  Serial.print("Button ");
  Serial.print(buttonAddress);
  Serial.print(": ");
  Serial.print(states.button[buttonAddress]);
  Serial.print(" | ");
  Serial.print(states.buttonExtended[buttonAddress], BIN);
}

void printlnButtonState(core::state_t states, byte buttonAddress)
{
  printButtonState(states, buttonAddress);
  Serial.println();
}

void printDialState(core::state_t states, byte dialAddress)
{
  Serial.print("Dial ");
  Serial.print(dialAddress);
  Serial.print(": ");
  Serial.print(states.dial[dialAddress]);
}

void printlnDialState(core::state_t states, byte dialAddress)
{
  printDialState(states, dialAddress);
  Serial.println();
}

void printStateToSerial(core::state_t states)
{
  Serial.print("states: ");
  for (int i = 0; i < 16; i++)
  {
    Serial.print(states.button[i]);
    Serial.print(' ');
  }
  Serial.print("| ");

  // Serial.print("ext: ");
  // for (int i = 0; i < 16; i++)
  // {
  //   Serial.print(states.buttonExtended[i], BIN);
  //   Serial.print(' ');
  // }
  // Serial.print("| ");

  Serial.print("dials: ");
  for (int i = 0; i < core::szDialArray; i++)
  {
    Serial.print(states.dial[i]);
    Serial.print(' ');
  }
  Serial.println();
}

byte printBuffer64(byte *buffer)
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      int count = (i * 8) + j;
      Serial.print(buffer[count]);
      Serial.print(' ');
    }
    Serial.println();
  }
  return 0;
}
#endif

// HID definitions
// #define VENDOR_ID               0x16BF
// #define PRODUCT_ID              0x047f
// #define RAWHID_USAGE_PAGE       0xFFAB  // recommended: 0xFF00 to 0xFFFF
// #define RAWHID_USAGE            0x0200  // recommended: 0x0100 to 0xFFFF

// #define RAWHID_RX_SIZE          64      // receive packet size
// #define RAWHID_RX_INTERVAL      8       // max # of ms between receive packets

// Hardware definitions

const char *boardName = "Toaster";

/*
 * Packet counter to keep input and output on pace
 */
unsigned int packetCount = 0;

IntervalTimer hwUpdateTimer;
IntervalTimer ledUpdateTimer;

/**
 * This constant defines the scanning period in milliseconds.
 */
const long timer1PeriodUs = 4L;

core::state_t stateCurr;
core::state_t statePrev;

int attachedDevices = 0;

// volatile std::queue<dkEvent::Event> eventQueue;

void updateState()
{
#ifdef CORE_H_
  core::updateState();
#endif

#ifdef LED_H_
  mkshft_led::updateState();
#endif
}

void sendItem(core::item_t item)
{
  // for (int j = 0; j < item.size; j++)
  // {
  //   Serial.print(item.data[j],BIN);
  // }
  Serial.write(item.data, item.size);
  Serial.print("\n");
}

void setup()
{

#ifdef CORE_H_
  core::init();
#endif
#ifdef LED_H_
  mkshft_led::init();
#endif

  hwUpdateTimer.begin(updateState, timer1PeriodUs * 1000);

  delay(100);

  // TODO - initialize data sizes for each module in memory
}

uint8_t stateDelta = 0;
bool stateChanged = false;

void loop()
{
  statePrev = stateCurr;
  stateCurr = core::getState();

  // check button states
  for (int i = 0; i < core::szButtonArray; i++)
  {
    if (statePrev.button[i] != stateCurr.button[i])
    {
      stateChanged = true;
    }
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
    // sendItem(
    //  core::generateItem(stateDelta, i)
    // );
    printStateToSerial(stateCurr);
  }
  stateChanged = false;
}

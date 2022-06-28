#ifndef MKSHFT_CTRL_H_
#define MKSHFT_CTRL_H_

#include <Arduino.h>
#include <PacketSerial.h>
#include <color.hpp>
#include <functional>
#include <map>
#include <mkshft_core.hpp>
#include <mkshft_display.hpp>
#include <mkshft_led.hpp>
#include <mkshft_ui.hpp>
#include <vector>

inline namespace mkshft_ctrl {

typedef std::string DataLabel;

enum DataType {
  STRING,
  INT,
  FLOAT,
};

struct DataValue {
  DataType type;
  std::vector<uint8_t> value;
};

typedef std::function<bool(DataLabel, DataValue)> MkshftCtrlFn;

// extern std::map<DataLabel, MkshftCtrlFn> functionStore;

/**
 * This data structure stores 'arbitrary' data as a typestring
 */
extern std::map<DataLabel, DataValue> dataStore;

extern SLIPPacketSerial packetSerial;

enum MessageType { INIT = 0, STATE_UPDATE = 1, CALL_ERROR };

bool getWidgets();

std::vector<DataLabel> getDataLabels();

// bool setNumberData(DataLabel label, int newInt);

// bool setNumberData(DataLabel label, float newFloat);

// bool toggleOverlay();


bool setTextData(DataLabel label, DataValue newText);

void sendState(core::state_t st);

void init(uint8_t const serial[4]);

void send(const uint8_t *buf, size_t sz);

void onPacketReceived(const uint8_t *buffer, size_t size);

} // namespace mkshft_ctrl

#endif // MKSHFT_CTRL_H_

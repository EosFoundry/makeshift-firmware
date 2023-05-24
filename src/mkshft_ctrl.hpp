#ifndef MKSHFT_CTRL_H_
#define MKSHFT_CTRL_H_

#include <Arduino.h>
#include <PacketSerial.h>
#include <TeensyID.h>

#include <map>
#include <string>
#include <vector>

#include <makethift.hpp>
#include <mkshft_core.hpp>

inline namespace mkshft_ctrl {

extern SLIPPacketSerial packetSerial;

enum MessageType {
  PING,
  ACK,
  READY,
  STATE_UPDATE,
  ERROR,
  STRING,
  DISCONNECT,
};

extern uint8_t connected;

bool getWidgets();

void sendState(core::state_t);
void sendLayouts();

void init();

// Alias to keep the implementation detail out of the way
inline void update() { packetSerial.update(); };

void sendReady();

void sendString(std::string);
void sendLine(std::string);

// wraps PacketSerial.send with a connection check
void send(MessageType, const uint8_t *, size_t);
void sendRaw(const uint8_t *, size_t);

void onPacketReceived(const uint8_t *, size_t);
void handleSymExp(std::string);

} // namespace mkshft_ctrl

#endif // MKSHFT_CTRL_H_

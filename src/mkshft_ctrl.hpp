#ifndef MKSHFT_CTRL_H_
#define MKSHFT_CTRL_H_

#include <Arduino.h>
#include <PacketSerial.h>

#include <map>
#include <string>
#include <vector>

#include <makethift.hpp>

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

void sendState(core::state_t st);
void sendLayouts();

void init(uint8_t const serial[4]);

// Alias to keep the implementation detail out of the way
inline void update() { packetSerial.update(); };

void sendReady();

void sendString(std::string body);

// wraps PacketSerial.send with a connection check
void send(MessageType, const uint8_t *, size_t);

void onPacketReceived(const uint8_t *buffer, size_t size);
void handleSymExp(std::string);

} // namespace mkshft_ctrl

#endif // MKSHFT_CTRL_H_

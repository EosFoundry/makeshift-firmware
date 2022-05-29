#ifndef MKSHFT_CTRL_H_
#define MKSHFT_CTRL_H_

#include <Arduino.h>
#include <PacketSerial.h>

#include <message.hpp>

namespace mkshft_ctrl {

extern SLIPPacketSerial packetSerial;
enum MessageType {
  INIT = 0,
  STATE_UPDATE = 1
};

void send(const uint8_t *buf, size_t sz);

void onPacketReceived(const uint8_t* buffer, size_t size);

void init(const uint8_t serial[4]);
} // namespace mkshft_ctrl

#endif // MKSHFT_CTRL_H_

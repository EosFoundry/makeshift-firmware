#ifndef MKSHFT_CTRL_H_
#define MKSHFT_CTRL_H_

#include <Arduino.h>
#include <PacketSerial.h>

#include <message.hpp>

namespace mkshft_ctrl {

extern SLIPPacketSerial packetSerial;

void send(const uint8_t *buf, size_t sz);

void onPacketReceived(const uint8_t* buffer, size_t size);

void init();
} // namespace mkshft_ctrl

#endif // MKSHFT_CTRL_H_

#include <mkshft_ctrl.hpp>

namespace mkshft_ctrl {

SLIPPacketSerial packetSerial;

void init(uint8_t serial[4]) {
  Serial.begin(42069);
  uint8_t initMessage[8];

  initMessage[0] = MessageType::INIT;

  packetSerial.setStream(&Serial);
  packetSerial.setPacketHandler(&onPacketReceived);
}

void send(const uint8_t *buf, size_t sz) { packetSerial.send(buf, sz); }

void onPacketReceived(const uint8_t *buffer, size_t size) {
  packetSerial.send(buffer, size);
}
} // namespace mkshft_ctrl

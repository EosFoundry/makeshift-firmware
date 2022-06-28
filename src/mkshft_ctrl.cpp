#include <mkshft_ctrl.hpp>

inline namespace mkshft_ctrl {

SLIPPacketSerial packetSerial;

// bool setNumberData(DataLabel label, int newInt) {}

bool setTextData(DataLabel label, std::string newText) {
  auto tempData = dataStore.find(label);

  if (tempData == dataStore.end()) {
    DataValue newData;
    newData.type = STRING;
    newData.value.resize(newText.size());

    for (int n = 0; n < newText.size(); n++) {
      newData.value[n] = newText[n];
    }
    
    dataStore.emplace(label, newData);
  } else {
    
  }

  return true;
}

void sendState(core::state_t st) {
  int sz = 20;
  uint8_t buffer[sz];

  for (int n = 0; n < sz; n++) {
    buffer[n] = 0;
  }
  buffer[0] = MessageType::STATE_UPDATE;
  // Fill least significant byte with button states 0 -> 7
  buffer[2] += st.button[0] * 0x01;
  buffer[2] += st.button[1] * 0x02;
  buffer[2] += st.button[2] * 0x04;
  buffer[2] += st.button[3] * 0x08;
  buffer[2] += st.button[4] * 0x10;
  buffer[2] += st.button[5] * 0x20;
  buffer[2] += st.button[6] * 0x40;
  buffer[2] += st.button[7] * 0x80;

  // Fill most significant byte with button states 8 -> 15
  buffer[1] += st.button[8] * 0x01;
  buffer[1] += st.button[9] * 0x02;
  buffer[1] += st.button[10] * 0x04;
  buffer[1] += st.button[11] * 0x08;
  buffer[1] += st.button[12] * 0x10;
  buffer[1] += st.button[13] * 0x20;
  buffer[1] += st.button[14] * 0x40;
  buffer[1] += st.button[15] * 0x80;

  // use bitwise AND (&) to break the integer dial state into 4 bytes
  uint8_t i = 0;
  for (uint8_t n = 0; n < 4; n++) {
    i = (n * 4) + 3;
    buffer[i] += (st.dial[n] & 0xFF000000) >> 24;
    buffer[i + 1] += (st.dial[n] & 0x00FF0000) >> 16;
    buffer[i + 2] += (st.dial[n] & 0x0000FF00) >> 8;
    buffer[i + 3] += (st.dial[n] & 0x000000FF);
  }

  // send endline as last byte
  buffer[19] = '\n';

  send(buffer, sz);
}

void init(uint8_t const serial[4]) {
  Serial.begin(115200);
  uint8_t initMessage[8];

  initMessage[0] = MessageType::INIT;

  packetSerial.setStream(&Serial);
  packetSerial.setPacketHandler(&onPacketReceived);
}

void send(const uint8_t *buf, size_t sz) { packetSerial.send(buf, sz); }

void onPacketReceived(const uint8_t *buffer, size_t size) {
  packetSerial.send(buffer, size);
  // Serial.write(buffer, size);
  // Serial.println();
}
} // namespace mkshft_ctrl

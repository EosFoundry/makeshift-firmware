#include <mkshft_ctrl.hpp>

inline namespace mkshft_ctrl {
uint8_t UID[8];
SLIPPacketSerial packetSerial;
uint8_t connected = 0;

void sendState(core::state_t st) {
  int sz = 23;
  uint8_t buffer[sz];

  for (int n = 0; n < sz; n++) {
    buffer[n] = 0;
  }
  // Fill least significant byte with button states 0 -> 7
  buffer[1] += st.button[0] * 0x01;
  buffer[1] += st.button[1] * 0x02;
  buffer[1] += st.button[2] * 0x04;
  buffer[1] += st.button[3] * 0x08;
  buffer[1] += st.button[4] * 0x10;
  buffer[1] += st.button[5] * 0x20;
  buffer[1] += st.button[6] * 0x40;
  buffer[1] += st.button[7] * 0x80;

  // Fill most significant byte with button states 8 -> 15
  buffer[0] += st.button[8] * 0x01;
  buffer[0] += st.button[9] * 0x02;
  buffer[0] += st.button[10] * 0x04;
  buffer[0] += st.button[11] * 0x08;
  buffer[0] += st.button[12] * 0x10;
  buffer[0] += st.button[13] * 0x20;
  buffer[0] += st.button[14] * 0x40;
  buffer[0] += st.button[15] * 0x80;

  // use bitwise AND (&) to break the integer dial state into 4 bytes
  uint8_t i = 0;
  for (uint8_t n = 0; n < 4; n++) {
    i = (n * 4) + 6;
    buffer[n + 2] = st.dialRelative[n];
    buffer[i] += (st.dial[n] & 0xFF000000) >> 24;
    buffer[i + 1] += (st.dial[n] & 0x00FF0000) >> 16;
    buffer[i + 2] += (st.dial[n] & 0x0000FF00) >> 8;
    buffer[i + 3] += (st.dial[n] & 0x000000FF);
  }

  // send endline as last byte
  buffer[sz - 1] = '\n';

  send(STATE_UPDATE, buffer, sz);
}

void init() {
  Serial.begin(115200);
  // uint8_t initMessage[8];

  // initMessage[0] = MessageType::READY;
  

  packetSerial.setStream(&Serial);
  packetSerial.setPacketHandler(&onPacketReceived);
}

// sends a ready signal through serial that bypasses the connection check
void sendReady() {
  std::string body = teensyUID64();
  send(READY, (uint8_t *)body.data(), body.size());
}

void sendString(std::string body) {
  send(STRING, (uint8_t *)body.data(), body.size());
}

void sendLine(std::string body) {
  sendString(body + "\n");
}

void sendByte(MessageType t) {
  uint8_t buf[1];
  buf[0] = (uint8_t)t;

  sendRaw(buf, 1);
}

void send(MessageType t, const uint8_t *body, size_t sz) {
  int size = sz + 1;
  uint8_t buf[size];

  buf[0] = (uint8_t)t;
  for (uint32_t idx = 0; idx < sz; idx++) {
    buf[idx + 1] = body[idx];
  }

  sendRaw(buf, size);
}

void sendRaw(const uint8_t *body, size_t size) {
  if(connected){
    packetSerial.send(body, size);
  } else {
    Serial.write(body, size);
  }
}

void onPacketReceived(const uint8_t *buffer, size_t size) {
  MessageType header = (MessageType)buffer[0];

#ifdef DEBUG
  // // start debug message
  // std::string msg = "Got packet: ";
  // send(STRING, (uint8_t *)msg.data(), msg.length());
  // send(STRING, buffer, size);
#endif

  switch (header) {
  case PING: {
    sendByte(ACK);
    break;
  }
  case STRING: {
    // convert buffer to string
    std::string exp;
    exp.assign((char *)buffer, size);
    // push the string back from header
    exp = exp.substr(1);
    handleSymExp(exp);
    break;
  }
  case ACK:
    break;
  case ERROR:
    break;
  case READY:
    connected = true;
    sendReady();
    break;
  case DISCONNECT:
    connected = false;
    break;
  default:
    break;
  }
}

void handleSymExp(std::string exp) {
#ifdef DEBUG
  // // start debug message
  // std::string msg;

  // int sz = exp.size();
  // int len = 2;
  // while (sz > 10) {
  //   sz = sz / 10;
  //   ++len;
  // }

  // // do some size shenanigans because std::to_string doesn't exist
  // char buf[len];
  // snprintf(buf, len, "%u", exp.size());
  // msg += buf;
#endif

  auto tokens = mkshft_lisp::tokenize(exp);

  SymExp res = mkshft_lisp::parseTokens(tokens);
  if (res.type != SexpType::ERROR) {
    log("Parsing successful");
    auto symRes = toSym(res);
    log(symRes);
  }

#ifdef DEBUG
  // msg = "tokenized exp";
  // sendString(msg);

  // std::string tkn;
  // for (auto t : tokens) {
  //   tkn = "TokenType: ";
  //   switch (t.type) {
  //   case PAR:
  //     tkn += "PAR";
  //     break;
  //   case SPC:
  //     tkn += "SPC";
  //     break;
  //   case SYM:
  //     tkn += "SYM";
  //     break;
  //   case NUM:
  //     tkn += "NUM";
  //     break;
  //   default:
  //     tkn += "UNDEF";
  //     break;
  //   }
  //   tkn += " | data: ";
  //   tkn.append(t.value);
  //   sendString(tkn);
  // }
#endif


  // Serial.write(buffer, size);
  // Serial.println();
}
} // namespace mkshft_ctrl

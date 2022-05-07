#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <Arduino.h>

class Mesasge {
public:
  enum Header {
    init = 0,
    stateUpdate = 1,
    cmdReponse = 2,
    empty = -1
  };

private:
  Header header;
  uint8_t data[256];
  uint8_t *item;
  int size = 256;

public:
  Header getHeader() { return header; }
  int getSize() { return size; }
};

const uint8_t itemOffset = 2;

#endif // MESSAGE_H_

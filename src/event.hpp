#ifndef EVENT_H_
#define EVENT_H_

#include <Arduino.h>

template <class E> class Event {
  Event(uint8_t inputID) {}

public:
  E state;
};

#endif
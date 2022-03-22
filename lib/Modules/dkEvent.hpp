#ifndef DKEVENT_H_
#define DKEVENT_H_

#include <Arduino.h>
#include <queue>

namespace donkey
{
    const byte itemOffset = 2;
    enum class eventClass_t
    {
        hwInput = 0,
        cmdReponse = 1,
        empty = -1
    };

    struct event_t
    {
        uint8_t header;
        uint8_t *data;
        uint8_t *item;
        uint8_t size;
    };
}
#endif

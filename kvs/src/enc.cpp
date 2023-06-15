#include "enc.hpp"

#include <stddef.h>
#include "Arduino.h"

void challengeReply(const EncMessage* input, EncMessage* output) {
    for (size_t i = 0; i < sizeof(EncMessage::data); i++) {
        Serial.println(input->data[sizeof(EncMessage::data) - i - 1], 16);
        output->data[i] = input->data[sizeof(EncMessage::data) - i - 1];
    }
}
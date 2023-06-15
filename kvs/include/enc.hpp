#ifndef __ENC_H__
#define __ENC_H__

#include <inttypes.h>

struct EncMessage {
    uint8_t data[16];
};

extern void challengeReply(const EncMessage* input, EncMessage* output);

#endif  // __ENC_H__
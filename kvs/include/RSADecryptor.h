#ifndef __RSADECRYPTOR_H__
#define __RSADECRYPTOR_H__

#include <cstdint>

class RSADecryptor {
    static const int BITS = 128;

public:
    RSADecryptor();
    void Decrypt(const uint8_t* in /*size=BITS*/, uint8_t* out /*size=BITS*/);

private:
    uint8_t temp_[2*BITS];
    uint8_t base_[BITS];
    uint8_t mod_[BITS];

    void mult_modulo(uint8_t* a /* size=BITS */, const uint8_t* b /* size=BITS */);
};

#endif // __RSADECRYPTOR_H__
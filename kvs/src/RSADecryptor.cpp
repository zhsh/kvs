#include <string>
#include "RSADecryptor.h"

// Keep these numbers secret!!!
//
// p = 10715396759774723011
// q = 17019186121003214119
// d = 63235998222315526624885483292362654973
// d_bits = 00101111100100101100111101101100110011011010100100100000010111100001011110100001110101000101111000100011010100011100010011111101
// d_bits_inverse = 10111111001000111000101011000100011110100010101110000101111010000111101000000100100101011011001100110110111100110100100111110100
// n = p*q = 182367331815000777515748660703949392309
// n_bits = 10001001001100101010101110100111010101010100100110011000011011001110110110000000001110000110100101011101111111101000100110110101

const int BITS = 128;

void mult(const uint8_t* n1 /* size=BITS */, const uint8_t* n2 /* size=BITS */, uint8_t* out /* size=2*BITS */) {
    for (int i = 0; i < 2*BITS; ++i) {
        out[i] = 0;
    }
    for (int b = 0; b < BITS; ++b) {
        if (n2[BITS-1-b] == 0) {
            continue;
        }
        int carry = 0;
        for (int i = 0; i < BITS; ++i) {
            int new_val = out[2*BITS-1-b-i] + n1[BITS-1-i] + carry;
            out[2*BITS-1-b-i] = new_val % 2;
            carry = new_val / 2;
        }
        out[BITS-1-b] = carry;
    }
}

bool greaterOrEqual(const uint8_t* n1, const uint8_t* n2, int size) {
    for (int i = 0; i < size; ++i) {
        if (n1[i] > n2[i]) {
            return true;
        }
        if (n1[i] < n2[i]) {
            return false;
        }
    }
    return true;
}

void subtract_in_place(uint8_t* n1, int n1Size, const uint8_t* n2, int n2Size) {
    int carry = 0;
    for (int i = 0; i < n2Size; ++i) {
        int newVal = n1[n1Size-1-i] - n2[n2Size-1-i] - carry;
        if (newVal < 0) {
            carry = 1;
            n1[n1Size-1-i] = newVal + 2;
        } else {
            carry = 0;
            n1[n1Size-1-i] = newVal;
        }
    }
    if (carry != 0) {
        n1[n1Size-1-n2Size] -= carry;
    }
}

void modulo_in_place(uint8_t* data /* size=2*BITS */, const uint8_t* mod /* size=BITS */) {
    int modStart = -1;
    for (int i = 0; i < BITS; ++i) {
        if (mod[i] == 1) {
            modStart = i;
            break;
        }
    }
    int modSize = BITS - modStart;

    for (int startPos = 0; startPos < 2*BITS; ++startPos) {
        if (data[startPos] == 0) {
            continue;
        }
        if (startPos + modSize > 2*BITS) {
            break;
        }
        if (greaterOrEqual(data + startPos, mod + modStart, modSize)) {
            subtract_in_place(data + startPos, modSize, mod + modStart, modSize);
        } else if (startPos + modSize + 1 <= 2*BITS) {
            subtract_in_place(data + startPos, modSize + 1, mod + modStart, modSize);
        }
    }
}

RSADecryptor::RSADecryptor() {
    std::string n_bits = "10001001001100101010101110100111010101010100100110011000011011001110110110000000001110000110100101011101111111101000100110110101";
    for (int i = 0; i < BITS; ++i) {
        mod_[i] = (n_bits[i] == '1' ? 1 : 0);
    }
}

void RSADecryptor::Decrypt(const uint8_t *in /* size=BITS */, uint8_t *out /* size=BITS */) {
    // Keep this string secret!
    std::string d_bits_inverse = "10111111001000111000101011000100011110100010101110000101111010000111101000000100100101011011001100110110111100110100100111110100";
    for (int i = 0; i < BITS; ++i) {
        base_[i] = in[i];
    }
    for (int i = 0; i < BITS; ++i) {
        out[i] = 0;
    }
    out[BITS-1] = 1;

    for (unsigned int i = 0; i < d_bits_inverse.length(); ++i) {
        if (d_bits_inverse[i] == '1') {
            mult_modulo(out, base_);
        }
        mult_modulo(base_, base_);
    }
}

void RSADecryptor::mult_modulo(uint8_t* a, const uint8_t* b) {
    mult(a, b, temp_);
    modulo_in_place(temp_, mod_);
    for (int i = 0; i < BITS; ++i) {
        a[i] = temp_[i+BITS];
    }
}

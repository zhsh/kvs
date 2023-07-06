#ifndef __BIGNUM_H__
#define __BIGNUM_H__

#include <stdint.h>
#include <string>
#include <vector>

struct BigInt {
  BigInt() = default;
  static BigInt bin(const std::string& str);
  static BigInt hex(const std::string& str);
  static BigInt fromUint32(uint32_t v);

  std::vector<uint32_t> digits;

  void print() const;
  BigInt operator*(const BigInt& other) const;
  BigInt operator+(const BigInt& other) const;
  BigInt operator-(const BigInt& other) const;
  bool operator==(const BigInt& other) const;
  bool operator!=(const BigInt& other) const;
  bool operator<(const BigInt& other) const;
  bool operator>(const BigInt& other) const;
  bool operator<=(const BigInt& other) const;
  bool operator>=(const BigInt& other) const;
  BigInt operator%(const BigInt& other) const;
  BigInt& operator+=(const BigInt& other);
  BigInt& operator-=(const BigInt& other);
  BigInt& operator%=(const BigInt& other);
  BigInt operator <<(uint32_t num) const;
  BigInt shift(uint32_t num) const;

  // Most significant bit
  uint32_t msb() const;
};

void pow(const BigInt& in, const BigInt& d, const BigInt& mod, BigInt& out);

#endif  // __BIGNUM_H__
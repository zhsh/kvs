#include <stdio.h>
#include <cassert>
#include <vector>
#include <stdint.h>
#include <string>
#include <string_view>


struct BigInt {
  BigInt() = default;
  static BigInt hex(const std::string_view str);

  std::vector<uint32_t> digits;

  void print() const;
  BigInt operator*(const BigInt& other) const;
  BigInt operator+(const BigInt& other) const;
  BigInt operator-(const BigInt& other) const;
  bool operator==(const BigInt& other) const;
  bool operator<(const BigInt& other) const;
  bool operator>(const BigInt& other) const;
  BigInt operator%(const BigInt& other) const;
  BigInt shift(uint32_t num) const;

  // Most significant bit
  uint32_t msb() const;
};

BigInt BigInt::hex(const std::string_view str) {
  BigInt res;
  for (int i = str.size(); i >= 0; i-=8) {

    uint32_t v = 0;
    for (int j = std::max(i - 8, 0); j < i; j++) {
      char c = str[j];
      int digit;
      if (c >= '0' && c <= '9') digit = c - '0';
      else if (c >= 'a' && c <= 'f') digit = c - 'a' + 10;
      else if (c >= 'A' && c <= 'F') digit = c - 'A' + 10;
      else {
        std::string copy = std::string(str);
        printf("Wrong char %c in %.*s\n", c, (int)str.length(), str.data());
        abort();
      }
      v = (v << 4) + digit;
    }
    res.digits.push_back(v);
  }
  return res;
}

void BigInt::print() const {
    printf("\"");
    bool first = true;
    for (int i = digits.size() - 1; i >= 0; i--) {
      if (first && digits[i] == 0) continue;
      printf(first ? "%x" : "%08x", digits[i]);
      first = false;
    }
    printf("\"\n");
}

BigInt mul(const BigInt& a, const BigInt& b) {
  BigInt result;
  result.digits.resize(a.digits.size() + b.digits.size());

  for (int i = 0; i < a.digits.size(); i++) {
    uint32_t carry = 0;
    for (int j = 0; j < b.digits.size(); j++) {
      uint32_t m1 = a.digits[i];
      uint32_t m2 = b.digits[j];
      uint64_t product = (uint64_t)(m1) * m2 + carry + result.digits[i + j];
      result.digits[i + j] = product;
      carry = product >> 32;
    }
    result.digits[i + b.digits.size()] = carry;
  }
  while (result.digits.back() == 0) result.digits.pop_back();
  return result;
}

BigInt BigInt::operator*(const BigInt& other) const {
  return mul(*this, other);
}

int cmp(const BigInt& a, const BigInt& b) {
  BigInt result;
  int len_min = std::min(a.digits.size(), b.digits.size());
  for (int i = len_min; i < a.digits.size(); i++) {
    if (a.digits[i] != 0) return 1;
  }
  for (int i = len_min; i < b.digits.size(); i++) {
    if (b.digits[i] != 0) return -1;
  }
  for (int i = len_min - 1; i >= 0; --i) {
    if (a.digits[i] != b.digits[i]) return a.digits[i] < b.digits[i] ? -1 : 1;
  }
  return 0;
}

bool BigInt::operator==(const BigInt& other) const {
  return cmp(*this, other) == 0;
}

bool BigInt::operator<(const BigInt& other) const {
  return cmp(*this, other) == -1;
}

bool BigInt::operator>(const BigInt& other) const {
  return cmp(*this, other) == 1;
}

BigInt add(const BigInt& a, const BigInt& b) {
  BigInt result;
  result.digits.resize(std::max(a.digits.size(), b.digits.size()));

  uint32_t carry = 0;
  for (int i = 0; i < result.digits.size(); i++) {
    uint32_t v1 = i < a.digits.size() ? a.digits[i] : 0;
    uint32_t v2 = i < b.digits.size() ? b.digits[i] : 0;
    uint64_t sum = (uint64_t)(v1) + v2 + carry;
    result.digits[i] = sum;
    carry = sum >> 32;
  }
  if (carry) result.digits.push_back(carry);
  return result;
}

BigInt BigInt::operator+(const BigInt& other) const {
  return add(*this, other);
}

BigInt sub(const BigInt& a, const BigInt& b) {
  BigInt result;
  result.digits.resize(std::max(a.digits.size(), b.digits.size()));

  // Sign extended carry
  int64_t carry = 0;

  for (int i = 0; i < result.digits.size(); i++) {
    uint32_t v1 = a.digits[i];
    uint32_t v2 = i < b.digits.size() ? b.digits[i] : 0;
    uint64_t sum = (uint64_t)(v1) - v2 + carry;
    result.digits[i] = sum;
    carry = sum;
    // Sign extend carry;
    carry >>= 32;
  }
  assert(carry == 0);
  while (result.digits.back() == 0) result.digits.pop_back();
  return result;
}

BigInt BigInt::operator-(const BigInt& other) const {
  return sub(*this, other);
}

BigInt shift(const BigInt& in, uint32_t num) {
  uint32_t full = num / 32;
  uint32_t partial = num % 32;
  uint32_t opposite = 32 - partial;
  BigInt res;
  res.digits.resize(full + 1 + in.digits.size());
  uint32_t carry = 0;
  for (size_t i = 0; i < in.digits.size(); i++) {
    uint32_t v = in.digits[i];
    res.digits[full + i] = (v << partial) | carry;
    carry = v >> opposite;
  }
  while (res.digits.back() == 0) res.digits.pop_back();
  return res;
}

BigInt BigInt::shift(uint32_t num) const {
  return ::shift(*this, num);
}

uint32_t msb(const BigInt& v) {
  for (int i = v.digits.size() - 1; i >= 0; i--) {
    uint32_t mask = 0x80000000;
    uint32_t pos = 31;
    do {
      if (v.digits[i] & mask) {
        return i * 32 + pos;
      }
      mask >>= 1;
      pos--;
    } while (mask != 0);
  }
  return 0;
}

uint32_t BigInt::msb() const {
  return ::msb(*this);
}


BigInt modulo(const BigInt& a, const BigInt& b) {
  BigInt res = a;
  int a_msb = a.msb();
  int b_msb = b.msb();
  int max_shift = a_msb - b_msb;

  for (int shift = max_shift; shift >= 0; shift--) {
    auto bshifted = b.shift(shift);

    if (res > bshifted) {
      res = res - bshifted;
    }
  }
  return res;
}

BigInt BigInt::operator%(const BigInt& other) const {
  return modulo(*this, other);
}

int main(int argc, char **argv) {
  {
    auto a = BigInt::hex("5cb61254faaeb95cb4911034303cebdd");
    auto b = BigInt::hex("a824017a138a875f8ddd9134e5e76");
    assert(a > b);
    assert(a * b == BigInt::hex("3ce4862b4460aa39dc8c87750736a11256e2a135dc12948a5912adf05ddde"));
    assert(a + b == BigInt::hex("5cc09495124ff2052a89ee0d438b4a53"));
    assert(a - b == BigInt::hex("5cab9014e30d80b43e98325b1cee8d67"));
    assert(a % b == BigInt::hex("539048ea8fb7c277c4b87e6ffbf11"));
  }

  {
    BigInt v0 = BigInt::hex("222FFFFFFFFFFFFFFFFFFFFFFF0000000000000");
    BigInt v1 = BigInt::hex("011000000000000000000000010000000000000");
    assert(v0 + v1 == BigInt::hex("234000000000000000000000000000000000000"));
    assert((v0 + v1) - v1 == v0);
  }

  {
    auto v0 = BigInt::hex("c00000000000000000000");
    auto v1 = BigInt::hex("8");
    assert(v0 + v1 > v0);
    assert(v0 - v1 == BigInt::hex("bfffffffffffffffffff8"));
    assert((v0 - v1) + v1 + v1 == BigInt::hex("c00000000000000000008"));
  }
  printf("Tests passed\n");
}

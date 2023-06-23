#include <stdio.h>
#include <cassert>
#include <vector>
#include <stdint.h>
#include <string>


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

BigInt BigInt::fromUint32(uint32_t v) {
  BigInt res;
  res.digits.push_back(v);
  return res;
}

void strip_leading_zeros(BigInt& a) {
  while (a.digits.size() > 1 && a.digits.back() == 0) a.digits.pop_back();
}

BigInt BigInt::bin(const std::string& str) {
  BigInt res;
  for (int i = str.size(); i >= 0; i-=32) {

    uint32_t v = 0;
    for (int j = std::max(i - 32, 0); j < i; j++) {
      char c = str[j];
      int digit;
      if (c == '0' ) digit = 0;
      else if (c == '1') digit = 1;
      else {
        std::string copy = std::string(str);
        printf("Wrong char %c in %.*s\n", c, (int)str.length(), str.data());
        abort();
      }
      v = (v << 1) + digit;
    }
    res.digits.push_back(v);
  }
  if (res.digits.back() == 0 && res.digits.size() > 1) res.digits.pop_back();
  return res;
}

BigInt BigInt::hex(const std::string& str) {
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
  if (res.digits.back() == 0 && res.digits.size() > 1) res.digits.pop_back();
  return res;
}

void BigInt::print() const {
    printf("\"");
    bool first = true;
    for (ssize_t i = digits.size() - 1; i >= 0; i--) {
      if (first && digits[i] == 0) continue;
      printf(first ? "%x" : "%08x", digits[i]);
      first = false;
    }
    printf("\"\n");
}

void mul(const BigInt& a, const BigInt& b, BigInt& result) {
  result.digits.clear();
  result.digits.resize(a.digits.size() + b.digits.size());

  for (size_t i = 0; i < a.digits.size(); i++) {
    uint32_t carry = 0;
    for (size_t j = 0; j < b.digits.size(); j++) {
      uint32_t m1 = a.digits[i];
      uint32_t m2 = b.digits[j];
      uint64_t product = (uint64_t)(m1) * m2 + carry + result.digits[i + j];
      result.digits[i + j] = product;
      carry = product >> 32;
    }
    result.digits[i + b.digits.size()] = carry;
  }
  strip_leading_zeros(result);
}

BigInt BigInt::operator*(const BigInt& other) const {
  BigInt result;
  mul(*this, other, result);
  return result;
}

int cmp(const BigInt& a, const BigInt& b) {
  int len_min = std::min(a.digits.size(), b.digits.size());
  for (size_t i = len_min; i < a.digits.size(); i++) {
    if (a.digits[i] != 0) return 1;
  }
  for (size_t i = len_min; i < b.digits.size(); i++) {
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

bool BigInt::operator!=(const BigInt& other) const {
  return cmp(*this, other) != 0;
}

bool BigInt::operator<(const BigInt& other) const {
  return cmp(*this, other) < 0;
}
bool BigInt::operator>(const BigInt& other) const {
  return cmp(*this, other) > 0;
}
bool BigInt::operator<=(const BigInt& other) const {
  return cmp(*this, other) <= 0;
}

bool BigInt::operator>=(const BigInt& other) const {
  return cmp(*this, other) >= 0;
}

void add_inplace(BigInt& a, const BigInt& b) {
  if (a.digits.size() < b.digits.size()) {
    a.digits.resize(b.digits.size());
  }

  uint32_t carry = 0;
  for (size_t i = 0; i < b.digits.size(); i++) {
    uint64_t v1 = a.digits[i];
    uint64_t v2 = b.digits[i];
    uint64_t sum = v1 + v2 + carry;
    a.digits[i] = sum;
    carry = sum >> 32;
  }
  for (size_t i = b.digits.size(); carry && i < a.digits.size(); i++) {
    uint64_t v1 = a.digits[i];
    uint64_t sum = v1 + carry;
    a.digits[i] = sum;
    carry = sum >> 32;
  }
  if (carry) a.digits.push_back(carry);
}

BigInt BigInt::operator+(const BigInt& other) const {
  BigInt result = *this;
  add_inplace(result, other);
  return result;
}

BigInt& BigInt::operator+=(const BigInt& other) {
  add_inplace(*this, other);
  return *this;
}

void sub_inplace(BigInt& a, const BigInt& b) {
  assert(a.digits.size() >= b.digits.size());

  // Sign extended carry
  int64_t carry = 0;

  for (size_t i = 0; i < b.digits.size(); i++) {
    uint64_t v1 = a.digits[i];
    uint64_t v2 = b.digits[i];
    uint64_t res = (uint64_t)(v1) - v2 + carry;
    a.digits[i] = res;
    carry = res;
    // Sign extend carry;
    carry >>= 32;
  }

  for (size_t i = b.digits.size(); carry && i < a.digits.size(); i++) {
    uint64_t v1 = a.digits[i];
    uint64_t res = v1 + carry;
    a.digits[i] = res;
    carry = res;
    // Sign extend carry;
    carry >>= 32;
  }
  assert(carry == 0);
  strip_leading_zeros(a);
}

void sub_shifted_inplace(BigInt& a, const BigInt& b, int b_shift) {
  int full = b_shift / 32;
  uint32_t partial = b_shift % 32;
  uint32_t opposite = 32 - partial;
  assert(a.digits.size() >= b.digits.size() + full);

  // Sign extended carry
  int64_t sub_carry = 0;
  uint32_t shift_carry = 0;
  size_t i;

  if (partial != 0) {
    for (i = 0; i < b.digits.size(); i++) {
      uint64_t v1 = a.digits[i + full];
      uint32_t b_word = b.digits[i];
      uint32_t v2 = b_word << partial | shift_carry;
      shift_carry = b_word >> opposite;
      uint64_t res = v1 - v2 + sub_carry;
      a.digits[i + full] = res;
      sub_carry = res;
      // Sign extend sub_carry;
      sub_carry >>= 32;
    }
    if (shift_carry) {
      uint64_t v1 = a.digits[i + full];
      uint64_t res = v1 - shift_carry + sub_carry;
      a.digits[i + full] = res;
      sub_carry = res;
      sub_carry >>= 32;
      i++;
    }
  } else {
    for (i = 0; i < b.digits.size(); i++) {
      uint64_t v1 = a.digits[i + full];
      uint32_t v2 = b.digits[i];
      uint64_t res = v1 - v2 + sub_carry;
      a.digits[i + full] = res;
      sub_carry = res;
      // Sign extend sub_carry;
      sub_carry >>= 32;
    }
  }

  for (; sub_carry && i < a.digits.size(); i++) {
    uint64_t v1 = a.digits[i + full];
    uint64_t res = v1 + sub_carry;
    a.digits[i + full] = res;
    sub_carry = res;
    // Sign extend sub_carry;
    sub_carry >>= 32;
  }
  assert(sub_carry == 0);
  strip_leading_zeros(a);
}

BigInt BigInt::operator-(const BigInt& other) const {
  BigInt result = *this;
  sub_inplace(result, other);
  return result;
}

BigInt& BigInt::operator-=(const BigInt& other) {
  sub_inplace(*this, other);
  return *this;
}

int cmp_shifted(const BigInt& a, const BigInt& b, uint32_t b_shift) {
  int full = b_shift / 32;
  uint32_t partial = b_shift % 32;
  if (partial == 0) {
    // Simple case without bit shifts
    int len_min = std::min(a.digits.size(), full + b.digits.size());
    for (int i = len_min; i < (int) a.digits.size(); i++) {
      if (a.digits[i] != 0) return 1;
    }
    for (int i = std::max(0, len_min - full); i < (int) b.digits.size(); i++) {
      if (b.digits[i] != 0) return -1;
    }
    int i;
    for (i = len_min - 1; i >= full; --i) {
      if (a.digits[i] != b.digits[i - full]) return a.digits[i] < b.digits[i - full] ? -1 : 1;
    }
    for (; i >= 0; --i) {
      if (a.digits[i] != 0) return 1;
    }
    return 0;
  }
  uint32_t opposite = 32 - partial;
  uint32_t carry = 0;
  int len_min = std::min(a.digits.size(), full + b.digits.size());


  if (len_min < (int) a.digits.size()) {
    // A is longer
    for (size_t i = len_min + 1; i < a.digits.size(); i++) {
      if (a.digits[i] != 0) return 1;
    }
    uint32_t a_val = a.digits[len_min];
    uint32_t b_w = len_min - full - 1 >= 0 ? b.digits[len_min - full - 1] : 0;
    uint32_t b_val = b_w >> opposite;
    if (a_val != b_val) return a_val < b_val ? -1 : 1;
    carry = b_w << partial;
  } else {
    // handle: len_min == a.digits.size(), shifted b is longer or equal
    // len_min < full + b.digits.size()
    for (int i = std::max(0, len_min - full); i < (int) b.digits.size(); i++) {
      if (b.digits[i] != 0) return -1;
    }
    uint32_t b_w = len_min - full - 1 >= 0 ? b.digits[len_min - full - 1] : 0;
    if ((b_w >> opposite) != 0) return -1;
    carry = b_w << partial;
  }

  int i;
  for (i = len_min - 1; i >= full + 1; --i) {
    uint32_t b_w = b.digits[i - full - 1];
    uint32_t b_val = carry | (b_w >> opposite);
    if (a.digits[i] != b_val) return a.digits[i] < b_val ? -1 : 1;
    carry = b_w << partial;
  }

  if (i == full && a.digits[full] != carry) return a.digits[full] < carry ? -1 : 1;
  i--;

  for (; i >= 0; --i) {
    if (a.digits[i] != 0) return 1;
  }
  return 0;
}

BigInt shift(const BigInt& in, uint32_t num) {
  BigInt res;
  uint32_t full = num / 32;
  uint32_t partial = num % 32;
  if (partial == 0) {
    res.digits.resize(full + in.digits.size());
    for (size_t i = 0; i < in.digits.size(); i++) {
      res.digits[full + i] = in.digits[i];
    }
  } else {
    uint32_t opposite = 32 - partial;
    res.digits.resize(full + 1 + in.digits.size());
    uint32_t carry = 0;
    for (size_t i = 0; i < in.digits.size(); i++) {
      uint32_t v = in.digits[i];
      res.digits[full + i] = (v << partial) | carry;
      carry = v >> opposite;
    }
    res.digits[full + in.digits.size()] = carry;
  }
  strip_leading_zeros(res);
  return res;
}

BigInt BigInt::shift(uint32_t num) const {
  return ::shift(*this, num);
}

BigInt BigInt::operator<<(uint32_t num) const {
  return ::shift(*this, num);
}

uint32_t msb(const BigInt& v) {
  for (ssize_t i = v.digits.size() - 1; i >= 0; i--) {
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

void modulo(BigInt& a, const BigInt& b) {
  int a_msb = a.msb();
  int b_msb = b.msb();
  int max_shift = a_msb - b_msb;

  for (int shift = max_shift; shift >= 0; shift--) {
    if (cmp_shifted(a, b, shift) >= 0) {
      sub_shifted_inplace(a, b, shift);
    }
  }
}

uint32_t top32(const BigInt& v, uint32_t msb) {
  int offset = msb - 31;
  size_t bword = offset / 32;
  int bshift = offset % 32;
  int compliment = 32 - bshift;
  uint32_t w0 = bword + 1 < v.digits.size() ? v.digits[bword+1] : 0;
  uint32_t w1 = v.digits[bword];
  return w0 << compliment | w1 >> bshift;
}

uint64_t top64(const BigInt& v, uint32_t msb) {
  int offset = msb - 31;
  size_t bword = offset / 32;
  int bshift = offset % 32;
  int compliment = 32 - bshift;
  uint64_t w0 = bword + 1 < v.digits.size() ? v.digits[bword+1] : 0;
  uint64_t w1 = v.digits[bword+0];
  uint64_t w2 = v.digits[bword-1];
  return w0 << (compliment + 32) | w1 << compliment | w2 >> bshift;
}

void modulo2(BigInt& a, const BigInt& b, BigInt& tmp_product, BigInt& tmp_mul) {
  int b_msb = b.msb();

  // The algorithm below only works for large b.
  if (b_msb < 32) { modulo(a, b); return; }

  // Use 31 bits as +1 can potentially overflow?
  uint32_t b_top = top32(b, b_msb + 1) + 1;

  int msb;

  while (true) {
    msb = a.msb();
    if (msb - b_msb < 32) break;

    // 2 zeros at msb
    uint64_t top = top64(a, msb + 2);
    tmp_mul.digits.resize(1);
    tmp_mul.digits[0] = top / b_top;

    mul(tmp_mul, b, tmp_product);
    sub_shifted_inplace(a, tmp_product, msb - b_msb - 31);
  }

  // fallback to per-bit shifts
  int max_shift = msb - b_msb;

  for (ssize_t shift = max_shift; shift >= 0; shift--) {
    if (cmp_shifted(a, b, shift) > 0) {
      sub_shifted_inplace(a, b, shift);
    }
  }
}

void pow(const BigInt& in, const BigInt& d, const BigInt& mod, BigInt& out) {
  out.digits.clear();
  out.digits.push_back(1);

  BigInt tmp_product;
  BigInt modulo_scratch1;
  BigInt modulo_scratch2;
  BigInt v = in;
  uint32_t msb = d.msb();
  uint32_t mask = 1;
  uint32_t idx = 0;

  for (uint32_t i = 0; i <= msb; i++) {
    if ((d.digits[idx] & mask) != 0) {
      mul(out, v, tmp_product);
      modulo2(tmp_product, mod, modulo_scratch1, modulo_scratch2);
      std::swap(out, tmp_product);
    }

    mul(v, v, tmp_product);
    modulo2(tmp_product, mod, modulo_scratch1, modulo_scratch2);
    std::swap(v, tmp_product);

    mask <<= 1;
    if (mask == 0) {
      mask = 1;
      idx++;
    }
  }
}


BigInt BigInt::operator%(const BigInt& other) const {
  BigInt scatch1;
  BigInt scatch2;
  BigInt result = *this;
  modulo2(result, other, scatch1, scatch2);
  return result;
}

BigInt& BigInt::operator%=(const BigInt& other) {
  BigInt scatch1;
  BigInt scatch2;
  modulo2(*this, other, scatch1, scatch2);
  return *this;
}

int main(int argc, char **argv) {
  const BigInt zero = BigInt::fromUint32(0);
  const BigInt one = BigInt::fromUint32(1);
  const BigInt f_16 = BigInt::hex("FFFFFFFFFFFFFFFF");
  const BigInt v64bit = BigInt::hex("10000000000000000");
  const BigInt v64bit_and_one = BigInt::hex("10000000000000001");
  {
    // Mul
    assert((one * one).digits.size() == 1);
  }
  {
    // Cmp
    assert(cmp(BigInt::hex("FFFFFFFF0000000000000000"), one) == 1);
    assert(cmp(one, BigInt::hex("FFFFFFFF0000000000000000")) == -1);
  }
  {
    // Add
    assert(f_16 + one + one == v64bit_and_one);
  }
  {
    // Add inplace
    assert(((BigInt(f_16) += one) += one) == v64bit_and_one);
    assert((BigInt(one) += f_16) == v64bit);
  }
  {
    // Sub
    assert(v64bit_and_one - one - one == f_16);
  }
  {
    // Sub inplace
    assert(((BigInt(v64bit_and_one) -= one) -= one) == f_16);
  }
  {
    // Shift
    const auto v = BigInt::hex("12345678");
    assert(BigInt(v) << 0 == v);
    assert(BigInt(v) << 4 == BigInt::hex("123456780"));
    assert(BigInt(v) << 32 == BigInt::hex("1234567800000000"));
    assert(BigInt::bin("10101") << 1 == BigInt::bin("101010"));
    assert(BigInt::bin("10101").shift(1).digits.size() == 1);

    for (auto base : {f_16, one, zero}) {
      for (int ashift : {0, 1, 2, 30, 31, 32, 33, 63, 64, 65, 64 + 31, 64 + 32, 64 + 33, 127, 128, 129}) {
        for (int bpreshift : {0, 1, 2, 30, 31, 32, 33, 63}) {
          for (int bshift : {0, 1, 2, 30, 31, 32, 33, 63, 64, 127, 128, 129, 511, 512, 513}) {
            // test the other case
            //if (bshift % 64 != 0) continue;
            BigInt a = base << ashift;
            BigInt b = base << bpreshift;
            int res = base == zero ? 0 : (ashift == bpreshift + bshift ? 0
                : (ashift < bpreshift + bshift? -1 : 1));
            if (cmp_shifted(a, b, bshift) != res) {
              cmp_shifted(a, b, bshift);
            }
            assert(cmp_shifted(a, b, bshift) == res);
          }
        }
      }
    }
    for (int bpreshift : {0, 1, 2, 30, 31, 32, 33, 63}) {
      for (int bshift : {0, 1, 2, 30, 31, 32, 33, 63, 64, 127, 128, 129, 511, 512, 513}) {
        assert(cmp_shifted(one << (bpreshift + bshift), one << bpreshift, bshift) == 0);
        assert(cmp_shifted((one << (bpreshift + bshift)) + one, one << bpreshift, bshift) == 1);
        assert(cmp_shifted(one << (bpreshift + bshift), zero, bshift) == 1);
        assert(cmp_shifted(zero, one << bpreshift, bshift) == -1);
      }
    }
  }
  {
    // msb
    assert(zero.msb() == 0);  // kinda odd
    assert(one.msb() == 0);
    assert(f_16.msb() == 63);
    assert(v64bit.msb() == 64);
  }
  {
    auto a = BigInt::hex("5cb61254faaeb95cb4911034303cebdd");
    auto b = BigInt::hex("a824017a138a875f8ddd9134e5e76");
    assert(a > b);
    assert(a * b == BigInt::hex("3ce4862b4460aa39dc8c87750736a11256e2a135dc12948a5912adf05ddde"));
    assert(a + b == BigInt::hex("5cc09495124ff2052a89ee0d438b4a53"));
    assert(a - b == BigInt::hex("5cab9014e30d80b43e98325b1cee8d67"));
    assert(a % b == BigInt::hex("539048ea8fb7c277c4b87e6ffbf11"));

    auto c = a;
    c += b;
    assert(c == a + b);

    c = a;
    c -= b;
    assert(c == a - b);

    assert(top32(b, b.msb()) == 0xa824017a);
    assert(top64(b, b.msb()) == 0xa824017a138a875f);
    assert(top64(b, b.msb() + 31) == 0x1504802f4);
    assert(top32(a, a.msb()) == 0xb96c24a9);
    assert(top32(a, a.msb() + 31) == 1);
    assert(top32(a, a.msb() + 1) == 0x5cb61254);
    assert(top64(a, a.msb()) == 0xb96c24a9f55d72b9);
  }

  {
    BigInt v0 = BigInt::hex("222FFFFFFFFFFFFFFFFFFFFFFF0000000000000");
    BigInt v1 = BigInt::hex("011000000000000000000000010000000000000");
    assert(v0 + v1 == BigInt::hex("234000000000000000000000000000000000000"));
    assert((v0 + v1) - v1 == v0);
  }

  {
    auto v0 = BigInt::hex("c00000000000000000000");
    auto v1 = BigInt::fromUint32(8);
    assert(v0 + v1 > v0);
    assert(v0 - v1 == BigInt::hex("bfffffffffffffffffff8"));
    assert((v0 - v1) + v1 + v1 == BigInt::hex("c00000000000000000008"));
    auto v = v0;
    v -= v1;
    v += v1;
    v += v1;
    assert((v0 - v1) + v1 + v1 == v);
  }

  {
    // Modulo
    assert(BigInt::hex("2542352412354554635652352234432545634")
        % BigInt::hex("34324123434345345") == BigInt::hex("c81a554d5702408c"));

    assert(BigInt::hex("F0F0F0F0F") == BigInt::bin("111100001111000011110000111100001111"));
  }

  {
    // Pow
    const auto d = BigInt::hex("2f92cf6ccda9205e17a1d45e2351c4fd");
    const auto n = BigInt::hex("8932aba75549986ced8038695dfe89b5");
    const auto in = BigInt::fromUint32(2);
    BigInt enc;
    pow(in, BigInt::fromUint32(65537), n, enc);
    assert(enc == BigInt::hex("107f6020d4697f2598701df630ec0751"));
    BigInt out;
    pow(enc, d, n, out);
    assert(out == in);
    for (int i = 1; i < 50000; i++) {
      const auto in = BigInt::fromUint32(i);
      BigInt enc;
      pow(in, BigInt::fromUint32(65537), n, enc);
      BigInt out;
      pow(enc, d, n, out);
      assert(out == in);
    }
  }
  printf("Tests passed\n");
}

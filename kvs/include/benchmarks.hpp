#ifndef __BIGNUM_H__
#define __BIGNUM_H__

#include <Arduino.h>
#include "bignum.hpp"

void runEncryptionBenchmarks128(int rounds) {
  const auto n = BigInt::hex("8932aba75549986ced8038695dfe89b5");
  for (int i = 0; i < rounds; ++i) {
      const auto in = BigInt::fromUint32(i + 2);
      BigInt enc;
      pow(in, BigInt::fromUint32(65537), n, enc);
  }
}

void runDecryptionBenchmarks128(int rounds) {
  const auto d = BigInt::hex("2f92cf6ccda9205e17a1d45e2351c4fd");
  const auto n = BigInt::hex("8932aba75549986ced8038695dfe89b5");
  for (int i = 0; i < rounds; ++i) {
      const auto in = BigInt::fromUint32(i + 2);
      BigInt out;
      pow(in, d, n, out);
  }
}

void runEncryptionBenchmarks1024(int rounds) {
  const auto n = BigInt::hex("91e1305f2262d2f89e2437ed19dd3af449f95304bf1eebb15cf98fff7c7b791929e491940affc63719c0eaacbb2dc744ab493457488fd139ea343dbc5cba07b061db6933d37fb9f63c631db69ff278f68461ebaf004411830e8959410b83e496d98237fd2fb5f077e9b925fd29bbce9980a9aca6919e480cb2b2070d486f5985");
  for (int i = 0; i < rounds; ++i) {
      const auto in = BigInt::fromUint32(i + 2);
      BigInt enc;
      pow(in, BigInt::fromUint32(65537), n, enc);
  }
}

void runDecryptionBenchmarks1024(int rounds) {
  const auto d = BigInt::hex("452b94c71743eaab2e39183799b2f7b84bb727f1313a5521a567442b2d7852846703d52f7043b555fd2527613de3c7ffbc841a886484a4fda41a9f474864812e18c78e14d6532441ce916b0db46394177f4c53579283f15766e31fdac1646ef2191b9d6b599d07716ade4b3f28e1c5fe8c313372b855341a7fd02cb8606e7b05");
  const auto n = BigInt::hex("91e1305f2262d2f89e2437ed19dd3af449f95304bf1eebb15cf98fff7c7b791929e491940affc63719c0eaacbb2dc744ab493457488fd139ea343dbc5cba07b061db6933d37fb9f63c631db69ff278f68461ebaf004411830e8959410b83e496d98237fd2fb5f077e9b925fd29bbce9980a9aca6919e480cb2b2070d486f5985");
  for (int i = 0; i < rounds; ++i) {
      const auto in = BigInt::fromUint32(i + 2);
      BigInt out;
      pow(in, d, n, out);
  }
}

void runEncryptionBenchmarks2048(int rounds) {
  const auto n = BigInt::hex("b3bed9b83164e2faaabf690bf49266f06b2c28c29ef79702363aca94449c59de24e53c2d116e754acd00a37a31ca4621902c5912653e6d372d7a65f52921e2e3c9131279c8576059a06ff47b629117fb7b1cbea779afb7ae233cd1c134386125f1a4d9b64637e13268c86fdb7756515312bfe4b80690f92b30fb701cc07307ece6bbca1afea7b64cfc8e4bee0fab983d610a16b5b476bc92672e6245b479efa41443daed3543afc6c3abd4fd9e73c3ba49654f590c5efcda307f3c87a7faccf8337a24025a0491b04cb5c8f79f4104cc65480ab294507b1a302dae387e6af74f74f17b29ad214abdf44a176adc360a2c287c547a0cb0fedce07a72846d2ee7dd");
  for (int i = 0; i < rounds; ++i) {
      const auto in = BigInt::fromUint32(i + 2);
      BigInt enc;
      pow(in, BigInt::fromUint32(65537), n, enc);
  }
}

void runDecryptionBenchmarks2048(int rounds) {
  const auto d = BigInt::hex("87410e8db6c566b31ef727b1b258d4130762544d96d7520f7d0522590d83e56feead7c69a15b4ff1786f8994329135d29dad715d11483760b4dd0c89bd68dd557081e47e354de297aecd6fd7c4a4cfba11bae0901f56254e68ffa40ad123b79e98c5e6796d3f313b00740b0fc72df1c6242eb789ee8efaa0109cb3913f13901ae5958e06981d3b8f69bba52cb6645216d6b68becb528f1ddbdf6e50ff5526689eecdf3f0a2a5feb389f8769914bc2e9ef576f88d439af133c0623d961b385a2b6883bb9dbf0a5f2f90e1af7a78804874b2c71a4929284a4fde9f7f92b56cccc7f9db38773b9fb92c02ebeb21dafd285d6acb9e88217872bd6942c257befcacb5");
  const auto n = BigInt::hex("b3bed9b83164e2faaabf690bf49266f06b2c28c29ef79702363aca94449c59de24e53c2d116e754acd00a37a31ca4621902c5912653e6d372d7a65f52921e2e3c9131279c8576059a06ff47b629117fb7b1cbea779afb7ae233cd1c134386125f1a4d9b64637e13268c86fdb7756515312bfe4b80690f92b30fb701cc07307ece6bbca1afea7b64cfc8e4bee0fab983d610a16b5b476bc92672e6245b479efa41443daed3543afc6c3abd4fd9e73c3ba49654f590c5efcda307f3c87a7faccf8337a24025a0491b04cb5c8f79f4104cc65480ab294507b1a302dae387e6af74f74f17b29ad214abdf44a176adc360a2c287c547a0cb0fedce07a72846d2ee7dd");
  for (int i = 0; i < rounds; ++i) {
      const auto in = BigInt::fromUint32(i + 2);
      BigInt out;
      pow(in, d, n, out);
  }
}

void run() {
  unsigned long startTime, encTime, decTime;
  int ROUNDS128 = 100;

  startTime = micros();
  runEncryptionBenchmarks128(ROUNDS128);
  encTime = micros() - startTime;
  Serial.print("RSA-128 encryption: ");
  Serial.print(encTime / ROUNDS128);
  Serial.println(" microseconds");

  startTime = micros();
  runDecryptionBenchmarks128(ROUNDS128);
  decTime = micros() - startTime;
  Serial.print("RSA-128 decryption: ");
  Serial.print(decTime / ROUNDS128);
  Serial.println(" microseconds");

  int ROUNDS_1024 = 10;

  startTime = micros();
  runEncryptionBenchmarks1024(ROUNDS_1024);
  encTime = micros() - startTime;
  Serial.print("RSA-1024 encryption: ");
  Serial.print(encTime / ROUNDS_1024);
  Serial.println(" microseconds");

  startTime = micros();
  runDecryptionBenchmarks1024(ROUNDS_1024);
  decTime = micros() - startTime;
  Serial.print("RSA-1024 decryption: ");
  Serial.print(decTime / ROUNDS_1024);
  Serial.println(" microseconds");

  int ROUNDS_2048 = 10;

  startTime = micros();
  runEncryptionBenchmarks2048(ROUNDS_2048);
  encTime = micros() - startTime;
  Serial.print("RSA-2048 encryption: ");
  Serial.print(encTime / ROUNDS_2048);
  Serial.println(" microseconds");

  startTime = micros();
  runDecryptionBenchmarks2048(ROUNDS_2048);
  decTime = micros() - startTime;
  Serial.print("RSA-2048 decryption: ");
  Serial.print(decTime / ROUNDS_2048);
  Serial.println(" microseconds");
}

#endif  // __BIGNUM_H__

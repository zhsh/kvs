#ifdef ESP32

#include <Arduino.h>
#include <BluetoothSerial.h>
#include "storage.hpp"
#include "bignum.hpp"

/* Check if Bluetooth configurations are enabled in the SDK */
/* If not, then you have to recompile the SDK */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif


FlashStorage storage;
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(9600);
  Serial.println("Initialized.");
}

void _setup() {
  Serial.begin(115200);
  // delay(5000);
  Serial.println("Initialized.");
  storage.Init();
  auto value = storage.Get("foo");
  Serial.println(value.c_str());

  /* If no name is given, default 'ESP32' is applied */
  /* If you want to give your own name to ESP32 Bluetooth device, then */
  /* specify the name as an argument SerialBT.begin("myESP32Bluetooth*/
  SerialBT.begin();
  Serial.println("Bluetooth Started! Ready to pair...");
}

String readLine() {
  String line;
  while (true) {
    int c = SerialBT.read();
    if (c == -1) {
      continue;
    }
    if (c == '\n') {
      return line;
    }
    line += char(c);
  }
}

void runEncryptionBenchmarks(int rounds) {
  const auto n = BigInt::hex("8932aba75549986ced8038695dfe89b5");
  for (int i = 0; i < rounds; ++i) {
      const auto in = BigInt::fromUint32(i + 2);
      BigInt enc;
      pow(in, BigInt::fromUint32(65537), n, enc);
  }
}

void runDecryptionBenchmarks(int rounds) {
  const auto d = BigInt::hex("2f92cf6ccda9205e17a1d45e2351c4fd");
  const auto n = BigInt::hex("8932aba75549986ced8038695dfe89b5");
  for (int i = 0; i < rounds; ++i) {
      const auto in = BigInt::fromUint32(i + 2);
      BigInt out;
      pow(in, d, n, out);
  }
}

void loop() {
  int ROUNDS = 100;

  auto startTime = micros();
  runEncryptionBenchmarks(ROUNDS);
  auto encTime = micros() - startTime;
  Serial.print("Encryption: ");
  Serial.print(encTime / ROUNDS);
  Serial.println(" micro sec");

  startTime = micros();
  runDecryptionBenchmarks(ROUNDS);
  auto decTime = micros() - startTime;
  Serial.print("Decryption: ");
  Serial.print(decTime / ROUNDS);
  Serial.println(" micro sec");

  delay(2000);
}

void _loop() {
  String s = readLine();
  Serial.println(s);
  String toSend = s;
  toSend.toUpperCase();
  toSend += '\n';
  Serial.print(toSend);
  SerialBT.write((const uint8_t*)toSend.c_str(), toSend.length());

  delay(20);
}
#endif  // ESP32

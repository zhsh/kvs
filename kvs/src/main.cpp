#include <Arduino.h>
#include <BluetoothSerial.h>
#include "storage.hpp"

/* Check if Bluetooth configurations are enabled in the SDK */
/* If not, then you have to recompile the SDK */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif


FlashStorage storage;
BluetoothSerial SerialBT;

void setup() {
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

void loop() {
  String s = readLine();
  Serial.println(s);
  String toSend = s;
  toSend.toUpperCase();
  toSend += '\n';
  Serial.print(toSend);
  SerialBT.write((const uint8_t*)toSend.c_str(), toSend.length());

  delay(20);
}
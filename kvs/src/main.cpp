#ifdef ESP32

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
#else

#include <ArduinoBLE.h>
#include "FlashIAP.h"
#include "BLEProperty.h"
#include "storage.hpp"

BLEService battery("180F");

BLEUnsignedCharCharacteristic batteryLevel("2A19", BLERead | BLENotify);
BLEDescriptor batteryLevelDescriptor("2901", "Percentage 0 - 100");

FlashStorage store;

void setup() {
  Serial.begin(9600);
  // Comment out when running on battery ;)
  while (!Serial);
  Serial.println("Connected.");

  store.Init();
  //store.Put("foo", "bar");
  auto res = store.Get("foo");
  Serial.println(res.c_str());


  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin
  digitalWrite(LED_BUILTIN, HIGH);

  if (!BLE.begin()) {   // initialize BLE
    Serial.println("starting BLE failed!");
    while (1);
  }
  BLE.setLocalName("Nano33BLE");  // Set name for connection
  BLE.setAdvertisedService(battery); // Advertise service
  batteryLevel.addDescriptor(batteryLevelDescriptor);
  batteryLevel.setValue(75);
  battery.addCharacteristic(batteryLevel); // Add characteristic to service
  BLE.addService(battery); // Add service

  BLE.advertise();  // Start advertising
  Serial.print("Peripheral device MAC: ");
  Serial.println(BLE.address());
  Serial.println("Waiting for connections...");
}

int batteryPercent = 75;
unsigned long last_update = 0;

void loop() {
  BLEDevice central = BLE.central();  // Wait for a BLE central to connect

  // if a central is connected to the peripheral:
  if (central) {
    Serial.print("Connected to central MAC: ");
    // print the central's BT address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, LOW);

    while (central.connected()) {
      delay(10);
      unsigned long ms = millis();
      if (ms - last_update > 1000) {
        last_update = ms;
        batteryPercent = (batteryPercent + 23) % 100;
        batteryLevel.setValue(batteryPercent);
        batteryLevel.broadcast();
        Serial.print("Battery: ");
        Serial.println(batteryPercent);
      }
    }

    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.print("Disconnected from central MAC: ");
    Serial.println(central.address());
  }
}
#endif  // !ESP32

#ifdef MBED_CONF_ESP8266_PROVIDE_DEFAULT
#endif

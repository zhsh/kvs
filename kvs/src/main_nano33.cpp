#ifndef ESP32

#include <string>
#include <ArduinoBLE.h>
#include "FlashIAP.h"
#include "BLEProperty.h"
#include "storage.hpp"
#include "enc.hpp"

BLEService battery("180F");
BLEUnsignedCharCharacteristic batteryLevel("2A19", BLERead | BLENotify);
BLEDescriptor batteryLevelDescriptor("2A19", "Percentage 0 - 100");

// random 128bit uuid
BLEService challengeService("DFe8CFA50FFA08E336DE0F884944750F");
BLECharacteristic challengeChar("DFe8CFA50FFA08E336DE0F8849447510", 
  BLERead | BLENotify | BLEWrite, /*value_length=*/sizeof(EncMessage::data), /*fixed_length*/true);
BLEDescriptor challengeDesc("2A19", "Blah-blah");

FlashStorage store;

void challengeHandler(BLEDevice device, BLECharacteristic characteristic) {
  EncMessage reply;
  challengeReply((EncMessage *) characteristic.value(), &reply);
  characteristic.setValue(reply.data, sizeof(reply.data));
}

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

  BLE.setAdvertisedService(challengeService);
  // challengeChar.addDescriptor(challengeDesc);
  challengeChar.setValue("foobar");
  challengeChar.setEventHandler(BLEWritten, &challengeHandler);
  challengeService.addCharacteristic(challengeChar);
  BLE.addService(challengeService);


  BLE.advertise(); // Start advertising
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

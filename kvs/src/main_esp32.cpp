#ifdef ESP32

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "storage.hpp"

/* Check if Bluetooth configurations are enabled in the SDK */
/* If not, then you have to recompile the SDK */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID  "64dfd1b8-0c60-46b7-9840-76c1fdcbe011"
#define INPUT_UUID    "a30b08c5-94a3-4678-a026-47dcf3ebec1f"
#define OUTPUT_UUID   "4c599c12-9cff-4d7b-aea6-b811907f367e"


FlashStorage storage;

BLECharacteristic* InputCharacteristic;
BLECharacteristic* OutputCharacteristic;

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("onConnect");
  };
  void onDisconnect(BLEServer* pServer) {
    Serial.println("onDisconnect");
    Serial.println("Starting to advertise");
    pServer->getAdvertising()->start();
  }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    Serial.println("onWrite starts");
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0 && OutputCharacteristic != nullptr)
    {
      Serial.println("*********");
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++)
      {
        Serial.print(value[i]);
      }

      Serial.println();
      Serial.println("*********");

      std::string output = value + "_secret";
      OutputCharacteristic->setValue(output);
      OutputCharacteristic->notify();
    }
    Serial.println("onWrite ends");
  }
};

void setup() {
  Serial.begin(9600);
  Serial.println("Initialized.");
  storage.Init();
  auto value = storage.Get("foo");
  Serial.println(value.c_str());

  BLEDevice::init("Key-Value Storage");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  InputCharacteristic = pService->createCharacteristic(
                                         INPUT_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  OutputCharacteristic = pService->createCharacteristic(
                                         OUTPUT_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );
  OutputCharacteristic->addDescriptor(new BLE2902());

  InputCharacteristic->setValue("Key");
  InputCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  Serial.println("Starting to advertise");
  pServer->getAdvertising()->start();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  delay(2000);
}

#endif  // ESP32

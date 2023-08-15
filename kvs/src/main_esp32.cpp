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

#define SERVICE_UUID            "64dfd1b8-0c60-46b7-9840-76c1fdcbe011"
#define READ_KEY_INPUT_UUID     "a30b08c5-94a3-4678-a026-47dcf3ebec1f"
#define READ_VALUE_OUTPUT_UUID  "4c599c12-9cff-4d7b-aea6-b811907f367e"
#define WRITE_KEY_INPUT_UUID    "3dadb224-692c-4e3b-b02f-b5d1c2c43b13"
#define WRITE_VALUE_INPUT_UUID  "7775c5de-9143-4d69-9619-c4c537e926f7"
#define WRITE_READY_INPUT_UUID  "9eaa2c33-1f21-441a-924c-8399e5ee9b90"


FlashStorage storage;

BLECharacteristic* ReadKeyInput;
BLECharacteristic* ReadValueOutput;
BLECharacteristic* WriteKeyInput;
BLECharacteristic* WriteValueInput;
BLECharacteristic* WriteReadyInput;

std::string recentWriteKey;
std::string recentWriteValue;

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

class ReadCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    Serial.println("onWrite starts");
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0 && ReadValueOutput != nullptr)
    {
      std::string output = storage.Get(value);
      Serial.print(value.c_str());
      Serial.print(": ");
      Serial.println(output.c_str());

      ReadValueOutput->setValue(output);
      ReadValueOutput->notify();
    }
    Serial.println("onWrite ends");
  }
};

class WriteKeyCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    Serial.print("WriteKeyCallback: ");
    recentWriteKey = pCharacteristic->getValue();
    Serial.println(recentWriteKey.c_str());
  }
};

class WriteValueCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    Serial.print("WriteValueCallback: ");
    recentWriteValue = pCharacteristic->getValue();
    Serial.println(recentWriteValue.c_str());
  }
};

class WriteReadyCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    Serial.println("WriteReadyCallback");
    if (!recentWriteKey.empty() && !recentWriteValue.empty()) {
      Serial.print(recentWriteKey.c_str()); Serial.print(": "); Serial.println(recentWriteValue.c_str());
      storage.Put(recentWriteKey, recentWriteValue);
    }
  }
};

void printKeyValue(const std::string key) {
  auto value = storage.Get(key);
  Serial.print(key.c_str());
  Serial.print(": ");
  Serial.println(value.c_str());
}

void setup_write() {
  Serial.begin(9600);
  Serial.println("Initialized.");
  storage.Init();
  Serial.println("Storage initialized.");

  Serial.print("Putting keys and values... ");
  storage.Put("Firstname", "Antonio");
  storage.Put("Lastname", "Banderas");
  storage.Put("Age", "30");
  Serial.println("DONE");

  printKeyValue("Firstname");
  printKeyValue("Lastname");
  printKeyValue("Age");
  printKeyValue("Salary");
}

void setup_read() {
  Serial.begin(9600);
  Serial.println("Initialized.");
  storage.Init();
  Serial.println("Storage initialized.");

  printKeyValue("Firstname");
  printKeyValue("Lastname");
  printKeyValue("Age");
  printKeyValue("Salary");
}

void setup_main() {
  Serial.begin(9600);
  Serial.println("Initialized.");
  storage.Init();

  BLEDevice::init("Key-Value Storage");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  ReadKeyInput = pService->createCharacteristic(
                                         READ_KEY_INPUT_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  ReadValueOutput = pService->createCharacteristic(
                                         READ_VALUE_OUTPUT_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );
  ReadValueOutput->addDescriptor(new BLE2902());

  ReadKeyInput->setValue("Key");
  ReadKeyInput->setCallbacks(new ReadCallback());

  WriteKeyInput = pService->createCharacteristic(
                                         WRITE_KEY_INPUT_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  WriteKeyInput->setCallbacks(new WriteKeyCallback());

  WriteValueInput = pService->createCharacteristic(
                                         WRITE_VALUE_INPUT_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  WriteValueInput->setCallbacks(new WriteValueCallback());

  WriteReadyInput = pService->createCharacteristic(
                                         WRITE_READY_INPUT_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  WriteReadyInput->setCallbacks(new WriteReadyCallback());

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

void setup() {
  setup_main();
}

void loop() {
  delay(2000);
}

#endif  // ESP32

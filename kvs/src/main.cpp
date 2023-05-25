#include <Arduino.h>
#include "storage.hpp"

FlashStorage storage;

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.println("Initialized.");
  storage.Init();
  auto value = storage.Get("foo");
  Serial.println(value.c_str());
}

void loop() {
  // put your main code here, to run repeatedly:
}

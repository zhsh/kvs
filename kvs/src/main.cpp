#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.println("Initialized.");
  Serial.printf("Flash size: %d\n", ESP.getFlashChipSize());

  // Dump partition table
  //
  // Partition API:
  // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/partition.html
  //
  // Types:
  // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html
  //
  // Alternative partition layouts:
  // https://github.com/espressif/arduino-esp32/tree/master/tools/partitions
  auto it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, nullptr);
  while(it != nullptr) {
    auto *partition = esp_partition_get(it);
    Serial.printf("Type: %d Subtype: %d, Addr: 0x%x, Size: 0x%x Label: %s\n",
        partition->type, partition->subtype, partition->address, partition->size,
        partition->label != nullptr ? partition->label : "<no label>");
    it = esp_partition_next(it);
  }

  // TODO: create custom partition table, find it, experiment with writting/overwritting flash.
}

void loop() {
  // put your main code here, to run repeatedly:
}

#include "storage.hpp"
#include "Arduino.h"
#include "esp_debug_helpers.h"
#include "nvs_flash.h"
#include "rom/crc.h"

const char* Storage::kMagic = "kvs";

void fail() {
    printf("Abort.\n");
    esp_backtrace_print(32);
    while (true) sleep(1);
}

 void FlashStorage::BoundsCheck(const char*op, uint32_t pos, uint32_t length) {
  if (pos > length_ || length > length_ || pos + length > length_) {
    Serial.printf("Out of bounds %s: %d/%d\n", op, pos, length);
    fail();
  }
}

void FlashStorage::Write(uint32_t pos, const char*buf, uint32_t length) {
  BoundsCheck("write", pos, length);

  esp_err_t res = esp_flash_write(esp_flash_default_chip, buf, start_ + pos, length);
  if (res != ESP_OK) {
    printf("Failed to write %s to %d: err %d\n", buf, 0, res);
    fail();
  }
}

void FlashStorage::Read(uint32_t pos, char*buf, uint32_t length) {
  BoundsCheck("read", pos, length);

  esp_err_t res = esp_flash_read(esp_flash_default_chip, buf, start_ + pos, length);
  if (res == ESP_OK) {
    return;
  } else if (res == ESP_ERR_NO_MEM) {
    Serial.println("No permissions");
    fail();
  } else {
    Serial.printf("Unknown error reading flash: %d\n", res);
    fail();
  }
}

void FlashStorage::Erase(uint32_t pos, uint32_t length) {
  BoundsCheck("erase", pos, length);
  if ((length & ~(kBlockSize - 1)) != 0 || length == 0) {
    Serial.printf("Incorrect erase alignment: length=0x%x", length);
    fail();
  }

  esp_err_t res = esp_flash_erase_region(esp_flash_default_chip, start_ + pos, length);
  if (res != ESP_OK) {
    Serial.printf("Failed to erase, error: %d\n", res);
    fail();
  }
}

bool FlashStorage::Init() {
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
        partition->label);
    it = esp_partition_next(it);
  }

  auto *p = esp_partition_find_first(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, "ffat");
  if (p == nullptr) {
    Serial.println("Failed to find ffat partition\n");
    fail();
  }
  start_ = p->address;
  length_ = p->size;

  esp_err_t res = esp_flash_init(esp_flash_default_chip);
  if (res != ESP_OK) {
    Serial.printf("Flash init failure: %d\n", res);
    fail();
  }

  return true;
}

Storage::Status Storage::Put(const char* key, const char* value) {

  uint32_t key_length = strlen(key);
  uint32_t value_length = strlen(value);
  uint32_t total_length = key_length + value_length;

  if (key_length + value_length > 254) {
    Serial.printf("Total length too big %d\n", total_length);
    return STORAGE_INVALID_ARG;
  }

  char buf[256];
  uint8_t lens[2];
  uint32_t offset = 0;
  ReadNext(&offset, buf, strlen(kMagic));

  // Not initialized, do lazy init
  if (strncmp(buf, kMagic, strlen(kMagic)) != 0) {
    Erase(0, kBlockSize);
    Write(0, kMagic, strlen(kMagic));
  } else {
    while (true) {
      ReadNext(&offset, (char*)lens, sizeof(lens));
      // End of log
      if (lens[0] == 255) break;

      // Key size match, check if old key matches
      if (lens[1] == key_length) {
        ReadNext(&offset, buf, key_length);

        if (strncmp(buf, key, key_length) == 0) {
          // Wipe old key, value and key length
          bzero(buf, lens[0] + 1);
          Write(offset - key_length - 1, buf, lens[0] + 1);
        }
        // Skip value
        offset += lens[0] - lens[1];
      } else {
        // Skip key and value
        offset += lens[0];
      }
    }
    // Go back for the header size
    offset -= sizeof(lens);
  }
  uint32_t end_offset = offset + 2 + total_length;

  // First, make sure next flash page is cleared lazily if needed
  // Keep 256 bytes cleared after the end of the new record.
  uint32_t old_cleared_page = (offset + 256) / kBlockSize;
  uint32_t new_cleared_page = (end_offset + 256) / kBlockSize;
  if (new_cleared_page != old_cleared_page) {
    Erase(new_cleared_page * kBlockSize, kBlockSize);
  }
  // Write total length first. If it fails we just have a bad record.
  // Serial.printf("Write record %d len %d\n", offset, total_length + 2);
  Write(offset, (char*)&total_length, 1);
  Write(offset + 1, (char*)&key_length, 1);
  Write(offset + 2, key, key_length);
  Write(offset + 2 + key_length, value, value_length);
  return STORAGE_OK;
}

std::string Storage::Get(const char *key) {
  char buf[256];
  uint8_t lens[2];

  uint32_t offset = 0;
  ReadNext(&offset, buf, strlen(kMagic));

  // Not initialized, no result
  if (strncmp(buf, kMagic, strlen(kMagic)) != 0)
    return "";

  uint32_t key_length = strlen(key);

  while (true) {
    ReadNext(&offset, (char*)lens, sizeof(lens));
    if (lens[0] == 255) return "";
    if (lens[1] < lens[0] && lens[1] == key_length) {
      // Key length match, read key
      ReadNext(&offset, buf, lens[1]);
      if (strncmp(buf, key, key_length) == 0) {
        // Found match, read value
        ReadNext(&offset, buf, lens[0] - lens[1]);
        return std::string(buf, lens[0] - lens[1]);
      } else {
        offset += lens[0] - lens[1];
      }
    } else {
      offset += lens[0];
    }
  }
}

std::string Storage::List() const {
    return "Not implemented yet";
}

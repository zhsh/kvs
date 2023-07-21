#include "storage.hpp"
#include <memory>
#include <cstring>

const char* Storage::kMagic = "kvs0";

#ifdef ESP32
#include "Arduino.h"
#include "esp_debug_helpers.h"
#include "nvs_flash.h"
#include "rom/crc.h"


void fail() {
    printf("Abort.\n");
    esp_backtrace_print(32);
    while (true) sleep(1);
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
  if ((length & (sector_size_ - 1)) != 0 || length == 0) {
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
  SetSectorAndPageSize(4096, 1);

  esp_err_t res = esp_flash_init(esp_flash_default_chip);
  if (res != ESP_OK) {
    Serial.printf("Flash init failure: %d\n", res);
    fail();
  }

  return true;
}
#endif  // ESP32
#ifdef ARDUINO_ARCH_MBED

void fail() {
    printf("Abort.\n");
    while (true) delay(1);
}

#include "ArduinoBLE.h"
#include "FlashIAP.h"

void FlashStorage::Write(uint32_t pos, const char*buf, uint32_t length) {
  BoundsCheck("write", pos, length);

  int res = flash_.program(buf, start_ + pos, length);
  if (res != 0) {
    Serial.print("Failed to write ");
    Serial.print(length);
    Serial.print(" bytes: ");
    Serial.println(res);
    fail();
  }
}

void FlashStorage::Read(uint32_t pos, char*buf, uint32_t length) {
  BoundsCheck("read", pos, length);

  int res = flash_.read(buf, start_ + pos, length);
  if (res == 0) {
    return;
  } else {
    Serial.print("Flash read err: ");
    Serial.println(res);
    fail();
  }
}

void FlashStorage::Erase(uint32_t pos, uint32_t length) {
  Serial.print("Erase ");
  Serial.println(length);
  BoundsCheck("erase", pos, length);
  if ((length & (sector_size_ - 1)) != 0 || length == 0) {
    Serial.println("Incorrect erase alignment");
    Serial.println(pos);
    Serial.println(length);
    Serial.println(sector_size_);
    fail();
  }

  int res = flash_.erase(start_ + pos, length);
  if (res != 0) {
    Serial.print("Failed to erase, error: ");
    Serial.println(res);
    fail();
  }
}

bool FlashStorage::Init() {
  int status = flash_.init();
  if (status != 0) {
    Serial.print("Init flash failure: ");
    Serial.println(status);
    fail();
  }

  start_ = 3 * 256 * 1024;
  length_ = 256 * 1024;
  SetSectorAndPageSize(flash_.get_sector_size(start_), flash_.get_sector_size(start_));
  return true;
}

void bzero(void *buf, size_t length) {
  char* chars = (char*) buf;
  for (size_t i = 0; i < length; i++) chars[i] = 0;
}

#endif  // ARDUINO_ARCH_MBED

void FlashStorage::BoundsCheck(const char *op, uint32_t pos, uint32_t length) {
    if (pos > length_ || length > length_ || pos + length > length_)
    {
    Serial.print("Out of bounds ");
    Serial.print(op);
    Serial.print(": pos");
    Serial.print(pos);
    Serial.print(" length ");
    Serial.println(length);
    fail();
    }
}

void Storage::SetSectorAndPageSize(uint32_t sector_sz, uint32_t page_sz) {
  sector_size_ = sector_sz;
  page_size_ = page_sz;
  Serial.print("Sector size: ");
  Serial.println(sector_size_);
  buffer_.resize(2 + 254 + page_size_ - 1);

}

Storage::Status Storage::Put(const std::string& key, const std::string& value) {

  uint32_t key_length = key.length();
  uint32_t value_length = value.length();
  uint32_t total_length = key_length + value_length;

  if (key_length + value_length > 254) {
    Serial.print("Total length too big ");
    Serial.println(total_length);
    return STORAGE_INVALID_ARG;
  }

  uint8_t lens[2];
  uint32_t offset = 0;
  ReadNext(&offset, buffer_.data(), strlen(kMagic));

  // Not initialized, do lazy init
  if (memcmp(buffer_.data(), kMagic, strlen(kMagic)) != 0) {
    int erase_size = strlen(kMagic) + 1;
    Erase(0, (erase_size + sector_size_ - 1) & ~(sector_size_ - 1));
    Write(0, kMagic, strlen(kMagic));
  } else {
    while (true) {
      int start = offset;
      ReadNext(&offset, (char*)lens, sizeof(lens));
      // End of log
      if (lens[0] == 255) break;
      int end = (offset + lens[0] + page_size_ - 1) & ~(page_size_ - 1);

      // Key size match, check if old key matches
      if (lens[1] == key_length) {
        ReadNext(&offset, buffer_.data(), key_length);

        if (memcmp(buffer_.data(), key.data(), key_length) == 0) {
          // Wipe old key, value and key length
          bzero(buffer_.data(), end - start);
          buffer_[0] = lens[0];
          Write(start, buffer_.data(), end - start);
        }
      }
      offset = end;
    }
    // Go back for the header size
    offset -= sizeof(lens);
  }
  uint32_t end_offset = (offset + 2 + total_length + page_size_ - 1) & ~(page_size_ - 1);

  // Clear lazily. At the start expect that the entire page
  // containing offset + page_size_ is erased. Make sure end_offset + page_size_
  // is erased.
  uint32_t next_page = (offset + page_size_ + sector_size_ - 1) & ~(sector_size_ - 1);
  uint32_t last_page = (end_offset + page_size_ + sector_size_ - 1) & ~(sector_size_ - 1);
  if (last_page != next_page) {
    Erase(next_page, last_page - next_page);
  }


  // Write total length first. If it fails we just have a bad record.
  buffer_[0] = total_length;
  buffer_[1] = key_length;
  memcpy(buffer_.data() + 2, key.data(), key_length);
  memcpy(buffer_.data() + 2 + key_length, value.data(), value_length);
  uint32_t padded_length = (2 + total_length + page_size_ - 1) & ~(page_size_ - 1);
  for (uint32_t i = total_length + 2; i < padded_length; i++)
    buffer_[i] = 0;
  Write(offset, buffer_.data(), padded_length);
  return STORAGE_OK;
}

std::string Storage::Get(const std::string& key) {
  uint8_t lens[2];

  uint32_t offset = 0;
  ReadNext(&offset, buffer_.data(), strlen(kMagic));

  // Not initialized, no result
  if (memcmp(buffer_.data(), kMagic, strlen(kMagic)) != 0)
    return "";

  while (true) {
    ReadNext(&offset, (char*)lens, sizeof(lens));
    if (lens[0] == 255) return "";
    uint32_t end = (offset + lens[0] + page_size_ - 1) & ~(page_size_ - 1);
    if (lens[1] < lens[0] && lens[1] == key.length()) {
      // Key length match, read key
      ReadNext(&offset, buffer_.data(), lens[1]);
      if (memcmp(buffer_.data(), key.data(), key.length()) == 0) {
        // Found match, read value
        ReadNext(&offset, buffer_.data(), lens[0] - lens[1]);
        return std::string(buffer_.data(), lens[0] - lens[1]);
      }
    }
    offset = end;
  }
}

std::string Storage::List() const {
    return "Not implemented yet";
}

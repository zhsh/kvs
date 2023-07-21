#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <string>
#include <vector>

#ifdef ARDUINO_ARCH_MBED
#include "ArduinoBLE.h"
#include "FlashIAP.h"
#endif

#ifdef HOST
#include "host.h"
#endif

void fail();

class Storage {
  public:
    enum Status {
        STORAGE_OK,
        STORAGE_INVALID_ARG,
        STORAGE_STORAGE_FAILURE,
    };

    virtual bool Init() = 0;
    Status Put(const std::string& key, const std::string& value);
    std::string Get(const std::string& key);
    std::string List() const;

    static const char* kMagic;

  protected:
    void SetSectorAndPageSize(uint32_t sector_sz, uint32_t page_sz);
    virtual void Write(uint32_t pos, const char*buf, uint32_t length) = 0;
    virtual void Read(uint32_t pos, char* buf, uint32_t length) = 0;
    virtual void Erase(uint32_t pos, uint32_t size) = 0;

    virtual void ReadNext(uint32_t* pos, char* buf, uint32_t length) {
        Read(*pos, buf, length);
        *pos += length;
    }

    uint32_t sector_size_;
    uint32_t page_size_;
    std::vector<char> buffer_;
};

class FlashStorage : public Storage {
  public:
    bool Init() override;

  protected:
    void Write(uint32_t pos, const char*buf, uint32_t length) override;
    void Read(uint32_t pos, char *buf, uint32_t length) override;
    void Erase(uint32_t pos, uint32_t size) override;

  private:
    void BoundsCheck(const char *op, uint32_t pos, uint32_t length);
    // Data partition
    uint32_t start_ = ~0;
    uint32_t length_ = ~0;
#ifdef ARDUINO_ARCH_MBED
    mbed::FlashIAP flash_;
#endif
};

#endif  // __STORAGE_H__

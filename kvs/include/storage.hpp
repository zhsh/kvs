#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <string>

void fail();

class Storage {
  public:
    enum Status {
        STORAGE_OK,
        STORAGE_INVALID_ARG,
        STORAGE_STORAGE_FAILURE,
    };

    virtual bool Init() = 0;
    Status Put(const char* key, const char* value);
    std::string Get(const char* key);
    std::string List() const;

    static const uint32_t kBlockSize = 4096;
    static const char* kMagic;

  protected:
    virtual void Write(uint32_t pos, const char*buf, uint32_t length) = 0;
    virtual void Read(uint32_t pos, char* buf, uint32_t length) = 0;
    virtual void Erase(uint32_t pos, uint32_t size) = 0;

    virtual void ReadNext(uint32_t* pos, char* buf, uint32_t length) {
        Read(*pos, buf, length);
        *pos += length;
    }
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
};

#endif  // __STORAGE_H__
#include <memory>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Arduino.h>
#include <string.h>
#include "storage.hpp"
#include "storage.cpp"

class MockStorage : public Storage {
  public:
    bool Init() override {
      for (int i = 0; i < kSize; i++) {
        data_[i] = i;
      }
      return true;
    }

    int NumBlocksErased() { return erased_blocks_; }
    void Dump() {
      for (int i = 0; i < 100; i++)
        Serial.printf("%d:%d(%c) ", i, data_[i], data_[i] > 32 ? data_[i] : '?' );
      Serial.println();
    }

  protected:
    void BoundsCheck(uint32_t pos, uint32_t length) {
      if (pos + length > kSize) fail();
    }

    void Write(uint32_t pos, const char*buf, uint32_t length) {
      BoundsCheck(pos, length);
      for (uint32_t i = 0; i < length; i++) {
        // Can only reset bits without erasing
        data_[pos + i] &= buf[i];
      }
    }
    void Read(uint32_t pos, char *buf, uint32_t length) {
      BoundsCheck(pos, length);
      // Serial.printf("read %d len %d\n", pos, length);
      bcopy(data_ + pos, buf, length);
    }
    void Erase(uint32_t pos, uint32_t length) {
      BoundsCheck(pos, length);
      for(int i = 0; i < length; i++) {
        data_[pos + i] = 0xff;
      }
      erased_blocks_ += length / kBlockSize;
    }

   static const uint32_t kSize = 16384;
   char data_[kSize];
   int erased_blocks_ = 0;
};

TEST(StorageTest, Foo) {
  using testing::Eq;

  Serial.println("Start testing");

  // WTF? No unique_ptr
  auto s = new MockStorage();
  // auto s = new FlashStorage();

  s->Init();
  // Check empty read works
  EXPECT_THAT(s->Get("bar"), Eq(""));

  s->Put("zzzzzzzz", "a");
  s->Put("foo", "123");
  s->Put("bar", "456");
  EXPECT_THAT(s->Get("bar"), Eq("456"));
  EXPECT_THAT(s->Get("foo"), Eq("123"));
  EXPECT_THAT(s->Get("baz"), Eq(""));

  // Not implemented yet
  // EXPECT_THAT(s->List(), Eq("foo\nbar\n"));

  EXPECT_THAT(s->NumBlocksErased(), Eq(1));

  // Test that lazy erase works
  for (int i = 0; i < 1024; i++) {
    Serial.print(".");
    // 4 bytes * 1024
    EXPECT_THAT(s->Put("a", "b"), Eq(Storage::Status::STORAGE_OK));
  }

  EXPECT_THAT(s->Get("a"), Eq("b"));
  EXPECT_THAT(s->NumBlocksErased(), Eq(2));

  // Check overwrite
  s->Put("foo", "new value");
  EXPECT_THAT(s->Get("foo"), Eq("new value"));

  Serial.println("End testing");
  s->Dump();
  delete s;
}

void setup() {
    Serial.begin(115200);

    ::testing::InitGoogleTest();
    ::testing::InitGoogleMock();

  // Run tests
  if (RUN_ALL_TESTS())
  ;
}

void loop() {

  // sleep for 1 sec
  delay(1000);
}

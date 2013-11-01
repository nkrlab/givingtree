// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#ifndef COMMON_UUID_H_
#define COMMON_UUID_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <string>


namespace common {

class Uuid {
 public:
  static const size_t kSize = 16;
  static const Uuid kNil;

  inline static void ToBytes(const Uuid &uuid, void *buf);
  inline static Uuid FromBytes(const void *buf);

  inline static std::string ToString(const Uuid &uuid);

  size_t size() const { return kSize; }
  inline bool IsNil() const;

  uint8_t data[kSize];

 private:
  inline static char ToChar(const uint8_t &bits4);
};


void Uuid::ToBytes(const Uuid &uuid, void *buf) {
  memcpy(buf, uuid.data, uuid.size());
}


Uuid Uuid::FromBytes(const void *buf) {
  Uuid uuid;
  memcpy(uuid.data, buf, uuid.size());
  return uuid;
}


std::string Uuid::ToString(const Uuid &uuid) {
  std::string result;
  result.reserve(36);

  for (size_t index = 0; index < uuid.size(); ++index) {
    const uint8_t &b = uuid.data[index];

    const uint8_t &hi = (b >> 4) & 0x0F;
    result += ToChar(hi);
    const uint8_t &lo = b & 0x0F;
    result += ToChar(lo);

    if (index == 3 || index == 5 || index == 7 || index == 9) {
      result += '-';
    }
  }
  return result;
}


bool Uuid::IsNil() const {
  for (size_t index = 0; index < kSize; ++index) {
    if (data[index] != 0U) {
      return false;
    }
  }
  return true;
}


char Uuid::ToChar(const uint8_t &bits4) {
  if (bits4 <= 9) {
    return static_cast<char>('0' + bits4);
  } else {
    return static_cast<char>('a' + (bits4 - 10));
  }
}


inline bool operator==(const Uuid &left, const Uuid &right) {
  for (size_t index = 0; index < Uuid::kSize; ++index) {
    if (left.data[index] != right.data[index]) {
      return false;
    }
  }
  return true;
}


inline bool operator<(const Uuid &left, const Uuid &right) {
  for (size_t index = 0; index < Uuid::kSize; ++index) {
    uint8_t left_element = left.data[index];
    uint8_t right_element = right.data[index];
    if (left_element == right_element) {
      continue;
    }
    return (left_element < right_element);
  }
  return false;
}

}  // namespace common

#endif  // COMMON_UUID_H_

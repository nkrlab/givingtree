// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#ifndef COMMON_CONCURRENT_UUID_H_
#define COMMON_CONCURRENT_UUID_H_

#include <pthread.h>

#include "common/scoped_lock.h"
#include "common/types.h"


namespace common {

class ConcurrentUuid {
 public:
  static const size_t kSize = Uuid::kSize;

  ConcurrentUuid() : uuid_(Uuid::kNil) {
    pthread_mutex_init(&mutex_, NULL);
  }

  virtual ~ConcurrentUuid() {
    pthread_mutex_destroy(&mutex_);
  }

  size_t size() const {
    common::ScopedLock lock(&mutex_);
    return uuid_.size();
  }

  const Uuid &uuid() const {
    common::ScopedLock lock(&mutex_);
    return uuid_;
  }

  void set_uuid(const Uuid &uuid) {
    common::ScopedLock lock(&mutex_);
    uuid_ = uuid;
  }

 private:
  Uuid uuid_;
  mutable pthread_mutex_t mutex_;
};

}  // namespace common

#endif  // COMMON_CONCURRENT_UUID_H_

// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#ifndef COMMON_SCOPED_LOCK_H_
#define COMMON_SCOPED_LOCK_H_

#include <pthread.h>


namespace common {

class ScopedLock {
 public:
  explicit ScopedLock(pthread_mutex_t *mutex) : mutex_(mutex) {
    pthread_mutex_lock(mutex_);
  }
  virtual ~ScopedLock() {
    pthread_mutex_unlock(mutex_);
  }
 private:
  pthread_mutex_t *mutex_;
};

}  // namespace common

#endif  // COMMON_SCOPED_LOCK_H_

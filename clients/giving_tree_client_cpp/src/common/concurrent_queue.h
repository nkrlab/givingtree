// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

// Please refer to: http://www.justsoftwaresolutions.co.uk/threading/
// implementing-a-thread-safe-queue-using-condition-variables.html

#ifndef COMMON_CONCURRENT_QUEUE_H_
#define COMMON_CONCURRENT_QUEUE_H_

#include <pthread.h>

#include <queue>

#include "common/scoped_lock.h"


namespace common {

template<typename Element>
class ConcurrentQueue {
 public:
  ConcurrentQueue() : queue_() {
    pthread_mutex_init(&mutex_, NULL);
    pthread_cond_init(&condition_variable_, NULL);
  }

  virtual ~ConcurrentQueue() {
    pthread_cond_destroy(&condition_variable_);
    pthread_mutex_destroy(&mutex_);
  }

  void Push(const Element &data) {
    {
      ScopedLock lock(&mutex_);
      queue_.push(data);
    }
    pthread_cond_signal(&condition_variable_);
  }

  bool Empty() const {
    ScopedLock lock(&mutex_);
    return queue_.empty();
  }

  bool TryPop(Element *popped_value) {
    ScopedLock lock(&mutex_);
    if (queue_.empty() == true) {
      return false;
    }
    *popped_value = queue_.front();
    queue_.pop();
    return true;
  }

  void WaitAndPop(Element *popped_value) {
    ScopedLock lock(&mutex_);
    while (queue_.empty() == true) {
      pthread_cond_wait(&condition_variable_, &mutex_);
    }
    *popped_value = queue_.front();
    queue_.pop();
  }

 private:
  std::queue<Element> queue_;
  mutable pthread_mutex_t mutex_;
  mutable pthread_cond_t condition_variable_;
};

}  // namespace common

#endif  // COMMON_CONCURRENT_QUEUE_H_

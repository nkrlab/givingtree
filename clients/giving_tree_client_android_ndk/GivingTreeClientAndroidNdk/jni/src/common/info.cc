// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#include "common/info.h"

#include <android/log.h>


#define LOG_TAG "GivingTreeClient"


namespace common {

void Info::in(std::string *info) {
  // TODO(khpark): how to get std in?
}


void Info::out(const char *info) {
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "NDK:LC:INFO [%s]", info);
}


void Info::err(const char *info) {
  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "NDK:LC:ERR [%s]", info);
}

}  // namespace common

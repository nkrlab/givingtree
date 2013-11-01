// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#include "common/info.h"

#include <iostream>


namespace common {

void Info::in(std::string *info) {
  std::getline(std::cin, *info);
}


void Info::out(const char *info) {
  std::cout << info << std::endl;
}


void Info::err(const char *info) {
  std::cerr << info << std::endl;
}

}  // namespace common

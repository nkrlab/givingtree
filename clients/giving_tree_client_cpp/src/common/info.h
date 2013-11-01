// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#ifndef COMMON_INFO_H_
#define COMMON_INFO_H_

#include <stdint.h>

#include <iostream>
#include <sstream>
#include <string>


namespace common {

class Info {
 public:
  static void out(const std::string &info) {
    out(std::cout, info);
  }
  static void out(const std::stringstream &info) {
    out(info.str());
  }
  static void out() {
    out("");
  }

  static void err(const std::string &info) {
    out(std::cerr, info);
  }
  static void err(const std::stringstream &info) {
    err(info.str());
  }
  static void err() {
    err("");
  }

  static std::string str(const bool &b) {
    return ((b == true) ? "true" : "false");
  }
  static std::string str(const int64_t &n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
  }
  static std::string str(const size_t &n) {
    return str(static_cast<int64_t>(n));
  }

 private:
  static void out(std::ostream &o, const std::string &info) {
    o << info << std::endl;
  }
};

}  // namespace common


using common::Info;

#endif  // COMMON_INFO_H_

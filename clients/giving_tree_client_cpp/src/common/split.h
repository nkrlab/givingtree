// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#ifndef COMMON_SPLIT_H_
#define COMMON_SPLIT_H_

#include <ctype.h>

#include <string>
#include <vector>


namespace common {

std::vector<std::string> Split(const std::string &s) {
  std::vector<std::string> tokens;
  std::string token;
  for (std::string::const_iterator i = s.begin(); i != s.end(); ++i) {
    char c = *i;
    if (std::isspace(c) == false) {
      token += c;
    } else if (token.empty() == false) {
      tokens.push_back(token);
      token.clear();
    }
  }
  if (token.empty() == false) {
    tokens.push_back(token);
  }

  return tokens;
}

}  // namespace common

#endif  // COMMON_SPLIT_H_

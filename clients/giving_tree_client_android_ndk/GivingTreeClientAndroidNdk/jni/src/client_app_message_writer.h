// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#ifndef CLIENT_APP_MESSAGE_WRITER_H_
#define CLIENT_APP_MESSAGE_WRITER_H_

#include "app_messages.pb.h"
#include "common/types.h"


namespace giving_tree {

class ClientAppMessageWriter {
 public:
  static ::AccountMessage *CreatePlayerRegisterName(
      const string &name);

  static ::AccountMessage *CreatePlayerTakeApple();

  static ::AccountMessage *CreatePlayerGiveApples(
      const string &target_service_provider,
      const string &target_local_account,
      const int64_t &apple_count);

  static ::AccountMessage *CreateAccountSendTalk(
      const string &receiver_service_provider,
      const string &receiver_local_account,
      const string &talk);
};

}  // namespace giving_tree

#endif  // CLIENT_APP_MESSAGE_WRITER_H_

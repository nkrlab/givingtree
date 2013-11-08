// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#ifndef FUNAPI_CLIENT_MESSAGE_WRITER_H_
#define FUNAPI_CLIENT_MESSAGE_WRITER_H_

#include "funapi/account/account_messages.pb.h"

#include "common/types.h"


namespace fun {

class ClientMessageWriter {
 public:
  static ::AccountMessage *CreateAccountLoginRequest(
      const string &auth_service_provider,
      const string &local_account,
      const string &auth_key);

  static ::AccountMessage *CreateAccountLogoutRequest();

  static ::AccountMessage *CreateVersionRequest(
      const Uuid &app_uuid,
      const string &client_version);

  static ::AccountMessage *CreateSuperAccountRequest(
      const Uuid &request_uuid,
      const string &command,
      const string &parameters_json);

  static ::AccountMessage *CreateClientAppMessage();
};

}  // namespace fun

#endif  // FUNAPI_CLIENT_MESSAGE_WRITER_H_

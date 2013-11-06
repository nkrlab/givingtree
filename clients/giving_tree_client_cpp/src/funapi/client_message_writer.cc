// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#include "funapi/client_message_writer.h"


namespace fun {

::AccountMessage *ClientMessageWriter::CreateAccountLoginRequest(
    const string &auth_service_provider,
    const string &local_account,
    const string &auth_key) {
  ::AccountMessage *account_msg = new ::AccountMessage();

  account_msg->set_type(::AccountMessage::kClientAccountMessage);
  ::ClientAccountMessage *client_msg = account_msg->mutable_client_message();

  client_msg->set_type(::ClientAccountMessage::kAccountLoginRequest);
  ::AccountLoginRequest *login = client_msg->mutable_login();
  login->set_authentication_service_provider(auth_service_provider);
  login->set_local_account(local_account);
  login->set_authentication_key(auth_key);

  return account_msg;
}


::AccountMessage *ClientMessageWriter::CreateAccountLogoutRequest() {
  ::AccountMessage *account_msg = new ::AccountMessage();

  account_msg->set_type(::AccountMessage::kClientAccountMessage);
  ::ClientAccountMessage *client_msg = account_msg->mutable_client_message();

  client_msg->set_type(::ClientAccountMessage::kAccountLogoutRequest);

  return account_msg;
}


::AccountMessage *ClientMessageWriter::CreateVersionRequest(
    const Uuid &app_uuid,
    const string &client_version) {
  ::AccountMessage *account_msg = new ::AccountMessage();

  account_msg->set_type(::AccountMessage::kClientAccountMessage);
  ::ClientAccountMessage *client_msg = account_msg->mutable_client_message();

  client_msg->set_type(::ClientAccountMessage::kVersionRequest);
  ::VersionRequest *version_request = client_msg->mutable_version_request();
  version_request->set_app_uuid(app_uuid.data, app_uuid.size());
  version_request->set_client_version(client_version);

  return account_msg;
}


::AccountMessage *ClientMessageWriter::CreateSuperAccountRequest(
    const Uuid &request_uuid,
    const string &command,
    const string &parameters_json) {
  ::AccountMessage *account_msg = new ::AccountMessage();

  account_msg->set_type(::AccountMessage::kClientAccountMessage);
  ::ClientAccountMessage *client_msg = account_msg->mutable_client_message();

  client_msg->set_type(::ClientAccountMessage::kSuperAccountRequest);
  ::SuperAccountRequest *super_request = client_msg->mutable_super_request();
  super_request->set_request_uuid(request_uuid.data, request_uuid.size());
  super_request->set_command(command);
  super_request->set_parameters_json(parameters_json);

  return account_msg;
}


::AccountMessage *ClientMessageWriter::CreateClientAppMessage() {
  ::AccountMessage *account_msg = new ::AccountMessage();

  account_msg->set_type(::AccountMessage::kClientAccountMessage);
  ::ClientAccountMessage *client_msg = account_msg->mutable_client_message();

  client_msg->set_type(::ClientAccountMessage::kClientAppMessage);

  return account_msg;
}

}  // namespace fun

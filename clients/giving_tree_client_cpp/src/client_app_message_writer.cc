// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#include "client_app_message_writer.h"

#include "funapi/client_message_writer.h"


namespace giving_tree {

::AccountMessage *ClientAppMessageWriter::CreatePlayerRegisterName(
    const string &name) {
  ::AccountMessage *account_msg =
      fun::ClientMessageWriter::CreateClientAppMessage();

  ::ClientAppMessage *app_msg =
      account_msg->mutable_client_message()->mutable_app_message();
  app_msg->SetExtension(client_message_type,
                        ::ClientAppMessageType::kPlayerRegisterName);
  ::PlayerRegisterName *sub_msg =
      app_msg->MutableExtension(player_register_name);
  sub_msg->set_name(name);

  return account_msg;
}


::AccountMessage *ClientAppMessageWriter::CreatePlayerTakeApple() {
  ::AccountMessage *account_msg =
      fun::ClientMessageWriter::CreateClientAppMessage();

  ::ClientAppMessage *app_msg =
      account_msg->mutable_client_message()->mutable_app_message();
  app_msg->SetExtension(client_message_type,
                        ::ClientAppMessageType::kPlayerTakeApple);

  return account_msg;
}


::AccountMessage *ClientAppMessageWriter::CreatePlayerGiveApples(
    const string &target_service_provider,
    const string &target_local_account,
    const int64_t &apple_count) {
  ::AccountMessage *account_msg =
      fun::ClientMessageWriter::CreateClientAppMessage();

  ::ClientAppMessage *app_msg =
      account_msg->mutable_client_message()->mutable_app_message();
  app_msg->SetExtension(client_message_type,
                        ::ClientAppMessageType::kPlayerGiveApples);
  ::PlayerGiveApples *sub_msg =
      app_msg->MutableExtension(player_give_apples);
  sub_msg->set_target_service_provider(target_service_provider);
  sub_msg->set_target_local_account(target_local_account);
  sub_msg->set_apple_count(apple_count);

  return account_msg;
}


::AccountMessage *ClientAppMessageWriter::CreateAccountSendTalk(
    const string &receiver_service_provider,
    const string &receiver_local_account,
    const string &talk) {
  ::AccountMessage *account_msg =
      fun::ClientMessageWriter::CreateClientAppMessage();

  ::ClientAppMessage *app_msg =
      account_msg->mutable_client_message()->mutable_app_message();
  app_msg->SetExtension(client_message_type,
                        ::ClientAppMessageType::kAccountSendTalk);
  ::AccountSendTalk *sub_msg =
      app_msg->MutableExtension(account_send_talk);
  sub_msg->set_receiver_service_provider(receiver_service_provider);
  sub_msg->set_receiver_local_account(receiver_local_account);
  sub_msg->set_talk(talk);

  return account_msg;
}

}  // namespace giving_tree

// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#include "giving_tree_client.h"

#include <stdlib.h>

#include <vector>

#include "client_app_message_writer.h"
#include "common/info.h"
#include "common/split.h"
#include "funapi/client_message_writer.h"


namespace giving_tree {

GivingTreeClient::GivingTreeClient()
    : FunapiClient(),
      command_queue_() {
}


GivingTreeClient::~GivingTreeClient() {
}


void *GivingTreeClient::RunNetworkReceiveThread(void *args) {
  fun::FunapiClient::RunNetworkReceiveThread((fun::FunapiClient *) args);
  return NULL;
}


void GivingTreeClient::InputCommand(GivingTreeClient *client,
                                    const string &cmd_line) {
  CommandQueue *command_queue = &(client->command_queue_);
  Command cmd = common::Split(cmd_line);
  command_queue->Push(cmd);
}


void *GivingTreeClient::RunMainThread(void *args) {
  GivingTreeClient *client = (GivingTreeClient *) args;
  client->Run();
  return NULL;
}


void GivingTreeClient::Run() {
  static const int64_t kFramePeriodInMicrosec = 100 * 1000;

  Info::out("GivingTreeClient::Run() begins.");

  bool quit = false;
  while (quit == false) {
    ::AccountMessage account_msg;
    bool msg_queued = message_queue()->TryPop(&account_msg);
    if (msg_queued == true) {
      OnMessageQueued(account_msg);
    }

    Command cmd;
    bool cmd_queued = command_queue_.TryPop(&cmd);
    if (cmd_queued == true) {
      quit = OnCommandQueued(cmd);
    }

    usleep(kFramePeriodInMicrosec);
  }

  fun::FunapiClient::Close();

  Info::out("GivingTreeClient::Run() ends.");
}


void GivingTreeClient::OnMessageQueued(const ::AccountMessage &account_msg) {
  fun::FunapiClient::OnMessageQueued(account_msg);
}


bool GivingTreeClient::OnCommandQueued(const Command &cmd) {
  if (cmd.empty() == true) {
    OnEmptyCommand(cmd);
  } else {
    const string &cmd0 = cmd[0];
    if (cmd0 == "quit" || cmd0 == "q") {
      return OnQuitCommand(cmd);
    } else if (cmd0 == "login" || cmd0 == "i") {
      OnLoginCommand(cmd);
    } else if (cmd0 == "login2" || cmd0 == "i2") {
      OnLogin2Command(cmd);
    } else if (cmd0 == "logout" || cmd0 == "o") {
      OnLogoutCommand(cmd);
    } else if (cmd0 == "give" || cmd0 == "g") {
      OnGiveCommand(cmd);
    } else if (cmd0 == "talk" || cmd0 == "t") {
      OnTalkCommand(cmd);
    } else {
      OnUnknownCommand(cmd);
    }
  }
  return false;
}


void GivingTreeClient::OnAccountLoginResponse(
    const ::AccountLoginResponse &login_response) {
  bool success = (login_response.fail_code() == 0);
  Info::out("AccountLoginResponse: " + Info::str(success));
}


void GivingTreeClient::OnAccountLogoutResponse(
    const ::AccountLogoutResponse &logout_response) {
  bool success = (logout_response.fail_code() == 0);
  Info::out("AccountLogoutResponse: " + Info::str(success));
}


void GivingTreeClient::OnAccountTimeout(
    const ::AccountTimeout &timeout) {
  Info::out("AccountTimeout.");
}


void GivingTreeClient::OnAccountSessionReplaced(
    const ::AccountSessionReplaced &session_replaced) {
}


void GivingTreeClient::OnVersionResponse(
    const ::VersionResponse &version_response) {
}


void GivingTreeClient::OnSuperAccountResponse(
    const ::SuperAccountResponse &super_response) {
}


void GivingTreeClient::OnAttributeUpdatesMessage(
    const ::AttributeUpdatesMessage &attribute_updates) {
  size_t updates_size = attribute_updates.attribute_update_size();
  for (size_t index = 0; index < updates_size; ++index) {
    const ::AttributeUpdatesMessage::AttributeUpdateMessage &update =
        attribute_updates.attribute_update(index);
    string obj_uuid_str =
        Uuid::ToString(Uuid::FromBytes(update.object_uuid().c_str()));
    Info::out("AttrUpdate[" + obj_uuid_str.substr(0, 8)
              + "],[" + update.attribute_name()
              + "],[" + update.new_json()
              + "]");
  }
  Info::out();
}


void GivingTreeClient::OnServerAppMessage(
    const ::ServerAppMessage &app_message) {
  ::ServerAppMessageType::Type type =
      app_message.GetExtension(server_message_type);
  switch (type) {
    case ::ServerAppMessageType::kAccountReceiveTalk: {
      OnAccountReceiveTalk(
          app_message.GetExtension(account_receive_talk));
      break;
    }
    case ::ServerAppMessageType::kAccountSendTalkResponse: {
      OnAccountSendTalkResponse(
          app_message.GetExtension(account_send_talk_response));
      break;
    }
    default: {
      Info::err("GivingTreeClient::OnServerAppMessage: "
                "Unknown Message Type " + Info::str((int64_t) type));
      assert(false);
      break;
    }
  }
}


void GivingTreeClient::OnAccountReceiveTalk(
    const ::AccountReceiveTalk &account_receive_talk) {
  const string &sender_service_provider =
      account_receive_talk.sender_service_provider();
  const string &sender_local_account =
      account_receive_talk.sender_local_account();
  const string &talk = account_receive_talk.talk();
  const string &sender = sender_service_provider + ':' + sender_local_account;

  Info::out("Talk from [" + sender + "]: [" + talk + "]");
}


void GivingTreeClient::OnAccountSendTalkResponse(
    const ::AccountSendTalkResponse &account_send_talk_response) {
  int64_t fail_code =
      account_send_talk_response.fail_code();
  const string &fail_description =
      account_send_talk_response.fail_description();

  if (fail_code == 0) {
    Info::out("TalkSend: success.");
  } else {
    Info::out("TalkSend: error: " + fail_description);
  }
}


void GivingTreeClient::OnEmptyCommand(const Command &cmd) {
  Info::out("Command: PlayerTakeApple");

  ::AccountMessage *account_msg =
      ClientAppMessageWriter::CreatePlayerTakeApple();
  Send(*account_msg);
  delete account_msg;
  account_msg = NULL;
}


void GivingTreeClient::OnUnknownCommand(const Command &cmd) {
  bool result = VerifyCommandSize(cmd.size(), 1, "player-name");
  if (result == false) {
    return;
  }

  const string &player_name = cmd[0];

  Info::out("Command: PlayerRegisterName: " + player_name);

  ::AccountMessage *account_msg =
      ClientAppMessageWriter::CreatePlayerRegisterName(player_name);
  Send(*account_msg);
  delete account_msg;
  account_msg = NULL;
}


bool GivingTreeClient::OnQuitCommand(const Command &cmd) {
  bool result = VerifyCommandSize(cmd.size(), 1, "quit");
  if (result == false) {
    return false;
  }

  Info::out("Command: Quit.");
  return true;
}


void GivingTreeClient::OnLoginCommand(const Command &cmd) {
  bool result = VerifyCommandSize(cmd.size(), 2, "login id");
  if (result == false) {
    return;
  }

  const string &service_provider = "free";
  const string &local_account = cmd[1];
  const string &auth_key = "";

  Info::out("Command: AccountLoginRequest: " +
            service_provider + ":" + local_account);

  ::AccountMessage *account_msg =
      fun::ClientMessageWriter::CreateAccountLoginRequest(
          service_provider, local_account, auth_key);
  Send(*account_msg);
  delete account_msg;
  account_msg = NULL;
}


void GivingTreeClient::OnLogin2Command(const Command &cmd) {
  bool result = VerifyCommandSize(
      cmd.size(), 4, "login2 service-provider id key");
  if (result == false) {
    return;
  }

  const string &service_provider = cmd[1];
  const string &local_account = cmd[2];
  const string &auth_key = cmd[3];

  Info::out("Command: AccountLoginRequest: " +
            service_provider + ":" + local_account);

  ::AccountMessage *account_msg =
      fun::ClientMessageWriter::CreateAccountLoginRequest(
          service_provider, local_account, auth_key);
  Send(*account_msg);
  delete account_msg;
  account_msg = NULL;
}


void GivingTreeClient::OnLogoutCommand(const Command &cmd) {
  bool result = VerifyCommandSize(cmd.size(), 1, "logout");
  if (result == false) {
    return;
  }

  Info::out("Command: AccountLogoutRequest.");

  ::AccountMessage *account_msg =
      fun::ClientMessageWriter::CreateAccountLogoutRequest();
  Send(*account_msg);
  delete account_msg;
  account_msg = NULL;
}


void GivingTreeClient::OnGiveCommand(const Command &cmd) {
  bool result = VerifyCommandSize(cmd.size(), 3, "give id 1");
  if (result == false) {
    return;
  }

  const string &target_id = cmd[1];
  int64_t apple_count = atoi(cmd[2].c_str());

  Info::out("Command: PlayerGiveApples: to: " +
            target_id + ", count: " + Info::str(apple_count));

  ::AccountMessage *account_msg =
      ClientAppMessageWriter::CreatePlayerGiveApples(
          "free", target_id, apple_count);
  Send(*account_msg);
  delete account_msg;
  account_msg = NULL;
}


void GivingTreeClient::OnTalkCommand(const Command &cmd) {
  bool result = VerifyCommandSize(cmd.size(), 3, "talk id message");
  if (result == false) {
    return;
  }

  const string &receiver_id = cmd[1];
  const string &talk = cmd[2];

  Info::out("Command: AccountSendTalk: to: " +
            receiver_id + ", talk: " + talk);

  ::AccountMessage *account_msg =
      ClientAppMessageWriter::CreateAccountSendTalk(
          "free", receiver_id, talk);
  Send(*account_msg);
  delete account_msg;
  account_msg = NULL;
}


bool GivingTreeClient::VerifyCommandSize(
    const size_t &input_size,
    const size_t &expected_size,
    const string &cmd_example) {
  if (input_size != expected_size) {
    Info::out("error: cmd.size() != " + Info::str(expected_size)
              + " : " + Info::str(input_size));
    Info::out("example> " + cmd_example);
    return false;
  }
  return true;
}

}  // namespace giving_tree

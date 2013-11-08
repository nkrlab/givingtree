// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#ifndef GIVING_TREE_CLIENT_H_
#define GIVING_TREE_CLIENT_H_

#include <vector>

#include "common/concurrent_queue.h"
#include "common/types.h"
#include "funapi/funapi_client.h"
#include "giving_tree_server_messages.pb.h"


namespace giving_tree {

class GivingTreeClient : public fun::FunapiClient {
 public:
  typedef std::vector<string> Command;
  typedef common::ConcurrentQueue<Command> CommandQueue;

  GivingTreeClient();
  virtual ~GivingTreeClient();

  static void *RunNetworkReceiveThread(void *args);
  static void InputCommand(GivingTreeClient *client, const string &cmd_line);
  static void *RunMainThread(void *args);

  void Run();

  void OnMessageQueued(const ::AccountMessage &account_msg);
  bool OnCommandQueued(const Command &cmd);

  virtual void OnAccountLoginResponse(
      const ::AccountLoginResponse &login_response);
  virtual void OnAccountLogoutResponse(
      const ::AccountLogoutResponse &logout_response);
  virtual void OnAccountTimeout(
      const ::AccountTimeout &timeout);
  virtual void OnAccountSessionReplaced(
      const ::AccountSessionReplaced &session_replaced);
  virtual void OnVersionResponse(
      const ::VersionResponse &version_response);
  virtual void OnSuperAccountResponse(
      const ::SuperAccountResponse &super_response);
  virtual void OnAttributeUpdatesMessage(
      const ::AttributeUpdatesMessage &attribute_updates);
  virtual void OnServerAppMessage(
      const ::ServerAppMessage &app_message);

  void OnAccountReceiveTalk(
      const ::AccountReceiveTalk &account_receive_talk);
  void OnAccountSendTalkResponse(
      const ::AccountSendTalkResponse &account_send_talk_response);

 private:
  void OnEmptyCommand(const Command &cmd);
  void OnUnknownCommand(const Command &cmd);
  bool OnQuitCommand(const Command &cmd);
  void OnLoginCommand(const Command &cmd);
  void OnLogin2Command(const Command &cmd);
  void OnLogoutCommand(const Command &cmd);
  void OnGiveCommand(const Command &cmd);
  void OnTalkCommand(const Command &cmd);

  static bool VerifyCommandSize(
      const size_t &input_size,
      const size_t &expected_size,
      const string &cmd_example);

  CommandQueue command_queue_;
};

}  // namespace giving_tree

#endif  // GIVING_TREE_CLIENT_H_

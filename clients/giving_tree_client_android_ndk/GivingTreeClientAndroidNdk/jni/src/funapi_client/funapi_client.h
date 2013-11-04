// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#ifndef FUNAPI_CLIENT_FUNAPI_CLIENT_H_
#define FUNAPI_CLIENT_FUNAPI_CLIENT_H_

#include <sys/socket.h>

#include "common/concurrent_queue.h"
#include "common/concurrent_uuid.h"
#include "common/types.h"
#include "funapi_client/account_messages.pb.h"


namespace fun {

class FunapiClient {
 public:
  typedef common::ConcurrentQueue<string> MessageQueue;
  typedef common::ConcurrentUuid SessionId;

  FunapiClient();
  virtual ~FunapiClient();

  static void RunNetworkReceiveThread(FunapiClient *funapi_client);

  int socket_fd() const {
    return socket_fd_;
  }

  MessageQueue *message_queue() {
    return &message_queue_;
  }

  SessionId *session_id() {
    return &session_id_;
  }

  bool Connect(const char *host_name, const uint16_t &port_number);
  void Close();
  void Send(const ::AccountMessage &message);

  void OnMessageQueued(const ::AccountMessage &account_msg);

  virtual void OnAccountLoginResponse(
      const ::AccountLoginResponse &/*login_response*/) {}
  virtual void OnAccountLogoutResponse(
      const ::AccountLogoutResponse &/*logout_response*/) {}
  virtual void OnAccountTimeout(
      const ::AccountTimeout &/*timeout*/) {}
  virtual void OnAccountSessionReplaced(
      const ::AccountSessionReplaced &/*session_replaced*/) {}
  virtual void OnVersionResponse(
      const ::VersionResponse &/*version_response*/) {}
  virtual void OnSuperAccountResponse(
      const ::SuperAccountResponse &/*super_response*/) {}
  virtual void OnAttributeUpdatesMessage(
      const ::AttributeUpdatesMessage &/*attribute_updates*/) {}
  virtual void OnServerAppMessage(
      const ::ServerAppMessage &/*app_message*/) {}

  virtual void OnMaintenanceAccountMessage(
      const ::MaintenanceAccountMessage &/*maintenance_msg*/) {}

 private:
  static const size_t kHeaderLength = 4;
  static const size_t kBufferSize = 65536;

  static bool ReceiveUntil(int socket_fd, size_t recv_size, uint8_t *buf,
                           size_t *offset);
  static void ConsumeReceived(size_t consume_size, uint8_t *buf,
                              size_t *offset);

  static void EncodeMessageSize(const size_t &msg_size, uint8_t *buf);
  static size_t DecodeMessageSize(const uint8_t *buf);

  void OnServerAccountMessage(
      const ::ServerAccountMessage &server_msg);

  int socket_fd_;
  MessageQueue message_queue_;
  SessionId session_id_;
};

}  // namespace fun

#endif  // FUNAPI_CLIENT_FUNAPI_CLIENT_H_

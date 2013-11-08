// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#include "funapi/funapi_client.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/info.h"


namespace fun {

FunapiClient::FunapiClient()
    : socket_fd_(socket(AF_INET, SOCK_STREAM, 0)),
      message_queue_(),
      session_id_() {
  assert(socket_fd_ >= 0);
}


FunapiClient::~FunapiClient() {
}


void FunapiClient::RunNetworkReceiveThread(FunapiClient *funapi_client) {
  int socket_fd = funapi_client->socket_fd();
  MessageQueue *message_queue = funapi_client->message_queue();
  SessionId *session_id = funapi_client->session_id();

  uint8_t buf[kBufferSize];
  size_t offset = 0;

  while (true) {
    // Reads message header(i.e., length of message body).
    bool result0 = ReceiveUntil(socket_fd,
                                kHeaderLength,
                                buf,
                                &offset);
    if (result0 == false) {
      break;
    }

    // Decodes length of message body.
    size_t body_len = DecodeMessageSize(buf);
    if (body_len < SessionId::kSize) {
      Info::err("RunNetworkReceiveThread: invalid body_len: " +
                Info::str(body_len));
      break;
    }

    // Reads message body.
    bool result1 = ReceiveUntil(socket_fd,
                                kHeaderLength + body_len,
                                buf,
                                &offset);
    if (result1 == false) {
      break;
    }

    // Decodes SessionId.
    Uuid uuid = Uuid::FromBytes(buf + kHeaderLength);
    assert(uuid.IsNil() == false);
    session_id->set_uuid(uuid);

    // Decodes AccountMessage.
    ::AccountMessage account_msg;
    size_t session_id_len = session_id->size();
    bool parsed = account_msg.ParseFromArray(
        buf + kHeaderLength + session_id_len,
        body_len - session_id_len);
    if (parsed == false) {
      Info::err("GivingTreeClient::Run(): failed to parse message.");
      break;
    }

    // Pushes message into message queue.
    message_queue->Push(account_msg);

    // Consumes used buffer.
    ConsumeReceived(kHeaderLength + body_len, buf, &offset);
  }
}


bool FunapiClient::Connect(const char *host_name,
                           const uint16_t &port_number) {
  struct sockaddr_in server_addr;
  {
    struct hostent *server = gethostbyname(host_name);
    if (server == NULL) {
      Info::err("FunapiClient::Connect: server == NULLL: " +
                std::string(host_name));
      return false;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr,
           server->h_addr,
           server->h_length);
    server_addr.sin_port = htons(port_number);
  }

  int result = connect(socket_fd_,
                       (struct sockaddr *) &server_addr,
                       sizeof(server_addr));
  if (result < 0) {
    Info::err("FunapiClient::Connect: connection failed: " +
              Info::str((int64_t) result));
    return false;
  }

  return true;
}


void FunapiClient::Close() {
  shutdown(socket_fd_, SHUT_RDWR);
  close(socket_fd_);
}


void FunapiClient::Send(const ::AccountMessage &message) {
  size_t msg_len = message.ByteSize();
  size_t session_id_len = session_id_.size();
  size_t body_len = session_id_len + msg_len;

  uint8_t buf[kBufferSize];
  size_t buf_size = 0;

  EncodeMessageSize(body_len, buf + buf_size);
  buf_size += kHeaderLength;

  Uuid::ToBytes(session_id_.uuid(), buf + buf_size);
  buf_size += session_id_len;

  message.SerializeToArray(buf + buf_size, msg_len);
  buf_size += msg_len;

  send(socket_fd_, buf, buf_size, 0);
}


void FunapiClient::OnMessageQueued(const ::AccountMessage &account_msg) {
  switch (account_msg.type()) {
    case ::AccountMessage::kServerAccountMessage: {
      assert(account_msg.has_server_message() == true);
      OnServerAccountMessage(account_msg.server_message());
      break;
    }
    case ::AccountMessage::kMaintenanceAccountMessage: {
      assert(account_msg.has_maintenance_message() == true);
      OnMaintenanceAccountMessage(account_msg.maintenance_message());
      break;
    }
    default: {
      assert(false);
    }
  }
}


bool FunapiClient::ReceiveUntil(
    int socket_fd,
    size_t recv_size,
    uint8_t *buf,
    size_t *offset) {
  while (*offset < recv_size) {
    int n = recv(socket_fd, buf + *offset, kBufferSize - *offset, 0);
    if (n <= 0) {
      fprintf(stderr, "FunapiClient::ReceiveUntil: "
                      "recv_size: %d, err_no: %d\n", n, errno);
      return false;
    }
    *offset += n;
  }
  return true;
}


void FunapiClient::ConsumeReceived(
    size_t consume_size,
    uint8_t *buf,
    size_t *offset) {
  assert(*offset >= consume_size);
  *offset -= consume_size;
  memcpy(buf, buf + consume_size, *offset);
}


void FunapiClient::EncodeMessageSize(const size_t &msg_size, uint8_t *buf) {
  uint32_t n = htonl(msg_size);
  assert(sizeof(n) == kHeaderLength);
  memcpy(buf, &n, kHeaderLength);
}


size_t FunapiClient::DecodeMessageSize(const uint8_t *buf) {
  uint32_t len_buf = 0;
  assert(sizeof(len_buf) == kHeaderLength);
  memcpy(&len_buf, buf, kHeaderLength);
  return ntohl(len_buf);
}


void FunapiClient::OnServerAccountMessage(
    const ::ServerAccountMessage &server_msg) {
  switch (server_msg.type()) {
    case ::ServerAccountMessage::kAccountLoginResponse: {
      assert(server_msg.has_login() == true);
      OnAccountLoginResponse(server_msg.login());
      break;
    }
    case ::ServerAccountMessage::kAccountLogoutResponse: {
      assert(server_msg.has_logout() == true);
      OnAccountLogoutResponse(server_msg.logout());
      break;
    }
    case ::ServerAccountMessage::kAccountTimeout: {
      assert(server_msg.has_timeout() == true);
      OnAccountTimeout(server_msg.timeout());
      break;
    }
    case ::ServerAccountMessage::kAccountSessionReplaced: {
      assert(server_msg.has_session_replaced() == true);
      OnAccountSessionReplaced(server_msg.session_replaced());
      break;
    }
    case ::ServerAccountMessage::kVersionResponse: {
      assert(server_msg.has_version_response() == true);
      OnVersionResponse(server_msg.version_response());
      break;
    }
    case ::ServerAccountMessage::kSuperAccountResponse: {
      assert(server_msg.has_super_response() == true);
      OnSuperAccountResponse(server_msg.super_response());
      break;
    }
    case ::ServerAccountMessage::kAttributeUpdatesMessage: {
      assert(server_msg.has_attribute_updates() == true);
      OnAttributeUpdatesMessage(server_msg.attribute_updates());
      break;
    }
    case ::ServerAccountMessage::kServerAppMessage: {
      assert(server_msg.has_app_message() == true);
      OnServerAppMessage(server_msg.app_message());
      break;
    }
    default: {
      assert(false);
    }
  }
}

}  // namespace fun

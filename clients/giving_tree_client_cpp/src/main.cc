// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#include <assert.h>
#include <pthread.h>

#include "common/info.h"
#include "common/types.h"
#include "giving_tree_client.h"


int main() {
  static const char *kRemoteIpAddress = "127.0.0.1";
  static const uint16_t kRemotePort = 8012;

  giving_tree::GivingTreeClient client;
  bool connected = client.Connect(kRemoteIpAddress, kRemotePort);
  assert(connected == true);

  Info::out("main: net_recv_thread starts.");
  pthread_t net_recv_thread;
  {
    int result = pthread_create(
        &net_recv_thread,
        NULL,
        &giving_tree::GivingTreeClient::RunNetworkReceiveThread,
        (void *) &client);
    assert(result == 0);
  }

  Info::out("main: key_input_thread starts.");
  pthread_t key_input_thread;
  {
    int result = pthread_create(
        &key_input_thread,
        NULL,
        &giving_tree::GivingTreeClient::RunKeyInputThread,
        (void *) &client);
    assert(result == 0);
  }

  Info::out("main: GivingTreeClient starts.");
  client.Run();
  Info::out("main: GivingTreeClient ends.");

  pthread_join(key_input_thread, NULL);
  Info::out("main: key_input_thread joins.");

  pthread_join(net_recv_thread, NULL);
  Info::out("main: net_recv_thread joins.");

  return 0;
}

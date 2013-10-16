// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#include <assert.h>
#include <pthread.h>

#include "common/types.h"
#include "giving_tree_client.h"

#include <iostream>


int main() {
  static const char *kRemoteIpAddress = "127.0.0.1";
  static const uint16_t kRemotePort = 8012;

  giving_tree::GivingTreeClient client;
  bool connected = client.Connect(kRemoteIpAddress, kRemotePort);
  assert(connected == true);

  std::cout << "main: net_recv_thread starts." << std::endl;
  pthread_t net_recv_thread;
  {
    int result = pthread_create(
        &net_recv_thread,
        NULL,
        &giving_tree::GivingTreeClient::RunNetworkReceiveThread,
        (void *) &client);
    assert(result == 0);
  }

  std::cout << "main: key_input_thread starts." << std::endl;
  pthread_t key_input_thread;
  {
    int result = pthread_create(
        &key_input_thread,
        NULL,
        &giving_tree::GivingTreeClient::RunKeyInputThread,
        (void *) &client);
    assert(result == 0);
  }

  std::cout << "main: GivingTreeClient starts." << std::endl;
  client.Run();
  std::cout << "main: GivingTreeClient ends." << std::endl;

  pthread_join(key_input_thread, NULL);
  std::cout << "main: key_input_thread joins." << std::endl;

  pthread_join(net_recv_thread, NULL);
  std::cout << "main: net_recv_thread joins." << std::endl;

  return 0;
}

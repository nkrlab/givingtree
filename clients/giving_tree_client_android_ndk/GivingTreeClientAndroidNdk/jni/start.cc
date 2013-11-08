#include <jni.h>
#include <string.h>
#include <google/protobuf/message.h>

#include <assert.h>
#include <pthread.h>

#include "common/info.h"
#include "common/types.h"
#include "giving_tree_client.h"


#ifdef __cplusplus
extern "C" {
#endif

giving_tree::GivingTreeClient the_client;

void Java_com_ifunfactory_givingtreeclientandroidndk_MainActivity_start(
    JNIEnv *env, jobject thiz) {
  static const char *kRemoteIpAddress = "10.10.1.8";
  static const uint16_t kRemotePort = 8012;

  bool connected = the_client.Connect(kRemoteIpAddress, kRemotePort);
  if (connected == false) {
    Info::err("main: failed to connect to remote server: " +
              std::string(kRemoteIpAddress) + ":" +
              Info::str((int64_t) kRemotePort));
    exit(-1);
    return;
  }

  Info::out("main: net_recv_thread starts.");
  pthread_t net_recv_thread;
  {
    int result = pthread_create(
        &net_recv_thread,
        NULL,
        &giving_tree::GivingTreeClient::RunNetworkReceiveThread,
        (void *) &the_client);
    assert(result == 0);
  }

  Info::out("main: main_thread starts.");
  pthread_t main_thread;
  {
    int result = pthread_create(
        &main_thread,
        NULL,
        &giving_tree::GivingTreeClient::RunMainThread,
        (void *) &the_client);
    assert(result == 0);
  }
}


void Java_com_ifunfactory_givingtreeclientandroidndk_MainActivity_command(
    JNIEnv *env, jobject thiz, jstring jcmd) {
  jboolean isCopy;

  const char *szCmd = env->GetStringUTFChars(jcmd, &isCopy);
  std::string cmd_line(szCmd);
  Info::out("command line: " + cmd_line);
  giving_tree::GivingTreeClient::InputCommand(&the_client, cmd_line);

  env->ReleaseStringUTFChars(jcmd, szCmd);
}


void Java_com_ifunfactory_givingtreeclientandroidndk_MainActivity_log(
    JNIEnv *env, jobject thiz, jstring logThis) {
  jboolean isCopy;

  const char *szLogThis = env->GetStringUTFChars(logThis, &isCopy);
  Info::out(szLogThis);

  env->ReleaseStringUTFChars(logThis, szLogThis);
}

#ifdef __cplusplus
}
#endif

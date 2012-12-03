// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include "funapi_event_handlers.h"

#include <funapi/account/account.h>
#include <funapi/account/multicaster.h>
#include <funapi/api/clock.h>
#include <funapi/common/serialization/bson_archive.h>
#include <funapi/object/object.h>
#include <funapi/system/logging.h>

#include "giving_tree_event_handlers.h"
#include "giving_tree_types.h"


namespace giving_tree {

const int64_t kWorldTickMicrosecond = 1000000;  // 1 second.
const fun::string kWorldObjectModelName("GivingTree");
const fun::string kAccountObjectModelName("Player");

const fun::string kRoomChannelName("room");
const fun::string kRoomChannelSubId("1");


void OnWorldReady(int64_t /*now_nanosec*/) {
  the_world = GivingTree::CreateNew(kWorldObjectModelName);
  InitializeWorld();
  the_world->EnterChannel(kRoomChannelName, kRoomChannelSubId);
}


void OnWorldTick(int64_t /*now_nanosec*/) {
  TickWorld();
}


fun::Object::Ptr CreateObject(const fun::string &model) {
  return giving_tree::GivingTree::CreateNew(model);
}


fun::Object::Ptr DeserializeObject(const fun::string &serial) {
  fun::BsonArchive::Ptr archive_ptr =
      fun::BsonArchive::CreateFromSerialized(serial);
  return giving_tree::GivingTree::CreateFromSerialized(*archive_ptr);
}


void OnAccountLogin(const fun::Account::Ptr &account) {
  GivingTreePtr player = GivingTree::Cast(account->object());
  const string &player_name = player->name();
  fun::Multicaster::Get().EnterChannel(kRoomChannelName, kRoomChannelSubId,
                                       account);

  FUN_LOG_INFO << "account login[" << account->account_id()
               << "] player name[" << player_name
               << "] apple count[" << player->apple_count()
               << "]";
}


void OnAccountLogout(const fun::Account::Ptr &account) {
  GivingTreePtr player = GivingTree::Cast(account->object());
  const string &player_name = player->name();
  ErasePlayer(player_name);
  fun::Multicaster::Get().LeaveChannel(kRoomChannelName, kRoomChannelSubId,
                                       account);

  FUN_LOG_INFO << "account logout[" << account->account_id()
               << "] player name[" << player_name
               << "] apple count[" << player->apple_count()
               << "]";
}


void OnAccountTimeout(const fun::Account::Ptr &account) {
  OnAccountLogout(account);
}


void OnAccountMessage(const fun::Account::Ptr &account,
                      const ::ClientAppMessage &msg) {
  GivingTreePtr player = GivingTree::Cast(account->object());

  ::ClientAppMessageType::Type msg_type = msg.GetExtension(client_message_type);
  switch (msg_type) {
    case ::ClientAppMessageType::kPlayerRegisterName: {
      OnPlayerRegisterName(player, msg.GetExtension(player_register_name));
      break;
    }
    case ::ClientAppMessageType::kPlayerTakeApple: {
      OnPlayerTakeApple(player, msg.GetExtension(player_take_apple));
      break;
    }
    default: {
      FUN_LOG_ERR << "Unknown client message type: " << (int64_t) msg_type;
      break;
    }
  }
}

}  // namespace giving_tree

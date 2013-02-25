// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include "event_handlers.h"

#include <boost/foreach.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <funapi/account/account.h>
#include <funapi/account/multicaster.h>
#include <funapi/api/clock.h>
#include <funapi/common/boost_util.h>
#include <funapi/common/serialization/bson_archive.h>
#include <funapi/object/object.h>
#include <glog/logging.h>

#include <algorithm>

#include "giving_tree_loggers.h"
#include "giving_tree_types.h"
#include "giving_tree.h"


namespace giving_tree {

const char *kWorldObjectModelName = "GivingTree";
const char *kAccountObjectModelName = "Player";


fun::Object::Ptr CreateObject(const string &model) {
  return giving_tree::GivingTree::CreateNew(model);
}


fun::Object::Ptr DeserializeObject(const string &serial) {
  fun::BsonArchive::Ptr archive_ptr =
      fun::BsonArchive::CreateFromSerialized(serial);
  return giving_tree::GivingTree::CreateFromSerialized(*archive_ptr);
}


const int64_t kWorldTickMicrosecond = 1000000;  // 1 second.


void OnWorldReady(int64_t /*now_microsec*/) {
  the_world = GivingTree::CreateNew(kWorldObjectModelName);
  InitializeWorld();
  the_world->EnterChannel(kRoomChannelName, kRoomChannelSubId);
}


void OnWorldTick(int64_t /*now_microsec*/) {
  TickWorld();
  logger::Tick(time(NULL), the_world, "Comment containting \" quote and \n newline");
}


void OnAccountLogin(const fun::Account::Ptr &account) {
  GivingTreePtr player = GivingTree::Cast(account->object());
  const string &player_name = player->name();
  fun::Multicaster::Get().EnterChannel(kRoomChannelName, kRoomChannelSubId,
                                       account);

  LOG(INFO) << "account login[" << account->account_id()
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

  LOG(INFO) << "account logout[" << account->account_id()
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
    case ::ClientAppMessageType::kPlayerGiveApples: {
      OnPlayerGiveApples(player, msg.GetExtension(player_give_apples));
      break;
    }
    case ::ClientAppMessageType::kAccountSendTalk: {
      OnAccountSendTalk(account, msg.GetExtension(account_send_talk));
      break;
    }
    default: {
      LOG(ERROR) << "Unknown client message type: " << (int64_t) msg_type;
      break;
    }
  }
}


const int64_t kCountDownStart = 10;

const char *kRoomChannelName = "room";
const char *kRoomChannelSubId = "1";


GivingTreePtr the_world;


void InitializeWorld() {
  ResetWorld();
}


void TickWorld() {
  int64_t count_down = CountDown();
  LOG(INFO) << "TickWorld: CountDown: " << count_down;

  if (count_down <= 0) {
    DropApple();
    LOG(INFO) << "TickWorld: DropApple: winner: ["
              << the_world->winner_name() << "]";

    ResetWorld();
  }
}


GivingTreePtr FindPlayer(const string &player_name) {
  return the_world->players().Find(player_name);
}


void InsertPlayer(const GivingTreePtr &player) {
  GivingTreePtrMap players = the_world->players();
  players.Insert(player->name(), player);
  the_world->set_players(players);
}


void ErasePlayer(const string &player_name) {
  GivingTreePtrMap players = the_world->players();
  players.erase(player_name);
  the_world->set_players(players);
}


void OnPlayerRegisterName(const GivingTreePtr &player,
                          const ::PlayerRegisterName &msg) {
  const string &new_name = msg.name();
  if (new_name == "(no name)" || new_name == "") {
    LOG(INFO) << "OnPlayerRegisterName: Fail: invalid player name: ["
              << new_name << "]";
    return;
  }

  string prev_name = player->name();
  if (new_name != prev_name) {
    ErasePlayer(prev_name);
    player->set_name(new_name);
    LOG(INFO) << "OnPlayerRegisterName["
              << prev_name << "]: ["
              << new_name << "]";
  }

  InsertPlayer(player);
  LOG(INFO) << "player entered: " << new_name;
}


void OnPlayerTakeApple(const GivingTreePtr &player,
                       const ::PlayerTakeApple &/*msg*/) {
  int64_t now_microsec = fun::MonotonicClock::Now();
  LOG(INFO) << "OnPlayerTakeApple: [" << player->name() << "]: "
            << now_microsec;

  player->set_bet_microsec(now_microsec);
}


void OnPlayerGiveApples(const GivingTreePtr &player,
                        const ::PlayerGiveApples &msg) {
  const string &service_provider = msg.target_service_provider();
  const string &local_account = msg.target_local_account();
  fun::AccountId target_id(service_provider, local_account);
  const int64_t &apple_count = msg.apple_count();

  LOG(INFO) << "OnPlayerGiveApples: [" << player->name()
            << "]->[" << target_id
            << "]: apples[" << apple_count
            << "].";

  GivingTreePtr target_player =
      GivingTree::Cast(fun::Account::FindAccountObject(target_id));
  if (not target_player) {
    LOG(INFO) << "OnPlayerGiveApples: target player doesn't exist: ["
              << target_id << "].";
    return;
  }

  GiveApples(player, target_player, apple_count);
}


void OnAccountSendTalk(const fun::Account::Ptr &account,
                       const ::AccountSendTalk &msg) {
  const fun::AccountId &sender_id = account->account_id();
  const string &service_provider = msg.receiver_service_provider();
  const string &local_account = msg.receiver_local_account();
  fun::AccountId receiver_id(service_provider, local_account);
  const string &talk = msg.talk();

  LOG(INFO) << "OnAccountSendTalk: [" << sender_id
            << "]->[" << receiver_id
            << "]: talk[" << talk
            << "].";

  bool send_result = false;
  {
    ::ServerAppMessage app_msg;
    app_msg.SetExtension(server_message_type,
                         ::ServerAppMessageType::kAccountReceiveTalk);
    ::AccountReceiveTalk *talk_msg =
        app_msg.MutableExtension(account_receive_talk);
    talk_msg->set_sender_service_provider(sender_id.service_provider());
    talk_msg->set_sender_local_account(sender_id.local_account());
    talk_msg->set_talk(talk);

    send_result = fun::Account::SendServerAppMessage(receiver_id, app_msg);
  }

  {
    int64_t fail_code = 0;
    string fail_description("");
    if (send_result == false) {
      fail_code = 1;
      fail_description = "Fail: account not logged in: " +
                         receiver_id.ToString();
    }

    ::ServerAppMessage app_msg;
    app_msg.SetExtension(server_message_type,
                         ::ServerAppMessageType::kAccountSendTalkResponse);
    ::AccountSendTalkResponse *response =
        app_msg.MutableExtension(account_send_talk_response);
    response->set_fail_code(fail_code);
    response->set_fail_description(fail_description);
    account->SendMessage(app_msg);
  }
}


void ResetWorld() {
  the_world->set_count_down(kCountDownStart);
  ResetPlayerBets(the_world->players());
}


int64_t CountDown() {
  int64_t count_down = the_world->count_down();
  --count_down;
  the_world->set_count_down(count_down);
  return count_down;
}


void DropApple() {
  const GivingTreePtrMap &players = the_world->players();
  GivingTreePtr winner = SelectWinner(players);
  string winner_name;
  if (winner) {
    GrantApple(winner);
    winner_name = winner->name();
  }
  the_world->set_winner_name(winner_name);
}


void ResetPlayerBets(const GivingTreePtrMap &players) {
  BOOST_FOREACH(const GivingTreePtrMap::value_type &element, players) {
    // const string &player_name = element.first;
    GivingTreePtr player = GivingTree::Cast(element.second);
    player->set_bet_microsec(0);
  }
}


GivingTreePtr SelectWinner(const GivingTreePtrMap &players) {
  GivingTreePtr winner;
  int64_t winner_microsec = 0;

  BOOST_FOREACH(const GivingTreePtrMap::value_type &element, players) {
    // const string &player_name = element.first;
    GivingTreePtr player = GivingTree::Cast(element.second);
    int64_t bet_microsec = player->bet_microsec();
    if (bet_microsec > winner_microsec) {
      winner = player;
      winner_microsec = bet_microsec;
    }
  }

  return winner;
}


void GrantApple(const GivingTreePtr &player) {
  int64_t apple_count = player->apple_count();
  ++apple_count;
  player->set_apple_count(apple_count);
}


void GiveApples(const GivingTreePtr &giver, const GivingTreePtr &taker,
                const int64_t &apple_count) {
  int64_t giver_count = giver->apple_count();
  int64_t taker_count = taker->apple_count();
  int64_t target_count = std::min(apple_count, giver_count);

  giver_count -= target_count;
  taker_count += target_count;

  giver->set_apple_count(giver_count);
  taker->set_apple_count(taker_count);

  LOG(INFO) << "GiveApples: [" << giver->name() << "](" << giver_count
            << ")->[" << taker->name() << "](" << taker_count
            << "): apples[" << target_count << " / " << apple_count
            << "].";
}

}  // namespace giving_tree

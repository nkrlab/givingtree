// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#include "event_handlers.h"

#include <boost/foreach.hpp>
#include <funapi/account/multicaster.h>
#include <funapi/api/clock.h>
#include <funapi/common/boost_util.h>
#include <funapi/common/serialization/bson_archive.h>
#include <funapi/world/world.h>
#include <glog/logging.h>

#include <algorithm>

#include "giving_tree.h"
#include "giving_tree_loggers.h"


namespace giving_tree {

const char *kWorldObjectModelName = "GivingTree";
const char *kAccountObjectModelName = "Player";


fun::Object::Ptr CreateObject(const string &model) {
  return GivingTree::CreateNew(model);
}


fun::Object::Ptr DeserializeObject(const string &serial) {
  fun::BsonArchive::Ptr archive_ptr =
      fun::BsonArchive::CreateFromSerialized(serial);
  return GivingTree::CreateFromSerialized(*archive_ptr);
}


const int64_t kWorldTickMicrosecond = 1000000;  // 1 second.


void OnWorldReady() {
  GivingTreePtr world = GivingTree::Cast(fun::World::Get().object());
  Initialize(world);
  world->EnterChannel(kRoomChannelName, kRoomChannelSubId);
}


void OnWorldTick() {
  GivingTreePtr world = GivingTree::Cast(fun::World::Get().object());
  Tick(world);
}


void OnAccountLogin(const fun::Account::Ptr &account) {
  const fun::AccountId &account_id = account->account_id();
  GivingTreePtr player = GivingTree::Cast(account->object());

  player->set_local_account(account_id.local_account());

  fun::Multicaster::Get().EnterChannel(kRoomChannelName, kRoomChannelSubId,
                                       account);

  logger::AccountLoginApp(account_id.ToString(),
                          player->name(),
                          player->apple_count());
}


void OnAccountLogout(const fun::Account::Ptr &account) {
  GivingTreePtr world = GivingTree::Cast(fun::World::Get().object());
  const fun::AccountId &account_id = account->account_id();
  GivingTreePtr player = GivingTree::Cast(account->object());

  const string &player_name = player->name();
  ErasePlayer(world, player_name);

  fun::Multicaster::Get().LeaveChannel(kRoomChannelName, kRoomChannelSubId,
                                       account);

  logger::AccountLogoutApp(account_id.ToString(),
                           player_name,
                           player->apple_count());
}


void OnAccountTimeout(const fun::Account::Ptr &account) {
  OnAccountLogout(account);
}


void OnSuperAccountRequest(const fun::Account::Ptr &account,
                           const fun::Uuid &request_uuid,
                           const string &command,
                           const json_spirit::mObject &parameters) {
  GivingTreePtr world = GivingTree::Cast(fun::World::Get().object());
  const fun::AccountId &account_id = account->account_id();

  DLOG(INFO) << "OnSuperAccountRequest: " << account_id
             << ": " << command << json_spirit::write(parameters);

  if (command == "GiveApples") {
    json_spirit::mObject::const_iterator i;
    string player_name;
    i = parameters.find("player_name");
    if (i != parameters.end()) {
      player_name = i->second.get_str();
    }
    int64_t apple_count = 0;
    i = parameters.find("apple_count");
    if (i != parameters.end()) {
      apple_count = i->second.get_int();
    }

    GivingTreePtr player = FindPlayer(world, player_name);
    if (player) {
      int64_t sum_count = player->apple_count() + apple_count;
      player->set_apple_count(sum_count);

      logger::SuperAccountGiveApples(account_id.ToString(),
                                     player_name,
                                     apple_count,
                                     sum_count);

      logger::CharacterItemHistory(
          player->local_account(),
          "0",
          player->name(),
          1,
          4,
          player->name(),
          "(supper command)" + account_id.local_account(),
          "GoldenApple",
          "10001",
          apple_count,
          sum_count - apple_count,
          player->apple_count(),
          "SuperCommand");

      json_spirit::Object result;
      result.push_back(json_spirit::Pair("sum_count", sum_count));
      account->RespondSuperAccount(request_uuid, result);
    } else {
      account->RespondSuperAccount(request_uuid, 101,
          "GiveApples: player does not exist: " + player_name);
    }
  } else {
    account->RespondSuperAccount(request_uuid, 1,
        "unknown command: " + command);
  }
}


void OnAccountMessage(const fun::Account::Ptr &account,
                      const ::ClientAppMessage &msg) {
  GivingTreePtr world = GivingTree::Cast(fun::World::Get().object());
  GivingTreePtr player = GivingTree::Cast(account->object());

  ::ClientAppMessageType::Type msg_type =
      msg.GetExtension(client_message_type);
  switch (msg_type) {
    case ::ClientAppMessageType::kAccountSendTalk: {
      OnAccountSendTalk(account,
                        msg.GetExtension(account_send_talk));
      break;
    }
    case ::ClientAppMessageType::kPlayerRegisterName: {
      OnPlayerRegisterName(world, player,
                           msg.GetExtension(player_register_name));
      break;
    }
    case ::ClientAppMessageType::kPlayerTakeApple: {
      OnPlayerTakeApple(world, player,
                        msg.GetExtension(player_take_apple));
      break;
    }
    case ::ClientAppMessageType::kPlayerGiveApples: {
      OnPlayerGiveApples(world, player,
                         msg.GetExtension(player_give_apples));
      break;
    }
    default: {
      LOG(ERROR) << "Unknown client message type: " << (int64_t) msg_type;
      break;
    }
  }
}


const string kEpochClockString =
    boost::posix_time::to_iso_string(fun::RealClock::kEpochClock);

const int64_t kCountDownStart = 10;

const char *kRoomChannelName = "room";
const char *kRoomChannelSubId = "1";


void OnAccountSendTalk(const fun::Account::Ptr &account,
                       const ::AccountSendTalk &msg) {
  const fun::AccountId &sender_id = account->account_id();
  const string &service_provider = msg.receiver_service_provider();
  const string &local_account = msg.receiver_local_account();
  fun::AccountId receiver_id(service_provider, local_account);
  const string &talk = msg.talk();

  logger::AccountSendTalk(sender_id.ToString(),
                          receiver_id.ToString(),
                          talk);

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


void OnPlayerRegisterName(const GivingTreePtr &world,
                          const GivingTreePtr &player,
                          const ::PlayerRegisterName &msg) {
  const string &new_name = msg.name();
  if (new_name == "(no name)" || new_name == "") {
    LOG(INFO) << "OnPlayerRegisterName: Fail: invalid player name: ["
              << new_name << "]";
    return;
  }

  string prev_name = player->name();
  if (new_name != prev_name) {
    ErasePlayer(world, prev_name);
    player->set_name(new_name);
    logger::PlayerRegisterName(prev_name, new_name);
  }

  InsertPlayer(world, player);
  logger::PlayerEnterInWorld(new_name);
}


void OnPlayerTakeApple(const GivingTreePtr &/*world*/,
                       const GivingTreePtr &player,
                       const ::PlayerTakeApple &/*msg*/) {
  const fun::RealClock::Value &clock = fun::api::RealClock();
  const string &clock_string = boost::posix_time::to_iso_string(clock);

  logger::PlayerTakeApple(player->name(), clock_string);

  player->set_bet_clock(clock_string);
}


void OnPlayerGiveApples(const GivingTreePtr &/*world*/,
                        const GivingTreePtr &player,
                        const ::PlayerGiveApples &msg) {
  const string &service_provider = msg.target_service_provider();
  const string &local_account = msg.target_local_account();
  fun::AccountId target_id(service_provider, local_account);
  const int64_t &apple_count = msg.apple_count();

  logger::PlayerTryGivingApples(player->name(),
                                target_id.ToString(),
                                apple_count);

  GivingTreePtr target_player =
      GivingTree::Cast(fun::Account::FindAccountObject(target_id));
  if (not target_player) {
    LOG(INFO) << "OnPlayerGiveApples: target player doesn't exist: ["
              << target_id << "].";
    return;
  }

  GiveApples(player, target_player, apple_count);
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

  logger::PlayerFinishGivingApples(giver->name(), giver_count,
                                   taker->name(), taker_count,
                                   target_count, apple_count);

  // for giver.
  logger::CharacterItemHistory(
      giver->local_account(),
      "0",
      giver->name(),
      2,
      3,
      taker->name(),
      giver->name(),
      "GoldenApple",
      "10001",
      target_count,
      giver_count + target_count,
      giver->apple_count(),
      "Market");

  // for taker.
  logger::CharacterItemHistory(
      taker->local_account(),
      "0",
      taker->name(),
      1,
      3,
      taker->name(),
      giver->name(),
      "GoldenApple",
      "10001",
      target_count,
      taker_count - target_count,
      taker->apple_count(),
      "Market");

  // for item.
  logger::ItemHistoryList(
      "10001",
      "GoldenApple",
      target_count,
      giver->name(),
      "GiveApples",
      taker->name(),
      "TakeApples");
}


void Initialize(const GivingTreePtr &world) {
  Reset(world);
}


void Tick(const GivingTreePtr &world) {
  int64_t count_down = CountDown(world);
  logger::WorldTick(world, count_down, time(NULL),
                    "Comment containing \" quote and \n newline");

  if (count_down <= 0) {
    DropApple(world);
    logger::WorldDropApple(world->winner_name());

    Reset(world);
  }
}


void Reset(const GivingTreePtr &world) {
  world->set_count_down(kCountDownStart);
  ResetPlayerBets(world->players());
}


int64_t CountDown(const GivingTreePtr &world) {
  int64_t count_down = world->count_down();
  --count_down;
  world->set_count_down(count_down);
  return count_down;
}


void DropApple(const GivingTreePtr &world) {
  int64_t world_apple_count = world->apple_count();
  ++ world_apple_count;
  world->set_apple_count(world_apple_count);

  const GivingTreePtrMap &players = world->players();
  GivingTreePtr winner = SelectWinner(players);
  string winner_name;
  if (winner) {
    GrantApple(winner);
    winner_name = winner->name();
  }
  world->set_winner_name(winner_name);
}


GivingTreePtr FindPlayer(const GivingTreePtr &world,
                         const string &player_name) {
  return world->players().Find(player_name);
}


void InsertPlayer(const GivingTreePtr &world,
                  const GivingTreePtr &player) {
  GivingTreePtrMap players = world->players();
  players.Insert(player->name(), player);
  world->set_players(players);
}


void ErasePlayer(const GivingTreePtr &world,
                 const string &player_name) {
  GivingTreePtrMap players = world->players();
  players.erase(player_name);
  world->set_players(players);
}


void ResetPlayerBets(const GivingTreePtrMap &players) {
  BOOST_FOREACH(const GivingTreePtrMap::value_type &element, players) {
    // const string &player_name = element.first;
    GivingTreePtr player = GivingTree::Cast(element.second);
    player->set_bet_clock(kEpochClockString);
  }
}


GivingTreePtr SelectWinner(const GivingTreePtrMap &players) {
  GivingTreePtr winner;
  fun::RealClock::Value winner_clock(fun::RealClock::kEpochClock);

  BOOST_FOREACH(const GivingTreePtrMap::value_type &element, players) {
    // const string &player_name = element.first;
    GivingTreePtr player = GivingTree::Cast(element.second);
    const string &bet_clock_string = player->bet_clock();
    if (bet_clock_string == kEpochClockString) {
      continue;
    }
    const fun::RealClock::Value &bet_clock =
        boost::posix_time::from_iso_string(bet_clock_string);
    if (bet_clock > winner_clock) {
      winner = player;
      winner_clock = bet_clock;
    }
  }

  return winner;
}


void GrantApple(const GivingTreePtr &player) {
  int64_t apple_count = player->apple_count();
  ++apple_count;
  player->set_apple_count(apple_count);

  logger::CharacterItemHistory(
      player->local_account(),
      "0",
      player->name(),
      1,
      4,
      player->name(),
      "(the giving-tree)",
      "GoldenApple",
      "10001",
      1,
      apple_count - 1,
      player->apple_count(),
      "UnderTheGivingTree");
}

}  // namespace giving_tree

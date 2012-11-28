// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include "giving_tree_event_handlers.h"

#include <funapi/api/clock.h>
#include <funapi/system/logging.h>

#include "giving_tree_event_handlers_sub_functions.h"
#include "giving_tree.h"


namespace giving_tree {

GivingTreePtr the_world;


void InitializeWorld() {
  ResetWorld();
}


void TickWorld() {
  int64_t count_down = CountDown();
  FUN_LOG_INFO << "TickWorld: CountDown: " << count_down;

  if (count_down <= 0) {
    DropApple();
    FUN_LOG_INFO << "TickWorld: DropApple: winner: ["
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
    FUN_LOG_INFO << "OnPlayerRegisterName: Fail: invalid player name: ["
                 << new_name << "]";
    return;
  }

  string prev_name = player->name();
  if (new_name != prev_name) {
    ErasePlayer(prev_name);
    player->set_name(new_name);
    FUN_LOG_INFO << "OnPlayerRegisterName["
                 << prev_name << "]: ["
                 << new_name << "]";
  }

  InsertPlayer(player);
  FUN_LOG_INFO << "player entered: " << new_name;
}


void OnPlayerTakeApple(const GivingTreePtr &player,
                       const ::PlayerTakeApple &/*msg*/) {
  int64_t now_nanosec = fun::api::MonotonicClock::Now();
  FUN_LOG_INFO << "OnClientTakeApple: [" << player->name() << "]: "
               << now_nanosec;

  player->set_bet_nanos(now_nanosec);
}

}  // namespace giving_tree

// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include "giving_tree_event_handlers_sub_functions.h"

#include <boost/foreach.hpp>

#include "giving_tree.h"


// private members.
namespace giving_tree {

extern GivingTreePtr the_world;

const int64_t kCountDownStart = 10;

void ResetPlayerBets(const GivingTreePtrMap &players);
GivingTreePtr SelectWinner(const GivingTreePtrMap &players);
void GrantApple(const GivingTreePtr &player);
}  // namespace giving_tree


namespace giving_tree {

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
    player->set_bet_nanos(0);
  }
}


GivingTreePtr SelectWinner(const GivingTreePtrMap &players) {
  GivingTreePtr winner;
  int64_t winner_nanos = 0;

  BOOST_FOREACH(const GivingTreePtrMap::value_type &element, players) {
    // const string &player_name = element.first;
    GivingTreePtr player = GivingTree::Cast(element.second);
    int64_t bet_nanos = player->bet_nanos();
    if (bet_nanos > winner_nanos) {
      winner = player;
      winner_nanos = bet_nanos;
    }
  }

  return winner;
}


void GrantApple(const GivingTreePtr &player) {
  int64_t apple_count = player->apple_count();
  ++apple_count;
  player->set_apple_count(apple_count);
}

}  // namespace giving_tree

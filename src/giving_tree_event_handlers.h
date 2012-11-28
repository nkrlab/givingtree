// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef GIVING_TREE_SRC_GIVING_TREE_EVENT_HANDLERS_H_
#define GIVING_TREE_SRC_GIVING_TREE_EVENT_HANDLERS_H_

#include <funapi/common/types.h>

#include "app_messages.pb.h"
#include "giving_tree_client_messages.pb.h"
#include "giving_tree_server_messages.pb.h"
#include "giving_tree_types.h"


namespace giving_tree {

// the one and only world object.
extern GivingTreePtr the_world;

// world methods.
void InitializeWorld();
void TickWorld();

// players methods.
GivingTreePtr FindPlayer(const string &player_name);
void InsertPlayer(const GivingTreePtr &player);
void ErasePlayer(const string &player_name);

// account message handlers.
void OnPlayerRegisterName(const GivingTreePtr &player,
                          const ::PlayerRegisterName &msg);
void OnPlayerTakeApple(const GivingTreePtr &player,
                       const ::PlayerTakeApple &msg);

}  // namespace giving_tree

#endif  // GIVING_TREE_SRC_GIVING_TREE_EVENT_HANDLERS_H_

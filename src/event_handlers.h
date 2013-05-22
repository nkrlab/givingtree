// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef SRC_EVENT_HANDLERS_H_
#define SRC_EVENT_HANDLERS_H_

#include <funapi/account/account_event_handler_registry.h>
#include <funapi/account/account.h>
#include <funapi/common/types.h>
#include <funapi/object/object_creator_registry.h>
#include <funapi/object/object.h>

#include "app_messages.pb.h"
#include "giving_tree_client_messages.pb.h"
#include "giving_tree_server_messages.pb.h"
#include "giving_tree_types.h"


namespace giving_tree {

///////////////////////////////////////////////////////////
// object creators.

// world object 용 model 의 이름.
extern const char *kWorldObjectModelName;

// account object 용 model 의 이름.
extern const char *kAccountObjectModelName;

fun::Object::Ptr CreateObject(const string &model);
fun::Object::Ptr DeserializeObject(const string &serial);


///////////////////////////////////////////////////////////
// world event handlers.

// server 의 timer event 발생 주기.
extern const int64_t kWorldTickMicrosecond;

// server 가 최초 뜰 때만 불린다.
void OnWorldReady(int64_t now_microsec);

// server 의 timer 를 통해 지정된 주기(kWorldTickMicrosecond)로 불린다.
void OnWorldTick(int64_t now_microsec);


///////////////////////////////////////////////////////////
// account event handlers.

void OnAccountLogin(const fun::Account::Ptr &account);
void OnAccountLogout(const fun::Account::Ptr &account);
void OnAccountTimeout(const fun::Account::Ptr &account);
void OnSuperAccountRequest(const fun::Account::Ptr &account,
                           const fun::Uuid &request_uuid,
                           const string &command,
                           const json_spirit::mObject &parameters);
void OnAccountMessage(const fun::Account::Ptr &account,
                      const ::ClientAppMessage &msg);


///////////////////////////////////////////////////////////
// app contents.

// 카운트다운 최초 카운트.
extern const int64_t kCountDownStart;

// room 용 채널의 이름과 서브 아이디.
extern const char *kRoomChannelName;
extern const char *kRoomChannelSubId;

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
void OnPlayerGiveApples(const GivingTreePtr &player,
                        const ::PlayerGiveApples &msg);
void OnAccountSendTalk(const fun::Account::Ptr &account,
                       const ::AccountSendTalk &msg);

// app sub functions.
void ResetWorld();
int64_t CountDown();
void DropApple();
void ResetPlayerBets(const GivingTreePtrMap &players);
GivingTreePtr SelectWinner(const GivingTreePtrMap &players);
void GrantApple(const GivingTreePtr &player);
void GiveApples(const GivingTreePtr &giver, const GivingTreePtr &taker,
                const int64_t &apple_count);

}  // namespace giving_tree

#endif  // SRC_EVENT_HANDLERS_H_

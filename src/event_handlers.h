// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef SRC_EVENT_HANDLERS_H_
#define SRC_EVENT_HANDLERS_H_

#include <funapi/account/account_event_handler_registry.h>
#include <funapi/account/account.h>
#include <funapi/api/clock.h>
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
void OnWorldReady();

// server 의 timer 를 통해 지정된 주기(kWorldTickMicrosecond)로 불린다.
void OnWorldTick();


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

// 최소 시각 값의 스트링 표현.
extern const string kEpochClockString;

// 카운트다운 최초 카운트.
extern const int64_t kCountDownStart;

// room 용 채널의 이름과 서브 아이디.
extern const char *kRoomChannelName;
extern const char *kRoomChannelSubId;

// account message handlers.
void OnAccountSendTalk(const fun::Account::Ptr &account,
                       const ::AccountSendTalk &msg);
void OnPlayerRegisterName(const GivingTreePtr &world,
                          const GivingTreePtr &player,
                          const ::PlayerRegisterName &msg);
void OnPlayerTakeApple(const GivingTreePtr &world,
                       const GivingTreePtr &player,
                       const ::PlayerTakeApple &msg);
void OnPlayerGiveApples(const GivingTreePtr &world,
                        const GivingTreePtr &player,
                        const ::PlayerGiveApples &msg);

// global functions.
void GiveApples(const GivingTreePtr &giver, const GivingTreePtr &taker,
                const int64_t &apple_count);

// methods for world.
void Initialize(const GivingTreePtr &world);
void Tick(const GivingTreePtr &world);
void Reset(const GivingTreePtr &world);
int64_t CountDown(const GivingTreePtr &world);
void DropApple(const GivingTreePtr &world);

GivingTreePtr FindPlayer(const GivingTreePtr &world,
                         const string &player_name);
void InsertPlayer(const GivingTreePtr &world,
                  const GivingTreePtr &player);
void ErasePlayer(const GivingTreePtr &world,
                 const string &player_name);

// methods for players.
void ResetPlayerBets(const GivingTreePtrMap &players);
GivingTreePtr SelectWinner(const GivingTreePtrMap &players);

// methods for player.
void GrantApple(const GivingTreePtr &player);

}  // namespace giving_tree

#endif  // SRC_EVENT_HANDLERS_H_

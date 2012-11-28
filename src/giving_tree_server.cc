// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include <boost/bind.hpp>
#include <funapi/account/account_event_handler_registry.h>
#include <funapi/account/object_creator_registry.h>
#include <funapi/api/tick/world_event_handler_registry.h>
#include <funapi/framework/installer.h>

#include <utility>

#include "funapi_event_handlers.h"


namespace {

class GivingTreeServerInstaller : public fun::framework::Installer {
 public:
  virtual bool Install(
      const fun::framework::Installer::ArgumentMap &/*arguments*/) {
    fun::ObjectCreatorRegistry::Install(
        boost::bind(giving_tree::CreateObject,
                    giving_tree::kAccountObjectModelName),
        giving_tree::DeserializeObject);

    fun::WorldEventHandlerRegistry::Install(
        giving_tree::OnWorldReady,
        std::make_pair(giving_tree::OnWorldTick,
                       giving_tree::kWorldTickMicrosecond));

    fun::AccountEventHandlerRegistry::Install(
        giving_tree::OnAccountLogin,
        giving_tree::OnAccountLogout,
        giving_tree::OnAccountTimeout,
        giving_tree::OnAccountMessage);

    return true;
  }

  virtual bool Uninstall() {
    return true;
  }
};

}  // unnamed namespace


REGISTER_INSTALLER(GivingTreeServer, GivingTreeServerInstaller)

// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_SERVER_SESSION_COMPONENT_H
#define MCC_CLIENT_MODULE_SERVER_SESSION_COMPONENT_H

#include "Common/State.h"
#include "Common/Utils/FlecsUtils.h"

#include <Hexis/Core/Task.h>

namespace Mcc
{

    REGISTER_STATE(StateTargetScene, SrvConnState, Pending, Failed, Connected, Disconnected)
    REGISTER_STATE(StateTargetScene, SessionState, Pending, Failed, Active, Inactive)

    namespace _ { struct ServerSessionModuleTag1 {}; struct ServerSessionModuleTag2 {}; };
    using CSrvConnTask  = ComponentWrapper<Hx::UniqueTaskRef<bool>, _::ServerSessionModuleTag1>;
    using CSrvDConnTask = ComponentWrapper<Hx::UniqueTaskRef<bool>, _::ServerSessionModuleTag2>;

}

#endif

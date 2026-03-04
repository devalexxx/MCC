// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_SERVER_SESSION_COMPONENT_H
#define MCC_CLIENT_MODULE_SERVER_SESSION_COMPONENT_H

#include "Common/State.h"

#include <Hexis/Core/Task.h>

namespace Mcc
{

    REGISTER_STATE(StateTargetScene, SrvConnState, Pending, Failed, Connected, Disconnected)
    REGISTER_STATE(StateTargetScene, SessionState, Pending, Failed, Active, Inactive)

    struct SrvConnResult : Hx::UniqueTaskRef<bool> {};
    struct SrvDConnResult : Hx::UniqueTaskRef<bool> {};

}

#endif

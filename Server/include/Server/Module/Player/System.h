// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_PLAYER_SYSTEM_H
#define MCC_SERVER_MODULE_PLAYER_SYSTEM_H

#include "Common/Module/Entity/Component.h"

#include <flecs.h>

namespace Mcc
{

    void ProcessPlayerInputs (flecs::entity entity, CUserInputQueue& queue);
    void HandlePlayerCreation(flecs::entity entity);

}

#endif

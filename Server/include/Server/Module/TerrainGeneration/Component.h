// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_TERRAIN_GENERATION_COMPONENT_H
#define MCC_SERVER_MODULE_TERRAIN_GENERATION_COMPONENT_H

#include "Server/Module/UserSession/Module.h"

#include "Common/State.h"

#include <Hexis/Core/Task.h>

#include <vector>

namespace Mcc
{

    REGISTER_STATE(StateTargetEntity, GenerationState, Planned, Progress, Done, Failed)

    struct PendingChunk
    {
        Hx::UniqueTaskRef<Chunk> pendingChunk;
    };

    struct PendingReplication
    {
        std::vector<UserSession*> sessions;
    };

}

#endif

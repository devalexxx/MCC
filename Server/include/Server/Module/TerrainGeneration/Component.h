// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_TERRAIN_GENERATION_COMPONENT_H
#define MCC_SERVER_MODULE_TERRAIN_GENERATION_COMPONENT_H

#include "Server/Module/UserSession/Module.h"

#include "Common/State.h"

#include <Hexis/Core/Task.h>

namespace Mcc
{

    REGISTER_STATE(StateTargetEntity, GenerationState, Planned, Progress, Done, Failed)

    namespace _ { struct TerrainGenerationModuleTag {}; };
    using CChunkGenTask       = ComponentWrapper<Hx::UniqueTaskRef<Chunk>        , _::TerrainGenerationModuleTag>;
    using CPendingReplication = ComponentWrapper<std::unordered_set<UserSession*>, _::TerrainGenerationModuleTag>;

}

#endif

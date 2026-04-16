// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_TERRAIN_GENERATION_SYSTEM_H
#define MCC_SERVER_MODULE_TERRAIN_GENERATION_SYSTEM_H

#include <Server/Module/TerrainGeneration/Component.h>

namespace Mcc
{

    void SetupBlockRegistrySystem    (flecs::iter& it);
    void HandleGenerationEndingSystem(flecs::entity entity, CChunkGenTask& task);

}

#endif

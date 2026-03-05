// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_TERRAIN_MODULE_H
#define MCC_SERVER_MODULE_TERRAIN_MODULE_H

#include "Server/Module/UserSession/Module.h"

#include "Common/Module/Base/Module.h"
#include "Common/Module/Network/Module.h"
#include "Common/Module/Terrain/Module.h"

#include <flecs.h>

namespace Mcc
{

    struct TerrainReplicationModule final : BaseModule<TerrainReplicationModule, TerrainModule>
    {
        TerrainReplicationModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterSystem(flecs::world& world) override;
        void RegisterHandler(flecs::world& world) override;

        static void ReplicateChunk(UserSession* session, const flecs::world& world, flecs::entity_t chunk);
    };

}

#endif

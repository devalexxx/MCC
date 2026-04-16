// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_TERRAIN_MODULE_H
#define MCC_SERVER_MODULE_TERRAIN_MODULE_H

#include "Server/Module/TerrainGeneration/Module.h"
#include "Server/Module/UserSession/Module.h"
#include "Server/ServerNetworkManager.h"

#include "Common/Module/Base/Module.h"
#include "Common/Module/Network/Module.h"
#include "Common/Module/Terrain/Module.h"
#include "Common/Utils/CallWrapper.h"

#include <flecs.h>

namespace Mcc
{

    struct TerrainReplicationModule final : BaseModule<TerrainReplicationModule, TerrainModule, TerrainGenerationModule>
    {
        TerrainReplicationModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterPrefab   (flecs::world& world) override;
        void RegisterSystem   (flecs::world& world) override;
        void RegisterObserver (flecs::world& world) override;

        static void LaunchChunkReplicationTask(std::vector<UserSession*> sessions, const flecs::world& world, flecs::entity_t chunk);

      private:

        struct NetworkInfo
        {
            std::vector<UserSession*> sessions;
            ServerNetworkManager*     net;
        };

        struct ChunkReplicationDesc
        {
            ROCallWrapper<std::shared_ptr<Chunk>> chunk;
            std::unordered_map<flecs::entity_t, NetworkHandle> mapping;
            ChunkPosV       position;
            NetworkHandle   handle;
            flecs::entity_t localHandle;
            std::vector<BlockData> blocks;
        };

        static void ChunkReplicationTask(NetworkInfo info, ChunkReplicationDesc desc);
    };

}

#endif

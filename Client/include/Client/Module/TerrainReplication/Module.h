// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_TERRAIN_REPLICATION_MODULE_H
#define MCC_CLIENT_MODULE_TERRAIN_REPLICATION_MODULE_H

#include "Common/Module/Base/Module.h"
#include "Common/Module/Terrain/Module.h"
#include "Common/Network/Packet.h"

#include <flecs.h>

namespace Mcc
{

    class TerrainReplicationModule final : public BaseModule<TerrainReplicationModule, TerrainModule>
    {
      public:
        explicit TerrainReplicationModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterPrefab   (flecs::world& world) override;
        void RegisterSystem   (flecs::world& world) override;
        void RegisterObserver (flecs::world& world) override;

      private:
        static void OnChunkHandler       (const OnChunk& packet       , const flecs::world& world);
        static void OnChunkUpdatedHandler(const OnChunkUpdated& packet, const flecs::world& world);
    };

}

#endif
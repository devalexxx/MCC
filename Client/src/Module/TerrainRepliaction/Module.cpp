// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/TerrainReplication/Module.h"

#include "Client/Module/Renderer/Component.h"
#include "Client/WorldContext.h"

#include "Common/Module/Network/Component.h"
#include "Common/Module/Terrain/Module.h"
#include "Common/SceneImporter.h"
#include "Common/Utils/Assert.h"
#include "Common/Utils/Benchmark.h"
#include "Common/Utils/Logging.h"
#include "Common/World/Geometry.h"

namespace Mcc
{

    TerrainReplicationModule::TerrainReplicationModule(flecs::world& world) : BaseModule(world)
    {
        const auto* ctx = ClientWorldContext::Get(world);
        ctx->networkManager.Subscribe<OnChunk>(OnChunkHandler, world);
    }

    void TerrainReplicationModule::RegisterComponent(flecs::world& /* world */) {}

    void TerrainReplicationModule::RegisterPrefab(flecs::world& /* world */) {}

    void TerrainReplicationModule::RegisterSystem(flecs::world& /* world */) {}

    void TerrainReplicationModule::RegisterObserver(flecs::world& /* world */) {}

    void TerrainReplicationModule::OnChunkHandler(const OnChunk& packet, const flecs::world& world)
    {
        auto* ctx = ClientWorldContext::Get(world);
        if (const auto lid = ctx->networkMapping.GetLHandle(packet.handle); lid)
        {
            MCC_LOG_WARN("Chunk({}) is already associated to a local entity(#{})", packet.handle, *lid);
            return;
        }

        std::unordered_map<NetworkHandle, flecs::entity_t> blockMapping;
        for (auto& [handle, type, meta, asset]: packet.blocks)
        {
            flecs::entity_t blockHandle;
            if (const auto blockHandleOpt = ctx->networkMapping.GetLHandle(handle); !blockHandleOpt)
            {
                blockHandle = world.entity()
                    .is_a<PBlock>()
                    .set<CNetProps>  ({ handle })
                    .set<CBlockMeta> (std::move(meta))
                    .set<CBlockAsset>(std::move(asset))
                    .set<CBlockType> (std::move(type))
                    .child_of<SceneRoot>();
            }
            else
            {
                blockHandle = *blockHandleOpt;
            }

            blockMapping.emplace(handle, blockHandle);
        }

        if (auto from = MCC_BENCH_TIME(RLEDecompression, Helper::FromNetwork)(packet.data, blockMapping); from)
        {
            const auto e = world.entity()
               .is_a<PChunk>()
               .set<CNetProps>({ packet.handle })
               .set<CChunkPos>(packet.position)
               .set<CChunkPtr>(std::make_shared<Chunk>(std::move(*from)))
               .child_of<SceneRoot>();

            ctx->chunkMapping.emplace(packet.position, e.id());
        }
    }


}
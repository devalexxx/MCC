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
#include "Common/World/Translation.h"
#include "Common/World/Geometry.h"

#include <glm/gtx/quaternion.hpp>

namespace Mcc
{

    TerrainReplicationModule::TerrainReplicationModule(flecs::world& world) : BaseModule(world)
    {
        const auto* ctx = ClientWorldContext::Get(world);
        ctx->networkManager.Subscribe<OnBlock>     (OnBlockHandler, world);
        ctx->networkManager.Subscribe<OnChunk>     (OnChunkHandler, world);
        ctx->networkManager.Subscribe<OnBlockBatch>(OnBlockBatchHandler, world);
        ctx->networkManager.Subscribe<OnChunkBatch>(OnChunkBatchHandler, world);
    }

    void TerrainReplicationModule::RegisterComponent(flecs::world& /* world */) {}

    void TerrainReplicationModule::RegisterPrefab(flecs::world& /* world */) {}

    void TerrainReplicationModule::RegisterSystem(flecs::world& /* world */) {}

    void TerrainReplicationModule::RegisterObserver(flecs::world& /* world */) {}

    void TerrainReplicationModule::OnBlockHandler(const OnBlock& packet, const flecs::world& world)
    {
        const auto* ctx = ClientWorldContext::Get(world);
        if (const auto lid = ctx->networkMapping.GetLHandle(packet.handle); lid.has_value())
        {
            MCC_LOG_WARN("[OnBlockHandler] Block({}) is already associated to a local entity(#{})", packet.handle, *lid);
            return;
        }

        world.entity()
            .is_a<PBlock>()
            .set<CNetProps>({ packet.handle })
            .set<CBlockMeta>(packet.meta)
            .set<CBlockAsset>(packet.asset)
            .set<CBlockType>(packet.type)
            .child_of<SceneRoot>();
    }

    void TerrainReplicationModule::OnChunkHandler(const OnChunk& packet, const flecs::world& world)
    {
        auto* ctx = ClientWorldContext::Get(world);
        if (const auto lid = ctx->networkMapping.GetLHandle(packet.handle); lid.has_value())
        {
            MCC_LOG_WARN("[OnChunkHandler] Chunk({}) is already associated to a local entity(#{})", packet.handle, *lid);
            return;
        }

        if (auto from = MCC_BENCH_TIME(RLEDecompression, Helper::FromNetwork)(packet.data, world); from.has_value())
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

    void TerrainReplicationModule::OnBlockBatchHandler(const OnBlockBatch& packet, const flecs::world& world)
    {
        for (auto& block: packet) { OnBlockHandler(block, world); }
    }

    void TerrainReplicationModule::OnChunkBatchHandler(const OnChunkBatch& packet, const flecs::world& world)
    {
        for (auto& chunk: packet) { OnChunkHandler(chunk, world); }
    }


}
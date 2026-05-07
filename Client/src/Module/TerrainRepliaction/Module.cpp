// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/TerrainReplication/Module.h"

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
        ctx->networkManager.Subscribe<OnChunk>       (OnChunkHandler       , world);
        ctx->networkManager.Subscribe<OnChunkUpdated>(OnChunkUpdatedHandler, world);
    }

    void TerrainReplicationModule::RegisterComponent(flecs::world& world)
    {
        world.component<CChunkUpdateInfo>();
    }

    void TerrainReplicationModule::RegisterPrefab(flecs::world& /* world */) {}

    void TerrainReplicationModule::RegisterSystem(flecs::world& world)
    {
        world.system()
            .kind<Phase::OnSetup>()
            .with<CChunkUpdateInfo>()
            .each([](const flecs::entity e)
            {
                e.remove<CChunkUpdateInfo>();
            });
    }

    void TerrainReplicationModule::RegisterObserver(flecs::world& /* world */) {}

    static flecs::entity CreateBlock(const BlockData& data, const flecs::world& world)
    {
        return world.entity()
            .is_a<PBlock>()
            .set<CNetProps>  ({ data.handle })
            .set<CBlockMeta> (std::move(data.meta))
            .set<CBlockAsset>(std::move(data.asset))
            .set<CBlockType> (std::move(data.type))
            .child_of<SceneRoot>();
    }

    void TerrainReplicationModule::OnChunkHandler(const OnChunk& packet, const flecs::world& world)
    {
        auto* ctx = ClientWorldContext::Get(world);

        const auto chunkEntityTOpt = ctx->networkMapping.GetLHandle(packet.handle);
        const auto chunkEntity     = chunkEntityTOpt
            ? world.entity(*chunkEntityTOpt)
                .add<TDirty>()
            : world.entity()
                .is_a<PChunk>()
                .set<CNetProps>({ packet.handle })
                .set<CChunkPos>(packet.position)
                .child_of<SceneRoot>();


        std::unordered_map<NetworkHandle, flecs::entity_t> blockMapping;
        for (auto& block: packet.blocks)
        {
            const auto blockHandleOpt = ctx->networkMapping.GetLHandle(block.handle);
            blockMapping.emplace(
                block.handle,
                *blockHandleOpt.or_else([&] { return std::optional(CreateBlock(block, world).id()); })
            );
        }

        auto from = MCC_BENCH_TIME(RLEDecompression, Helper::FromNetwork)(packet.data, blockMapping);
        if (!from)
        {
            MCC_LOG_ERROR("Failed to decompress chunk data");
            return;
        }

        chunkEntity.set<CChunkPtr>(std::make_shared<Chunk>(std::move(*from)));
        ctx->chunkMapping.emplace(packet.position, chunkEntity.id());
    }

    void TerrainReplicationModule::OnChunkUpdatedHandler(const OnChunkUpdated& packet, const flecs::world& world)
    {
        const auto* ctx         = ClientWorldContext::Get(world);
        const auto chunkEntityT = ctx->networkMapping.GetLHandle(packet.chunkHandle);
        if (!chunkEntityT)
        {
            MCC_LOG_WARN("Chunk({}) isn't associated to a local entity(#{})", packet.chunkHandle, *chunkEntityT);
            return;
        }

        CChunkUpdateInfo info;
        bool hasChanged = false;
        const auto chunkEntity = world.entity(*chunkEntityT);
        const auto chunkPtr    = chunkEntity.get<CChunkPtr>();
        for (const auto& [handle, position, block]: packet.updates)
        {
            const bool changed = chunkPtr->Set(
                position,
                *block
                    .transform([&](const BlockData& data) { return CreateBlock(data, world); })
                    .or_else([&] { return std::optional(world.entity(*ctx->networkMapping.GetLHandle(handle))); })
            );

            if (changed)
            {
                const glm::uvec3 pos = position;
                info.negativeX = pos.x == 0;
                info.positiveX = pos.x == Chunk::Size - 1;
                info.negativeY = pos.y == 0;
                info.positiveY = pos.y == Chunk::Size - 1;
                info.negativeZ = pos.z == 0;
                info.positiveZ = pos.z == Chunk::Size - 1;
            }

            hasChanged |= changed;
        }

        if (hasChanged)
        {
            MCC_LOG_DEBUG("Chunk({}) has been updated", packet.chunkHandle);
            chunkEntity.add<TDirty>();
            chunkEntity.set<CChunkUpdateInfo>(info);
        }
    }


}
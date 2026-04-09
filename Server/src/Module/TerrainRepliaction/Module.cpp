// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/TerrainReplication/Module.h"

#include "Server/Module/Player/Component.h"
#include "Server/Module/TerrainReplication/Component.h"
#include "Server/Module/TerrainReplication/System.h"
#include "Server/Module/UserSession/Module.h"
#include "Server/WorldContext.h"

#include "Common/Module/Network/Component.h"
#include "Common/Module/Terrain/Component.h"
#include "Common/Module/Terrain/Module.h"

namespace Mcc
{

    TerrainReplicationModule::TerrainReplicationModule(flecs::world& world) : BaseModule(world) {}

    void TerrainReplicationModule::RegisterComponent(flecs::world& world)
    {
        world.component<TBlockCreated>();
        world.component<TBlockDirty>();
        world.component<TBlockDestroyed>();

        world.component<TChunkCreated>();
        world.component<TChunkDirty>();
        world.component<TChunkDestroyed>();
    }

    void TerrainReplicationModule::RegisterPrefab(flecs::world& /* world */) {}

    void TerrainReplicationModule::RegisterSystem(flecs::world& /* world */) {}

    void TerrainReplicationModule::RegisterObserver(flecs::world& world)
    {
        world.observer("OnPlayerMove")
            .with(flecs::Any)
            .event<OnPlayerMoveEvent>()
            .each(OnPlayerMoveObserver);

        world.observer<const CEntityTransform>("OnPlayerCreated")
            .event<OnPlayerCreatedEvent>()
            .each(OnPlayerCreatedObserver);
    }

    void
    TerrainReplicationModule::ReplicateChunk(UserSession* session, const flecs::world& world, flecs::entity_t chunk)
    {
        // TODO: could data race
        const auto  ctx   = ServerWorldContext::Get(world);
        const auto& stage = world;

        OnChunk      chunkPacket;
        OnBlockBatch blockPacket;

        const auto  chunkEntity = stage.entity(chunk);
        const auto [handle] = chunkEntity.get<const CNetProps>();
        const auto position = chunkEntity.get<const CChunkPos>();
        const auto chunkPtr = chunkEntity.get<const CChunkPtr>();

        chunkPacket.handle   = handle;
        chunkPacket.position = position;
        if (auto rle = chunkPtr->ToNetwork(stage); rle.has_value())
        {
            chunkPacket.data = std::move(rle.value());
        }

        for (auto block: chunkPtr->GetPalette())
        {
            if (!session->replicatedBlocks->contains(block))
            {
                auto    blockEntity = stage.entity(block);
                OnBlock packet;
                packet.handle = blockEntity.get<const CNetProps>().handle;
                packet.type   = blockEntity.get<const CBlockType>();
                packet.meta   = blockEntity.get<const CBlockMeta>();
                packet.asset  = blockEntity.get<const CBlockAsset>();
                blockPacket.push_back(std::move(packet));

                session->replicatedBlocks->insert(block);
            }
        }
        session->replicatedChunksPending->erase(chunk);
        session->replicatedChunks->insert(chunk);

        if (!blockPacket.empty())
            ctx->networkManager.Send(session->peer, std::move(blockPacket), ENET_PACKET_FLAG_RELIABLE, 0);

        ctx->networkManager.Send(session->peer, std::move(chunkPacket), ENET_PACKET_FLAG_RELIABLE, 0);
    }

}
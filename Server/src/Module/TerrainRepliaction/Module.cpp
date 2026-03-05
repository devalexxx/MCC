// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/TerrainReplication/Module.h"

#include "Server/Module/Player/Component.h"
#include "Server/Module/TerrainGeneration/Component.h"
#include "Server/Module/TerrainReplication/Component.h"
#include "Server/Module/TerrainReplication/System.h"
#include "Server/Module/UserSession/Module.h"
#include "Server/WorldContext.h"

#include "Common/Module/Network/Component.h"
#include "Common/Module/Terrain/Component.h"
#include "Common/Module/Terrain/Module.h"
#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    TerrainReplicationModule::TerrainReplicationModule(flecs::world& world) : BaseModule(world)
    {}

    void TerrainReplicationModule::RegisterComponent(flecs::world& world)
    {
        world.component<BlockCreatedTag>();
        world.component<BlockDirtyTag>();
        world.component<BlockDestroyedTag>();

        world.component<ChunkCreatedTag>();
        world.component<ChunkDirtyTag>();
        world.component<ChunkDestroyedTag>();
    }

    void TerrainReplicationModule::RegisterSystem(flecs::world& /* world */)
    {

    }

    void TerrainReplicationModule::RegisterHandler(flecs::world& world)
    {
        world.observer("OnPlayerMoveObserver")
            .with(flecs::Any)
            .event<OnPlayerMoveEvent>()
            .each(OnPlayerMoveObserver);

        world.observer<const Transform>("OnPlayerCreatedObserver")
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

        const auto chunkEntity = stage.entity(chunk);
        const auto& [handle]   = chunkEntity.get<const NetworkProps>();
        const auto& position   = chunkEntity.get<const ChunkPosition>();
        const auto  chunkPtr   = chunkEntity.get<const ChunkHolder>().chunk;

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
                packet.handle = blockEntity.get<const NetworkProps>().handle;
                packet.color  = blockEntity.get<const BlockColor>().color;
                packet.meta   = blockEntity.get<const BlockMeta>();
                packet.type   = blockEntity.get<const BlockType>();
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
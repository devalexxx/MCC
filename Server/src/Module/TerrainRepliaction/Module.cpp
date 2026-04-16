// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/TerrainReplication/Module.h"

#include "Server/Module/Player/Component.h"
#include "Server/Module/TerrainReplication/Component.h"
#include "Server/Module/TerrainReplication/System.h"
#include "Server/Module/UserSession/Component.h"
#include "Server/Module/UserSession/Module.h"
#include "Server/WorldContext.h"

#include "Common/Module/Network/Component.h"
#include "Common/Module/Terrain/Component.h"
#include "Common/Module/Terrain/Module.h"
#include "Common/Phase.h"
#include "Common/Utils/ChunkHelper.h"

namespace Mcc
{

    TerrainReplicationModule::TerrainReplicationModule(flecs::world& world) : BaseModule(world)
    {
        const auto* ctx = ServerWorldContext::Get(world);
        ctx->networkManager.Subscribe<From<OnBlockBreak>>(OnBlockBreakHandler, world);
        ctx->networkManager.Subscribe<From<OnBlockPlace>>(OnBlockPlaceHandler, world);
    }

    void TerrainReplicationModule::RegisterComponent(flecs::world& world)
    {
        world.component<TBlockCreated>();
        world.component<TBlockDirty>();
        world.component<TBlockDestroyed>();

        world.component<TChunkCreated>();
        world.component<TChunkDirty>();
        world.component<TChunkDestroyed>();

        AutoRegister<CPendingReplication>::Register(world, "CPendingReplication");
    }

    void TerrainReplicationModule::RegisterPrefab(flecs::world& /* world */) {}

    void TerrainReplicationModule::RegisterSystem(flecs::world& world)
    {
        world.system<CPendingReplication>("DispatchPendingChunkReplication")
            .kind<Phase::OnRender>() // To change (add a new phase)
            .with<TChunk>()
            .each(DispatchPendingChunkReplicationSystem);
    }

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

    void TerrainReplicationModule::LaunchChunkReplicationTask(
        std::vector<UserSession*> sessions, const flecs::world& world, flecs::entity_t chunk
    )
    {
        const auto  ctx   = ServerWorldContext::Get(world);
        const auto& stage = world;

        NetworkInfo info;
        info.sessions = std::move(sessions);
        info.net      = &ctx->networkManager;

        const auto chunkEntity = stage.entity(chunk);
        const auto [handle] = chunkEntity.get<const CNetProps>();
        const auto position = chunkEntity.get<const CChunkPos>();
        const auto chunkPtr = chunkEntity.get<const CChunkPtr>();

        std::vector<BlockData>                             blocks;
        std::unordered_map<flecs::entity_t, NetworkHandle> mapping;
        for (const auto blockEntityT : chunkPtr->GetPalette())
        {
            const auto blockEntity = stage.entity(blockEntityT);
            BlockData block;
            block.handle = blockEntity.get<const CNetProps>().handle;
            block.type   = blockEntity.get<const CBlockType>();
            block.meta   = blockEntity.get<const CBlockMeta>();
            block.asset  = blockEntity.get<const CBlockAsset>();
            mapping.emplace(blockEntityT, block.handle);
            blocks.push_back(std::move(block));
        }

        ChunkReplicationDesc desc { .chunk = { chunkPtr } };
        desc.mapping     = std::move(mapping);
        desc.position    = position;
        desc.handle      = handle;
        desc.localHandle = chunk;
        desc.blocks      = std::move(blocks);

        ctx->scheduler.Insert(ChunkReplicationTask, std::move(info), std::move(desc)).Enqueue();
    }

    void TerrainReplicationModule::OnBlockBreakHandler(const From<OnBlockBreak>& from, const flecs::world& world)
    {
        const auto* ctx = ServerWorldContext::Get(world);
        const auto  it  = ctx->networkMapping.GetLHandle(from.packet.chunkHandle);
        if (!it)
        {
            MCC_LOG_WARN("Chunk({}) isn't associated to a local entity", from.packet.chunkHandle);
            return;
        }

        const auto placeholder = world.lookup("#32::mcc:block:air");
        const auto chunkEntity = world.entity(*it);
        const auto chunkPtr    = chunkEntity.get<CChunkPtr>();
        chunkPtr->Set(
            from.packet.position,
            placeholder
        );

        const glm::ivec2   chunkPos = chunkEntity.get<CChunkPos>();
        std::unordered_set peers    = { from.peer };
        world.query_builder<const CEntityTransform, const CUserSession>()
            .each([&](flecs::entity, const CEntityTransform& tr, const CUserSession& us)
            {
                if (auto [p, l] = tr.position; Helper::IsInCircle(chunkPos, glm::ivec2(p), ctx->settings.renderDistance))
                {
                    auto proxy = *us.ptr->replicatedChunks;
                    if (const auto repl = proxy->find(chunkEntity); repl != proxy->cend())
                    {
                        repl->second.hash = std::hash<Chunk>{}(*chunkPtr);
                    }

                    peers.insert(us.ptr->peer);
                }
            });

        const auto placeholderHandle = ctx->networkMapping.GetRHandle(placeholder).value();
        ctx->scheduler
            .Insert([=, peers = std::move(peers)]
            {
                ctx->networkManager.Send(
                    std::vector(peers.begin(), peers.end()),
                    OnChunkUpdated {
                        .chunkHandle = from.packet.chunkHandle,
                        .updates = {{
                            {
                                .blockHandle   = placeholderHandle,
                                .position = from.packet.position,
                                .blockOpt    = std::nullopt
                            }
                        }}
                    },
                    ENET_PACKET_FLAG_RELIABLE,
                    0
                );
            })
            .Enqueue();
    }

    void TerrainReplicationModule::OnBlockPlaceHandler(const From<OnBlockPlace>& from, const flecs::world& world)
    {
        const auto* ctx = ServerWorldContext::Get(world);
        const auto  it  = ctx->networkMapping.GetLHandle(from.packet.chunkHandle);
        if (!it)
        {
            MCC_LOG_WARN("Chunk({}) isn't associated to a local entity", from.packet.chunkHandle);
            return;
        }

        const auto placeholder = world.lookup("#32::mcc:block:air");
        const auto replacement = ctx->networkMapping.GetLHandle(from.packet.blockHandle);
        const auto chunkEntity = world.entity(*it);
        const auto chunkPtr    = chunkEntity.get<CChunkPtr>();

        if (!replacement)
        {
            MCC_LOG_ERROR("Block({}) isn't associated to a local entity", from.packet.blockHandle);
            return;
        }

        if (chunkPtr->Get(glm::uvec3(from.packet.position)) != placeholder)
        {
            MCC_LOG_WARN("Trying to place block({}) in place of another block", from.packet.blockHandle);
            return;
        }

        chunkPtr->Set(
            from.packet.position,
            *replacement
        );

        const glm::ivec2   chunkPos = chunkEntity.get<CChunkPos>();
        std::unordered_set peers    = { from.peer };
        world.query_builder<const CEntityTransform, const CUserSession>()
            .each([&](flecs::entity, const CEntityTransform& tr, const CUserSession& us)
            {
                if (auto [p, l] = tr.position; Helper::IsInCircle(chunkPos, glm::ivec2(p), ctx->settings.renderDistance))
                {
                    auto proxy = *us.ptr->replicatedChunks;
                    if (const auto repl = proxy->find(chunkEntity); repl != proxy->cend())
                    {
                        repl->second.hash = std::hash<Chunk>{}(*chunkPtr);
                    }

                    peers.insert(us.ptr->peer);
                }
            });

        ctx->scheduler
            .Insert([=, peers = std::move(peers)]
            {
                ctx->networkManager.Send(
                    std::vector(peers.begin(), peers.end()),
                    OnChunkUpdated {
                        .chunkHandle = from.packet.chunkHandle,
                        .updates = {{
                            {
                                .blockHandle = from.packet.blockHandle,
                                .position    = from.packet.position,
                                .blockOpt    = std::nullopt
                            }
                        }}
                    },
                    ENET_PACKET_FLAG_RELIABLE,
                    0
                );
            })
            .Enqueue();
    }

    void TerrainReplicationModule::ChunkReplicationTask(NetworkInfo info, ChunkReplicationDesc desc)
    {
        OnChunk chunkPacket;

        chunkPacket.handle   = desc.handle;
        chunkPacket.position = desc.position;
        chunkPacket.blocks   = std::move(desc.blocks);

        auto rle = desc.chunk->ToNetwork(desc.mapping);

        for (const auto session : info.sessions)
        {
            auto       proxy = *session->replicatedChunks;
            const auto it    = proxy->find(desc.localHandle);
            it->second.isPending = false;
            it->second.hash      = rle ? std::hash<Chunk>{}(*desc.chunk) : it->second.hash;
        }

        if (!rle)
        {
            MCC_LOG_ERROR("Failed to convert chunk({}, #{}) to network format", desc.handle, desc.localHandle);
            return;
        }
        chunkPacket.data = std::move(*rle);

        std::vector<ENetPeer*> peers;
        std::ranges::transform(info.sessions, std::back_inserter(peers), &UserSession::peer);
        info.net->Send(std::move(peers), std::move(chunkPacket), ENET_PACKET_FLAG_RELIABLE, 0);
    }

}
// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/TerrainReplication/System.h"

#include "Server/Module/Player/Component.h"
#include "Server/Module/TerrainGeneration/Component.h"
#include "Server/Module/TerrainGeneration/Module.h"
#include "Server/Module/TerrainReplication/Module.h"
#include "Server/Module/UserSession/Component.h"
#include "Server/Module/UserSession/Module.h"
#include "Server/WorldContext.h"

#include "Common/Utils/ChunkHelper.h"

#include <ranges>

namespace Mcc
{

    static void ReplicateChunksAroundPlayer(const flecs::entity entity, const long x, const long z)
    {
        const auto world   = entity.world();
        const auto session = entity.get<UserSessionHolder>().session;
        const auto ctx     = ServerWorldContext::Get(world);

        std::vector<std::pair<flecs::entity, ChunkPosition>> chunks;
        Helper::ForInCircle(x, z, ctx->settings.renderDistance, [&](const long cx, const long cz) {
            const glm::ivec3 position(cx, 0, cz);
            if (const auto it = ctx->chunkMap.find(position); it != ctx->chunkMap.end())
            {
                if (session->replicatedChunks->contains(it->second) ||
                    session->replicatedChunksPending->contains(it->second))
                {
                    return;
                }

                auto chunk = world.entity(it->second);
                if (GenerationState::Done::IsActive(chunk))
                {
                    chunks.emplace_back(chunk, chunk.get<ChunkPosition>());
                    session->replicatedChunksPending->insert(it->second);
                    return;
                }

                if (chunk.has<PendingReplication>())
                {
                    chunk.get_mut<PendingReplication>().sessions.push_back(session);
                }
                else
                {
                    chunk.set<PendingReplication>({ { session } });
                }
                session->replicatedChunksPending->insert(it->second);

                return;
            }

            const auto chunk = world.get<TerrainGenerationModule>().LaunchGenerationTask(world, position);
            chunk.set<PendingReplication>({ { session } });
            session->replicatedChunksPending->insert(chunk);
        });

        std::ranges::sort(chunks, [&](auto& lhs, auto& rhs) {
            auto      lp = lhs.second.position;
            auto      rp = rhs.second.position;
            const int dl = (lp.x - x) * (lp.x - x) + (lp.z - z) * (lp.z - z);
            const int dr = (rp.x - x) * (rp.x - x) + (rp.z - z) * (rp.z - z);
            return dl < dr;
        });

        for (const auto& id: chunks | std::views::keys)
        {
            ctx->scheduler.Insert(TerrainReplicationModule::ReplicateChunk, session, world, id).Enqueue();
        }
    }

    void OnPlayerCreatedObserver(const flecs::entity entity, const Transform& transform)
    {
        auto [x, z] = Helper::GetPlayerChunkPosition(transform.position);
        ReplicateChunksAroundPlayer(entity, x, z);
    }

    void OnPlayerMoveObserver(flecs::iter& it, size_t row)
    {
        const auto evt = it.param<OnPlayerMoveEvent>();
        auto [px, pz]  = Helper::GetPlayerChunkPosition(evt->prev);
        auto [cx, cz]  = Helper::GetPlayerChunkPosition(evt->curr);
        if (std::abs(px - cx) > 0 || std::abs(pz - cz) > 0)
        {
            ReplicateChunksAroundPlayer(it.entity(row), cx, cz);
        }
    }

}
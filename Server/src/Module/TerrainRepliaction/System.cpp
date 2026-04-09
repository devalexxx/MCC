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
        auto&      session = entity.get_mut<CUserSession>();
        const auto ctx     = ServerWorldContext::Get(world);

        std::vector<std::pair<flecs::entity, CChunkPos>> chunks;
        Helper::ForInCircle(x, z, ctx->settings.renderDistance, [&](const long cx, const long cz) {
            const glm::ivec2 position(cx, cz);
            if (const auto it = ctx->chunkMapping.find(position); it != ctx->chunkMapping.end())
            {
                if (session->replicatedChunks->contains(it->second) ||
                    session->replicatedChunksPending->contains(it->second))
                {
                    return;
                }

                auto chunk = world.entity(it->second);
                if (GenerationState::Done::IsActive(chunk))
                {
                    chunks.emplace_back(chunk, chunk.get<CChunkPos>());
                    session->replicatedChunksPending->insert(it->second);
                    return;
                }

                if (chunk.has<CPendingReplication>())
                {
                    chunk.get_mut<CPendingReplication>().emplace(session.Get());
                }
                else
                {
                    chunk.set<CPendingReplication>({ session.Get() });
                }
                session->replicatedChunksPending->insert(it->second);

                return;
            }

            const auto chunk = world.get<TerrainGenerationModule>().LaunchGenerationTask(world, { position.x, 0, position.y });
            chunk.set<CPendingReplication>({ session.Get() });
            session->replicatedChunksPending->insert(chunk);
        });

        std::ranges::sort(chunks, [&](auto& lhs, auto& rhs) {
            const glm::ivec2 lp = lhs.second;
            const glm::ivec2 rp = rhs.second;
            const int dl = (lp.x - x) * (lp.x - x) + (lp.y - z) * (lp.y - z);
            const int dr = (rp.x - x) * (rp.x - x) + (rp.y - z) * (rp.y - z);
            return dl < dr;
        });

        for (const auto& id: chunks | std::views::keys)
        {
            ctx->scheduler.Insert(TerrainReplicationModule::ReplicateChunk, session.Get(), world, id).Enqueue();
        }
    }

    void OnPlayerCreatedObserver(const flecs::entity entity, const CEntityTransform& transform)
    {
        auto [x, z] = get<0>(transform.position);
        ReplicateChunksAroundPlayer(entity, x, z);
    }

    void OnPlayerMoveObserver(flecs::iter& it, const size_t row)
    {
        const auto evt = it.param<OnPlayerMoveEvent>();
        auto [px, pz]  = get<0>(evt->prev);
        auto [cx, cz]  = get<0>(evt->curr);
        if (std::abs(px - cx) > 0 || std::abs(pz - cz) > 0)
        {
            ReplicateChunksAroundPlayer(it.entity(row), cx, cz);
        }
    }

}
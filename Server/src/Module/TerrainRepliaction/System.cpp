// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/TerrainReplication/System.h"

#include "Server/Module/Player/Component.h"
#include "Server/Module/TerrainGeneration/Module.h"
#include "Server/Module/TerrainReplication/Module.h"
#include "Server/Module/UserSession/Component.h"
#include "Server/Module/UserSession/Module.h"
#include "Server/WorldContext.h"

#include "Common/Utils/ChunkHelper.h"

#include <ranges>

namespace Mcc
{

    static void Replicate(CUserSession& session, const flecs::entity chunk)
    {
        session->replicatedChunks->emplace(chunk.id(), ReplicationInfo { .hash = 0, .isPending = true });
        chunk.ensure<CPendingReplication>().emplace(session.Get());
    }

    static void ReplicateChunksAroundPlayer(const flecs::entity entity, const long x, const long z)
    {
        const auto world   = entity.world();
        auto&      session = entity.get_mut<CUserSession>();
        const auto ctx     = ServerWorldContext::Get(world);

        Helper::ForInCircle(x, z, ctx->settings.renderDistance, [&](const long cx, const long cz)
        {
            const glm::ivec2 position(cx, cz);

            const auto it = ctx->chunkMapping.find(position);
            if (it == ctx->chunkMapping.cend()) // < Chunk don't exist -> Generate and Replicate
            {
                const auto chunk = world.get<TerrainGenerationModule>().LaunchGenerationTask(world, { position.x, 0, position.y });
                Replicate(session, chunk);
                return;
            }

            const auto chunk = world.entity(it->second);
            const auto repl  = session->replicatedChunks->find(chunk.id());
            if (repl == session->replicatedChunks->end()) // < Chunk has never been replicated -> Replicate
            {
                Replicate(session, chunk);
                return;
            }

            if (repl->second.isPending)
                return;

            if (const auto& chunkPtr = chunk.get<CChunkPtr>(); *chunkPtr == repl->second.hash)
                return;
            
            // < Chunk has been replicated, but version mismatch and the replication isn't yet pending -> Replicate
            repl->second.isPending = true;
            chunk.ensure<CPendingReplication>().emplace(session.Get());
        });
    }

    void DispatchPendingChunkReplicationSystem(const flecs::entity entity, CPendingReplication& pending)
    {
        if (!GenerationState::Done::IsActive(entity)) return;
        if (pending.empty())                          return;

        TerrainReplicationModule::LaunchChunkReplicationTask(
            std::vector(pending.begin(), pending.end()),
            entity.world(),
            entity.id()
        );
        pending.clear();
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
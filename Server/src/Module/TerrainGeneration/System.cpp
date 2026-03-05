// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/TerrainGeneration/System.h"

#include "Server/Module/TerrainGeneration/Module.h"
#include "Server/Module/TerrainReplication/Module.h"
#include "Server/WorldContext.h"

#include "Common/SceneImporter.h"

namespace Mcc
{

    void SetupBlockRegistrySystem(flecs::iter& it)
    {
        const auto world = it.world();

        world.entity("mcc:block:air")
            .is_a<BlockPrefab>()
            .set<BlockType>(BlockType::Gas)
            .set<BlockMeta>({ "mcc:block:air" })
            .child_of<SceneRoot>();

        world.entity("mcc:block:stone")
            .is_a<BlockPrefab>()
            .set<BlockType>(BlockType::Solid)
            .set<BlockColor>({{ .5f, .5f, .5f }})
            .set<BlockMeta>({ "mcc:block:stone" })
            .child_of<SceneRoot>();

        world.entity("mcc:block:dirt")
            .is_a<BlockPrefab>()
            .set<BlockType>(BlockType::Solid)
            .set<BlockColor>({{ .0f, .7f, .3f }})
            .set<BlockMeta>({ "mcc:block:dirt" })
            .child_of<SceneRoot>();

        world.get_mut<TerrainGenerationModule>().InitializeGenerator(world);
    }

    void HandleGenerationEndingSystem(const flecs::entity entity, PendingChunk& pending, ChunkHolder& holder)
    {
        if (pending.pendingChunk.GetState() == Hx::TaskState::Cancelled)
        {
            // TODO: add a state maybe
            entity.remove<PendingChunk>();
            GenerationState::Failed::Enter(entity);
            return;
        }

        if (pending.pendingChunk.GetState() == Hx::TaskState::Done)
        {
            const auto result = pending.pendingChunk.GetResult();
            MCC_ASSERT(result, "Chunk data has already been retrieve");
            holder.chunk = std::make_shared<Chunk>(result->get());
            entity.remove<PendingChunk>();
            GenerationState::Done::Enter(entity);
        }
    }

    void DispatchPendingReplication(const flecs::entity entity, const GenerationState)
    {
        entity.get([&](const PendingReplication& pending) {
            const auto world = entity.world();
            const auto ctx   = ServerWorldContext::Get(world);
            for (const auto session: pending.sessions)
            {
                ctx->scheduler.Insert(TerrainReplicationModule::ReplicateChunk, session, world, entity.id()).Enqueue();
            }
        });

        entity.remove<PendingReplication>();
    }

}
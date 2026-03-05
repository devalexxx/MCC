// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/TerrainGeneration/Module.h"

#include "Server/Module/TerrainGeneration/Component.h"
#include "Server/Module/TerrainGeneration/System.h"
#include "Server/WorldContext.h"

#include "Common/Phase.h"
#include "Common/SceneImporter.h"

namespace Mcc
{

    TerrainGenerationModule::TerrainGenerationModule(flecs::world& world) :
        BaseModule(world),
        mGenerator(123456789u)
    {}

    void TerrainGenerationModule::RegisterComponent(flecs::world& world)
    {
        GenerationState::Register(world);

        world.component<PendingChunk>();
        world.component<PendingReplication>();
    }

    void TerrainGenerationModule::RegisterSystem(flecs::world& world)
    {
        world.system()
            .kind<Phase::OnLoad>()
            .run(SetupBlockRegistrySystem);

        world.system<PendingChunk, ChunkHolder>("HandleGenerationEndingSystem")
            .kind<Phase::OnSetup>()
            .with<GenerationState, GenerationState::Progress>()
            .each(HandleGenerationEndingSystem);
    }

    void TerrainGenerationModule::RegisterHandler(flecs::world& world)
    {
        GenerationState::Done::OnEnter(world)
            .with<PendingReplication>()
            .each(DispatchPendingReplication);
    }

    void TerrainGenerationModule::InitializeGenerator(const flecs::world& world)
    {
        mGenerator.Setup(world);
    }

    flecs::entity
    TerrainGenerationModule::LaunchGenerationTask(const flecs::world& world, const glm::ivec3& position) const
    {
        const auto ctx    = ServerWorldContext::Get(world);
        const auto entity = world.entity()
            .is_a<ChunkPrefab>()
            .set<ChunkPosition>({ position })
            .child_of<SceneRoot>();

        GenerationState::Planned::Enter(entity);
        auto task = ctx->scheduler.Insert([=, this] { return mGenerator.Generate(position); }).AsUnique().Enqueue();

        entity.emplace<PendingChunk>(std::move(task));
        // Should be set in the task
        GenerationState::Progress::Enter(entity);

        ctx->chunkMap[position] = entity.id();

        return entity;
    }


}
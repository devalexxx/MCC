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

        AutoRegister<CChunkGenTask>      ::Register(world, "CChunkGenTask");
        AutoRegister<CPendingReplication>::Register(world, "CPendingReplication");
    }

    void TerrainGenerationModule::RegisterPrefab(flecs::world& /* world */) {}

    void TerrainGenerationModule::RegisterSystem(flecs::world& world)
    {
        world.system("SetupBlockRegistry")
            .kind<Phase::OnLoad>()
            .run(SetupBlockRegistrySystem);

        world.system<CChunkGenTask>("HandleGenerationEnding")
            .kind<Phase::OnSetup>()
            .with<GenerationState, GenerationState::Progress>()
            .each(HandleGenerationEndingSystem);
    }

    void TerrainGenerationModule::RegisterObserver(flecs::world& world)
    {
        GenerationState::Done::OnEnter(world)
            .with<CPendingReplication>()
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
            .is_a<PChunk>()
            .set<CChunkPos>(position)
            .child_of<SceneRoot>();

        GenerationState::Planned::Enter(entity);
        auto task = ctx->scheduler.Insert([=, this] { return mGenerator.Generate(position); }).AsUnique().Enqueue();

        entity.emplace<CChunkGenTask>(std::move(task));
        // Should be set in the task
        GenerationState::Progress::Enter(entity);

        ctx->chunkMap[position] = entity.id();

        return entity;
    }


}
// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/Renderer/Module.h"

#include "Client/Module/TerrainRenderer/Component.h"
#include "Client/Module/TerrainRenderer/Module.h"
#include "Client/Module/TerrainRenderer/System.h"
#include "Client/Scene/Scene.h"
#include "Client/WorldContext.h"

#include "Common/Module/Terrain/Component.h"
#include "Common/Phase.h"

namespace Mcc
{

    TerrainRendererModule::TerrainRendererModule(flecs::world& world) : BaseModule(world)
    {}

    void TerrainRendererModule::RegisterComponent(flecs::world& world)
    {
        world.component<ShouldBuildMeshTag>();
        world.component<CouldRenderChunkTag>();
        world.component<ShouldRenderChunkTag>();

        world.component<ChunkMesh>();
        world.component<MeshHolder>();
    }

    void TerrainRendererModule::RegisterSystem(flecs::world& world)
    {
        world.system<const ChunkHolder, const ChunkPosition>("BuildChunkMesh")
            .kind<Phase::PostUpdate>()
            .with<ShouldBuildMeshTag>()
            .each(BuildChunkMeshSystem)
            .add<GameScene>();

        world.system<MeshHolder>("SetupChunkRenderingMesh")
            .kind<Phase::PostUpdate>()
            .each([this](const flecs::entity entity, MeshHolder& holder) {
                SetupChunkRenderingMeshSystem(entity, holder);
            })
            .add<GameScene>();

        world.system("SetupChunkProgram").kind<Phase::OnLoad>().run([this](auto&&... args) {
            SetupChunkProgramSystem(args...);
        });

        world.system<const ChunkPosition, const ChunkMesh>("RenderChunkMeshSystem")
            .kind<Phase::OnDraw>()
            .with<ShouldRenderChunkTag>()
            .run([this](auto&&... args) { RenderChunkMeshSystem(args...); })
            .add<GameScene>();
    }

    void TerrainRendererModule::RegisterHandler(flecs::world& world)
    {
        const auto* ctx = ClientWorldContext::Get(world);

        world.observer<const Transform>("OnPlayerMove")
            .event(flecs::OnSet)
            .term_at(0)
            .src(world.entity(*ctx->networkMapping.GetLHandle(ctx->playerInfo.handle)))
            .each(OnPlayerMoveObserver);

        world.observer<const ChunkPosition>("OnChunkCreated")
            .event(flecs::OnSet)
            .each(OnChunkCreatedObserver);

        world.observer<const ChunkHolder>("OnChunkChanged")
            .event(flecs::OnSet)
            .with<CouldRenderChunkTag>()
            .each(OnChunkChangedObserver);
    }


}
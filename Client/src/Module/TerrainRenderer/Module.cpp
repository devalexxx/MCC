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

    TerrainRendererModule::TerrainRendererModule(flecs::world& world) :
        BaseModule(world),
        programEntity     (flecs::entity::null()),
        textureArrayEntity(flecs::entity::null()),
        program     (std::make_shared<OpenGLProgram>()),
        textureArray(std::make_shared<OpenGLTexture2DArray>()),
        textureIndex (std::make_shared<SafeAccess<std::unordered_map<std::string, size_t>>>()),
        textureToLoad(std::make_shared<SafeAccess<std::vector<std::string>>>())
    {}

    void TerrainRendererModule::RegisterComponent(flecs::world& world)
    {
        world.component<TShouldBuildMesh>();
        world.component<TCouldRenderChunk>();
        world.component<TShouldRenderChunk>();

        world.component<CChunkMeshGenTask>("CChunkMeshGenTask");
    }

    void TerrainRendererModule::RegisterPrefab(flecs::world& /* world */) {}

    void TerrainRendererModule::RegisterSystem(flecs::world& world)
    {
        world.system("SetupChunkProgram")
            .kind<Phase::OnLoad>()
            .run(SetupChunkProgramSystem);

        world.system("LoadTexture")
            .kind<Phase::OnSetup>()
            .run(LoadTextureSystem);

        world.system("HandleDirtyChunk")
            .kind<Phase::OnRender>() // TODO: maybe on pre-update
            .with<TChunk>()
            .with<TDirty>()
            .each(HandleDirtyChunkSystem)
            .add<GameScene>();

        world.system<const CChunkPtr, const CChunkPos>("BuildChunkMesh")
            .kind<Phase::PostUpdate>()
            .with<TShouldBuildMesh>()
            .with<TCouldRenderChunk>()
            .each(BuildChunkMeshSystem)
            .add<GameScene>();

        world.system<CChunkMeshGenTask>("SetupChunkRenderingMesh")
            .kind<Phase::PostUpdate>()
            .each(SetupChunkRenderingMeshSystem)
            .add<GameScene>();
    }

    void TerrainRendererModule::RegisterObserver(flecs::world& world)
    {
        const auto* ctx = ClientWorldContext::Get(world);

        world.observer<const CEntityTransform>("OnPlayerMove")
            .event(flecs::OnSet)
            .term_at(0)
            .src(world.entity(*ctx->networkMapping.GetLHandle(ctx->playerInfo.handle)))
            .each(OnPlayerMoveObserver);

        world.observer<const CChunkPos>("OnChunkCreated")
            .event(flecs::OnSet)
            .each(OnChunkCreatedObserver);
    }


}
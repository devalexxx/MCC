// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Module/Terrain/Module.h"

#include "Common/Module/Network/Component.h"
#include "Common/Module/Terrain/Component.h"

namespace Mcc
{

    TerrainModule::TerrainModule(flecs::world& world) : BaseModule(world) {}

    void TerrainModule::RegisterComponent(flecs::world& world)
    {
        world.component<TBlock>();
        world.component<TBlockState>();
        world.component<TChunk>();

        world.component<PBlock>();
        world.component<PChunk>();

        world.component<RBlockState>();

        world.component<CBlockColor>("CBlockColor")
            .is_a<CBlockColor::Type>();

        world.component<CBlockMeta>()
            .member<std::string>("id");

        world.component<CBlockType>();

        world.component<CChunkPtr>("CChunkPtr");
        world.component<CChunkPos>("CChunkPos")
            .is_a<CChunkPos::Type>();
    }

    void TerrainModule::RegisterPrefab(flecs::world& world)
    {
        world.prefab<PBlock>()
            .is_a<PNetObject>()
            .add<TBlock>()
            .add<CBlockMeta>()
            .add<CBlockColor>()
            .add<CBlockType>();

        world.prefab<PChunk>()
            .is_a<PNetObject>()
            .add<TChunk>()
            .add<CChunkPos>();
    }

    void TerrainModule::RegisterSystem(flecs::world& /* world */) {}

    void TerrainModule::RegisterObserver(flecs::world& /* world */) {}

}
// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Module/Terrain/Module.h"

#include "Common/Module/Network/Component.h"
#include "Common/Module/Terrain/Component.h"
#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

#include <memory>

namespace Mcc
{

    TerrainModule::TerrainModule(flecs::world& world) : BaseModule(world)
    {
        world.prefab<BlockPrefab>()
            .is_a<NetworkObjectPrefab>()
            .add<BlockTag>()
            .set_auto_override<BlockMeta>({})
            .set_auto_override<BlockColor>({})
            .set_auto_override<BlockType>({});

        world.prefab<ChunkPrefab>()
            .is_a<NetworkObjectPrefab>()
            .add<ChunkTag>()
            .add<ChunkPosition>()
            .set_auto_override<ChunkHolder>({});
    }

    void TerrainModule::RegisterComponent(flecs::world& world)
    {
        world.component<BlockTag>();
        world.component<BlockStateTag>();
        world.component<ChunkTag>();

        world.component<BlockPrefab>();
        world.component<ChunkPrefab>();

        world.component<BlockStateRelation>();

        world.component<BlockColor>();
        world.component<BlockMeta>();
        world.component<BlockType>();

        world.component<ChunkPosition>();
        world.component<ChunkHolder>();
    }

    void TerrainModule::RegisterSystem(flecs::world& /* world */)
    {}

    void TerrainModule::RegisterHandler(flecs::world& /* world */)
    {}

}
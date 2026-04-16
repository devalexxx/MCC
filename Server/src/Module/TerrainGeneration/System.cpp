// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/TerrainGeneration/System.h"

#include "Server/Module/TerrainGeneration/Module.h"
#include "Server/WorldContext.h"

#include "Common/SceneImporter.h"

namespace Mcc
{

    void SetupBlockRegistrySystem(flecs::iter& it)
    {
        const auto world = it.world();

        world.entity("mcc:block:air")
            .is_a<PBlock>()
            .set<CBlockType>(CBlockType::Gas)
            .set<CBlockMeta>({ "mcc:block:air" })
            .child_of<SceneRoot>();

        world.entity("mcc:block:stone")
            .is_a<PBlock>()
            .set<CBlockType>(CBlockType::Solid)
            .set<CBlockColor>({ .5f, .5f, .5f })
            .set<CBlockMeta>({ "mcc:block:stone" })
            .set<CBlockAsset>({
                .texture=std::string("texture://Stone/cubeStone_1.png")
            })
            .child_of<SceneRoot>();

        world.entity("mcc:block:dirt")
            .is_a<PBlock>()
            .set<CBlockType>(CBlockType::Solid)
            .set<CBlockColor>({ .0f, .7f, .3f })
            .set<CBlockMeta>({ "mcc:block:dirt" })
            .set<CBlockAsset>({
                .texture = std::string("texture://Dirt/cubeDirt_1.png")
            })
            .child_of<SceneRoot>();

        world.entity("mcc:block:grass")
            .is_a<PBlock>()
            .set<CBlockType>(CBlockType::Solid)
            .set<CBlockColor>({ .0f, .7f, .3f })
            .set<CBlockMeta>({ "mcc:block:dirt" })
            .set<CBlockAsset>({
                .texture = Hx::EnumArray<BlockFace, std::string>{
                    { BlockFace::Top   , "texture://Grass/cubeGreen_1-Top.png" },
                    { BlockFace::Bottom, "texture://Dirt/cubeDirt_1.png"       },
                    { BlockFace::Front , "texture://Grass/cubeGreen_1.png"     },
                    { BlockFace::Back  , "texture://Grass/cubeGreen_1.png"     },
                    { BlockFace::Left  , "texture://Grass/cubeGreen_1.png"     },
                    { BlockFace::Right , "texture://Grass/cubeGreen_1.png"     },
                }
            })
            .child_of<SceneRoot>();

        world.get_mut<TerrainGenerationModule>().InitializeGenerator(world);

        IgnoreIter(it);
    }

    void HandleGenerationEndingSystem(const flecs::entity entity, CChunkGenTask& task)
    {
        if (task.GetState() == Hx::TaskState::Cancelled)
        {
            entity.remove<CChunkGenTask>();
            GenerationState::Failed::Enter(entity);
            return;
        }

        if (task.GetState() == Hx::TaskState::Done)
        {
            const auto result = task.GetResult();
            MCC_ASSERT(result, "Chunk data has already been retrieve");
            entity.emplace<CChunkPtr>(result->get());
            entity.remove<CChunkGenTask>();
            GenerationState::Done::Enter(entity);
        }
    }

}
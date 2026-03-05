// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_MODULE_TERRAIN_COMPONENT_H
#define MCC_COMMON_MODULE_TERRAIN_COMPONENT_H

#include "Common/Utils/FlecsUtils.h"
#include "Common/World/Chunk.h"

#include <memory>
#include <string>

namespace Mcc
{

    struct TBlock      {};
    struct TBlockState {};
    struct TChunk      {};

    struct PBlock {};
    struct PChunk {};

    struct RBlockState {};

    enum class CBlockType
    {
        Solid,
        Fluid,
        Gas
    };

    struct CBlockMeta
    {
        std::string id;
    };

    namespace _ { struct TerrainModuleTag {}; };
    using CBlockColor = ComponentWrapper<glm::vec3             , _::TerrainModuleTag>;
    using CChunkPos   = ComponentWrapper<glm::ivec3            , _::TerrainModuleTag>;
    using CChunkPtr   = ComponentWrapper<std::shared_ptr<Chunk>, _::TerrainModuleTag>;

    template<typename Archive>
    void serialize(Archive& ar, CBlockMeta& packet);

}

#include "Common/Module/Terrain/Component.inl"

#endif
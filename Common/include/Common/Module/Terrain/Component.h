// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_MODULE_TERRAIN_COMPONENT_H
#define MCC_COMMON_MODULE_TERRAIN_COMPONENT_H

#include "Common/Utils/FlecsUtils.h"
#include "Common/World/Chunk.h"
#include "Common/World/Position.h"

#include <memory>
#include <string>
#include <variant>

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

    struct CBlockAsset
    {
        std::variant<std::string, Hx::EnumArray<BlockFace, std::string>> texture;
    };

    namespace _ { struct TerrainModuleTag {}; };
    using CBlockColor = ComponentWrapper<glm::vec3             , _::TerrainModuleTag>;
    using CChunkPos   = ComponentWrapper<ChunkPosV             , _::TerrainModuleTag>;
    using CChunkPtr   = ComponentWrapper<std::shared_ptr<Chunk>, _::TerrainModuleTag>;

    template<typename Archive>
    void serialize(Archive& ar, CBlockMeta& packet);
    template<typename Archive>
    void serialize(Archive& ar, CBlockAsset& packet);

}

#include "Common/Module/Terrain/Component.inl"

#endif
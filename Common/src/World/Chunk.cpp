// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/World/Chunk.h"

#include "Common/Module/Network/Component.h"
#include "Common/Module/Terrain/Component.h"
#include "Common/Utils/BlockUtils.h"
#include "Common/Utils/Logging.h"
#include "Common/WorldContext.h"

#include <algorithm>

namespace Mcc
{

    Chunk::Chunk() : mData({}, { Size * Size * Height, 2 })
    {}

    Chunk::Chunk(flecs::entity_t filler) : mData({}, { Size * Size * Height, 2 })
    {
        mData.palette.emplace_back(filler);
        for (size_t i = 0; i < mData.mapping.GetSize(); ++i) { mData.mapping.Set(i, 0); }
    }

    Chunk::Chunk(ChunkData<flecs::entity_t> data) : mData(std::move(data))
    {}

    flecs::entity_t Chunk::Get(const glm::ivec3 position) const
    {
        static auto null = flecs::entity::null().id();

        if (mData.palette.empty())
            return null;

        if (position.x < 0 || position.y < 0 || position.z < 0 || position.x >= Size || position.y >= Height ||
            position.z >= Size)
            return null;

        const size_t index = IndexFromPosition(position);
        return index < mData.mapping.GetSize() ? mData.palette[mData.mapping.Get(index)] : null;
    }

    Hx::EnumArray<BlockFace, flecs::entity_t> Chunk::GetNeighbors(const glm::ivec3 position) const
    {
        static glm::ivec3 left(-1, 0, 0);
        static glm::ivec3 right(1, 0, 0);
        static glm::ivec3 front(0, 0, 1);
        static glm::ivec3 back(0, 0, -1);
        static glm::ivec3 top(0, 1, 0);
        static glm::ivec3 bottom(0, -1, 0);

        return {
            { BlockFace::Left,   Get(position + left)   },
            { BlockFace::Right,  Get(position + right)  },
            { BlockFace::Front,  Get(position + front)  },
            { BlockFace::Back,   Get(position + back)   },
            { BlockFace::Top,    Get(position + top)    },
            { BlockFace::Bottom, Get(position + bottom) }
        };
    }

    void Chunk::Set(const glm::uvec3 position, const flecs::entity_t entity)
    {
        const size_t index = IndexFromPosition(position);

        if (index >= mData.mapping.GetSize())
            return;

        if (const auto it = std::ranges::find(mData.palette, entity); it == mData.palette.cend())
        {
            mData.palette.push_back(entity);
            mData.mapping.Set(index, mData.palette.size() - 1);
        }
        else
        {
            mData.mapping.Set(index, std::distance(mData.palette.begin(), it));
        }
    }

    const Chunk::Palette& Chunk::GetPalette() const
    {
        return mData.palette;
    }

    const BitArray& Chunk::GetMapping() const
    {
        return mData.mapping;
    }

    std::optional<RLEChunkData> Chunk::ToNetwork(const flecs::world& world) const
    {
        return Helper::ToNetwork(mData, world);
    }

    size_t Chunk::IndexFromPosition(const glm::uvec3 position)
    {
        return position.x + (position.y * Size) + (position.z * Size * Height);
    }

    namespace Helper
    {

        std::optional<RLEChunkData> ToNetwork(const ChunkData<flecs::entity_t>& data, const flecs::world& world)
        {
            RLEChunkData compressed;

            size_t currentIndex = 0;
            size_t count        = 0;
            for (size_t i = 0; i < data.mapping.GetSize(); ++i)
            {
                const auto index = data.mapping.Get(i);
                if (index == currentIndex)
                {
                    ++count;
                    if (i < data.mapping.GetSize() - 1)
                        continue;
                }

                if (count > 0)
                {
                    const auto id = data.palette[currentIndex];
                    const auto e  = world.entity(id);
                    if (const auto props = e.try_get<CNetProps>(); props)
                    {
                        if (!IsValid(props->handle))
                        {
                            MCC_LOG_WARN("[RLECompression] The network id assigned to the block({}, #{}) is invalid", GetBlockName(e), id);
                            return std::nullopt;
                        }

                        compressed.data.emplace_back(props->handle, count);
                    }
                    else
                    {
                        MCC_LOG_WARN("[RLECompression] No network id assigned to block({}, #{})", GetBlockName(e), id);
                        return std::nullopt;
                    }
                }

                currentIndex = index;
                count        = 1;
            }

            return compressed;
        }

        std::optional<ChunkData<flecs::entity_t>> FromNetwork(const RLEChunkData& rle, const flecs::world& world)
        {
            const auto* ctx = WorldContext<>::Get(world);

            ChunkData<flecs::entity_t> data {
                .palette={},
                .mapping={ Chunk::Size * Chunk::Size * Chunk::Height, 2 }
            };
            size_t currentOffset = 0;
            for (auto [handle, count]: rle.data)
            {
                if (const auto id = ctx->networkMapping.GetLHandle(handle); id.has_value())
                {
                    if (!world.is_valid(*id))
                    {
                        MCC_LOG_WARN("[RLEDecompression] The local id assigned to the block({}) is invalid", handle);
                        return std::nullopt;
                    }

                    size_t index;
                    if (auto it = std::ranges::find(data.palette, *id); it != data.palette.cend())
                    {
                        index = std::distance(data.palette.begin(), it);
                    }
                    else
                    {
                        index = data.palette.size();
                        data.palette.push_back(*id);
                    }

                    for (size_t i = currentOffset; i < currentOffset + count; ++i) { data.mapping.Set(i, index); }

                    currentOffset += count;
                }
                else
                {
                    MCC_LOG_WARN("[RLEDecompression] No local id attached to the block({})", handle);
                    return std::nullopt;
                }
            }

            return data;
        }

    }
}
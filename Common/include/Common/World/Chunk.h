// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_WORLD_CHUNK_H
#define MCC_COMMON_WORLD_CHUNK_H

#include "Common/Network/NetworkHandle.h"
#include "Common/Utils/BitArray.h"

#include <Hexis/Core/EnumArray.h>
#include <glm/glm.hpp>

#include <flecs.h>

#include <optional>
#include <shared_mutex>
#include <vector>

namespace Mcc
{

    enum class BlockFace
    {
        Left,
        Right,
        Front,
        Back,
        Top,
        Bottom,
        Count
    };

    template<typename T>
    struct ChunkData
    {
        using Palette = std::vector<T>;

        Palette  palette {};
        BitArray mapping { 0, 1 };
    };

    struct RLEChunkData
    {
        std::vector<std::tuple<NetworkHandle, size_t>> data;
    };

    template<typename Archive, typename T>
    void serialize(Archive& ar, ChunkData<T>& data);
    template<typename Archive>
    void serialize(Archive& ar, RLEChunkData& data);

    class MCC_LIB_API Chunk
    {
      public:
        using Palette = ChunkData<flecs::entity_t>::Palette;

        static constexpr unsigned char Size   = 31;
        static constexpr unsigned char Height = 127;

        Chunk();
        Chunk(flecs::entity_t filler);
        Chunk(ChunkData<flecs::entity_t> data);

        bool IsValid() const;

        flecs::entity_t Get(glm::ivec3 position) const;
        bool            Set(glm::uvec3 position, flecs::entity_t entity);

        size_t GetPaletteIndex(size_t index) const;
        size_t GetPaletteIndex(glm::uvec3 position) const;

        Hx::EnumArray<BlockFace, flecs::entity_t> GetNeighbors(glm::ivec3 position) const;

        const Palette & GetPalette() const;
        const BitArray& GetMapping() const;

        std::optional<RLEChunkData> ToNetwork(const flecs::world& world) const;
        std::optional<RLEChunkData> ToNetwork(const std::unordered_map<flecs::entity_t, NetworkHandle>& mapping) const;

        void LockReadOnly () const;
        void LockReadWrite() const;
        void UnlockReadOnly () const;
        void UnlockReadWrite() const;

        friend bool operator==(const Chunk& lhs, size_t rhs);
        friend bool operator!=(const Chunk& lhs, size_t rhs);

      private:
        friend struct std::hash<Chunk>;

        mutable std::shared_mutex  mMutex;
        ChunkData<flecs::entity_t> mData;
        size_t                     mVersion;

        static size_t IndexFromPosition(glm::uvec3 position);
    };

    namespace Helper
    {

        MCC_LIB_API std::optional<RLEChunkData>               ToNetwork  (const ChunkData<flecs::entity_t>& data, const flecs::world& world);
        MCC_LIB_API std::optional<ChunkData<flecs::entity_t>> FromNetwork(const RLEChunkData& rle, const flecs::world& world);


        MCC_LIB_API std::optional<RLEChunkData> ToNetwork(
            const ChunkData<flecs::entity_t>& data, const std::unordered_map<flecs::entity_t, NetworkHandle>& mapping
        );
        MCC_LIB_API std::optional<ChunkData<flecs::entity_t>> FromNetwork(
            const RLEChunkData& rle, const std::unordered_map<NetworkHandle, flecs::entity_t>& mapping
        );


    }

}

namespace std
{

    template<>
    struct hash<Mcc::Chunk>
    {
        inline size_t operator()(const Mcc::Chunk& c) const noexcept
        {
            size_t hash = c.mVersion;
            for (const auto a : c.mData.palette)
            {
                hash ^= a;
            }

            hash ^= c.mData.palette.size();
            hash ^= c.mData.mapping.GetStride();

            return hash;
        }
    };

}

#include "Common/World/Chunk.inl"

#endif

// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_WORLD_POSITION_H
#define MCC_COMMON_WORLD_POSITION_H



#include "Common/World/Coordinate.h"
#include "Common/Utils/GLMUtils.h"
#include "Common/Utils/MathUtils.h"
#include "Common/World/Chunk.h"

#include <glm/glm.hpp>

namespace Mcc
{

    template<typename, typename>
    class Position {};

    template<typename C>
    using WorldPos = Position<WorldSpace, C>;
    template<typename C>
    using ChunkPos = Position<ChunkSpace, C>;
    template<typename C>
    using LocalPos = Position<LocalSpace, C>;

    using ChunkPosV = Position<ChunkSpace, VoxelCoord>;
    using LocalPosE = Position<LocalSpace, EnttyCoord>;
    using LocalPosV = Position<LocalSpace, VoxelCoord>;
    using WorldPosV = Position<WorldSpace, VoxelCoord>;
    using WorldPosE = Position<WorldSpace, EnttyCoord>;
    using WorldPosF = Position<WorldSpace, FloatCoord>;

    /**
     * Position of chunk in world space
     */
    template<>
    class Position<ChunkSpace, VoxelCoord>
    {
      public:
        using type = glm::ivec2;

        constexpr Position() = default;
        constexpr Position(glm::ivec2 position);
        constexpr Position(glm::ivec3 position);
        constexpr Position(int32_t x, int32_t z);
        constexpr Position(std::integral auto x, std::integral auto z);

        constexpr operator glm::ivec2() const;
        constexpr operator WorldPosF () const;

        template<std::size_t>
        friend constexpr auto get(Position p);

        friend constexpr bool operator==(const Position& lhs, const Position& rhs);
        friend constexpr bool operator!=(const Position& lhs, const Position& rhs);

        template<typename Archive>
        friend constexpr void serialize(Archive& ar, Position& position);

      private:
        glm::ivec2 mPosition;
    };

    /**
     * Position of a block in a chunk
     */
    template<>
    class Position<LocalSpace, VoxelCoord>
    {
      public:
        using type = glm::uvec3;

        constexpr Position() = default;
        constexpr Position(glm::uvec3 position);
        constexpr Position(uint32_t x, uint32_t y, uint32_t z);

        constexpr operator LocalPosE () const;
        constexpr operator glm::uvec3() const;

        friend constexpr bool operator==(const Position& lhs, const Position& rhs);
        friend constexpr bool operator!=(const Position& lhs, const Position& rhs);

        template<typename Archive>
        friend constexpr void serialize(Archive& ar, Position& position);

      private:
        glm::uvec3 mPosition;
    };

    /**
     * Position of an entity in a chunk
     */
    template<>
    class Position<LocalSpace, EnttyCoord>
    {
      public:
        using type = glm::vec3;

        constexpr Position() = default;
        constexpr Position(glm::fvec3 position);
        constexpr Position(float x, float y, float z);

        constexpr operator LocalPosV() const;
        constexpr operator glm::vec3() const;

        friend constexpr bool operator==(const Position& lhs, const Position& rhs);
        friend constexpr bool operator!=(const Position& lhs, const Position& rhs);

        template<typename Archive>
        friend constexpr void serialize(Archive& ar, Position& position);

      private:
        glm::fvec3 mPosition;
    };

    /**
     * Position of a block in world space (e.g., a chunk position in world space and a block position in chunk space)
     */
    template<>
    class Position<WorldSpace, VoxelCoord>
    {
      public:
        constexpr Position() = default;
        constexpr Position(ChunkPosV parent, LocalPosV local);

        constexpr operator WorldPosE() const;
        constexpr operator WorldPosF() const;

        template<std::size_t>
        friend constexpr auto get(Position p);

        friend constexpr bool operator==(const Position& lhs, const Position& rhs);
        friend constexpr bool operator!=(const Position& lhs, const Position& rhs);

        template<typename Archive>
        friend constexpr void serialize(Archive& ar, Position& position);

      private:
        ChunkPosV mParent;
        LocalPosV mLocal;
    };

    /**
     * Position of an entity in world space (e.g., a chunk position in world space and an entity position in chunk space)
     */
    template<>
    class Position<WorldSpace, EnttyCoord>
    {
      public:
        constexpr Position() = default;
        constexpr Position(ChunkPosV parent, LocalPosE local);

        constexpr operator WorldPosV() const;
        constexpr operator WorldPosF() const;

        template<std::size_t>
        friend constexpr auto get(Position p);

        friend constexpr bool operator==(const Position& lhs, const Position& rhs);
        friend constexpr bool operator!=(const Position& lhs, const Position& rhs);

        template<typename Archive>
        friend constexpr void serialize(Archive& ar, Position& position);

      private:
        ChunkPosV mParent;
        LocalPosE mLocal;
    };

    /**
     * Position of anything in the world. With Chunk (0, 0)(0, 0, 0) for origin
     */
    template<>
    class Position<WorldSpace, FloatCoord>
    {
      public:
        constexpr Position() = default;
        constexpr Position(glm::fvec3 position);
        constexpr Position(float x, float y, float z);

        constexpr operator glm::fvec3() const;

        constexpr operator WorldPosV() const;
        constexpr operator WorldPosE() const;
        constexpr operator ChunkPosV() const;

        friend constexpr bool operator==(const Position& lhs, const Position& rhs);
        friend constexpr bool operator!=(const Position& lhs, const Position& rhs);

        template<typename Archive>
        friend constexpr void serialize(Archive& ar, Position& position);

      private:
        glm::fvec3 mPosition;
    };

    template<std::size_t I>
    constexpr auto get(ChunkPosV p)
    {
        if constexpr (I == 0) return p.mPosition.x;
        else                  return p.mPosition.y;
    }

    template<std::size_t I>
    constexpr auto get(WorldPosV p)
    {
        if constexpr (I == 0) return p.mParent;
        else                  return p.mLocal;
    }

    template<std::size_t I>
    constexpr auto get(WorldPosE p)
    {
        if constexpr (I == 0) return p.mParent;
        else                  return p.mLocal;
    }

    constexpr auto format_as(const ChunkPosV& position);
    constexpr auto format_as(const LocalPosV& position);
    constexpr auto format_as(const LocalPosE& position);
    constexpr auto format_as(const WorldPosV& position);
    constexpr auto format_as(const WorldPosE& position);
    constexpr auto format_as(const WorldPosF& position);

}

namespace std
{

    template<>
    struct tuple_size<Mcc::ChunkPosV> : std::integral_constant<size_t, 2> {};

    template<size_t N>
    struct tuple_element<N, Mcc::ChunkPosV>
    {
        using type = decltype(Mcc::get<N>(std::declval<Mcc::ChunkPosV>()));
    };

    template<>
    struct tuple_size<Mcc::WorldPosV> : std::integral_constant<size_t, 2> {};

    template<size_t N>
    struct tuple_element<N, Mcc::WorldPosV>
    {
        using type = decltype(Mcc::get<N>(std::declval<Mcc::WorldPosV>()));
    };

    template<>
    struct tuple_size<Mcc::WorldPosE> : std::integral_constant<size_t, 2> {};

    template<size_t N>
    struct tuple_element<N, Mcc::WorldPosE>
    {
        using type = decltype(Mcc::get<N>(std::declval<Mcc::WorldPosE>()));
    };

}

#include "Common/World/Position.inl"

#endif

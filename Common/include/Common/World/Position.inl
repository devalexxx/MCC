// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Utils/Logging.h"
#include "Common/Utils/Assert.h"
#include "Common/Utils/GLMUtils.h"

#include <glm/gtc/epsilon.hpp>
#include <glm/glm.hpp>


namespace Mcc
{

    constexpr Position<ChunkSpace, VoxelCoord>::Position(const data_type position) : mPosition(position) {}

    constexpr Position<ChunkSpace, VoxelCoord>::Position(
        std::integral auto x, std::integral auto y, std::integral auto z
    ) :
        mPosition(x, y, z)
    {}

    template<typename Archive>
    constexpr void serialize(Archive& ar, ChunkPosV& position)
    {
        ar(position.mPosition);
    }

    constexpr Position<ChunkSpace, VoxelCoord>::operator data_type() const
    {
        return mPosition;
    }

    constexpr Position<ChunkSpace, VoxelCoord>::operator Position<WorldSpace, FloatCoord>() const
    {
        return { glm::vec3(mPosition) * glm::vec3(Chunk::Size) };
    }

    constexpr bool
    operator==(const Position<ChunkSpace, VoxelCoord>& lhs, const Position<ChunkSpace, VoxelCoord>& rhs)
    {
        return lhs.mPosition == rhs.mPosition;
    }

    constexpr bool
    operator!=(const Position<ChunkSpace, VoxelCoord>& lhs, const Position<ChunkSpace, VoxelCoord>& rhs)
    {
        return !(lhs == rhs);
    }

    constexpr Position<LocalSpace, VoxelCoord>::Position(const glm::uvec3 position) : mPosition(position)
    {
        MCC_ASSERT(
            position.x < Chunk::Size && position.y < Chunk::Size && position.z < Chunk::Size,
            "Positon<LocalSpace, VoxelCoord>({}, {}, {}) out of bound", position.x, position.y, position.z
        );
    }

    constexpr Position<LocalSpace, VoxelCoord>::Position(const uint32_t x, const uint32_t y, const uint32_t z) :
        Position(glm::uvec3(x, y, z))
    {}

    constexpr Position<LocalSpace, VoxelCoord>::operator Position<LocalSpace, EnttyCoord>() const
    {
        return { glm::vec3(mPosition) / glm::vec3(Chunk::Size) };
    }

    constexpr Position<LocalSpace, VoxelCoord>::operator glm::uvec3() const
    {
        return mPosition;
    }

    constexpr bool
    operator==(const Position<LocalSpace, VoxelCoord>& lhs, const Position<LocalSpace, VoxelCoord>& rhs)
    {
        return lhs.mPosition == rhs.mPosition;
    }

    constexpr bool
    operator!=(const Position<LocalSpace, VoxelCoord>& lhs, const Position<LocalSpace, VoxelCoord>& rhs)
    {
        return !(lhs == rhs);
    }

    template<typename Archive>
    constexpr void serialize(Archive& ar, LocalPosV& position)
    {
        ar(position.mPosition);
    }

    constexpr Position<LocalSpace, EnttyCoord>::Position(const glm::fvec3 position) : mPosition(position)
    {
        MCC_ASSERT(
            position.x >= 0 && position.x < 1.0f && position.z >= 0 && position.z < 1.0f,
            "Positon<ChunkSpace, EntityCoord>({}, {}, {}) out of bound, x and z must be on grid (e.g., between 0 and 1)",
            position.x, position.y, position.z
        );
    }

    constexpr Position<LocalSpace, EnttyCoord>::Position(const float x, const float y, const float z) :
        Position(glm::fvec3(x, y, z))
    {}

    constexpr Position<LocalSpace, EnttyCoord>::operator Position<LocalSpace, VoxelCoord>() const
    {
        return { glm::floor(mPosition * glm::vec3(Chunk::Size)) };
    }

    constexpr Position<LocalSpace, EnttyCoord>::operator glm::vec3() const
    {
        return mPosition;
    }

    constexpr bool
    operator==(const Position<LocalSpace, EnttyCoord>& lhs, const Position<LocalSpace, EnttyCoord>& rhs)
    {
        return glm::all(glm::epsilonEqual(glm::vec3(lhs), glm::vec3(rhs), std::numeric_limits<float>::epsilon()));
    }

    constexpr bool
    operator!=(const Position<LocalSpace, EnttyCoord>& lhs, const Position<LocalSpace, EnttyCoord>& rhs)
    {
        return !(lhs == rhs);
    }

    template<typename Archive>
    constexpr void serialize(Archive& ar, LocalPosE& position)
    {
        ar(position.mPosition);
    }

    constexpr Position<WorldSpace, VoxelCoord>::Position(const ChunkPosV parent, const LocalPosV local) :
        mParent(parent),
        mLocal (local)
    {}

    constexpr Position<WorldSpace, VoxelCoord>::operator Position<WorldSpace, EnttyCoord>() const
    {
        return { mParent, mLocal };
    }

    constexpr Position<WorldSpace, VoxelCoord>::operator Position<WorldSpace, FloatCoord>() const
    {
        constexpr auto s = glm::vec3(Chunk::Size);

        const decltype(mParent)::data_type c = mParent;
        const decltype(mLocal) ::data_type i = mLocal;

        return { glm::vec3(c) * s + glm::vec3(i) };
    }

    constexpr bool
    operator==(const Position<WorldSpace, VoxelCoord>& lhs, const Position<WorldSpace, VoxelCoord>& rhs)
    {
        return lhs.mParent == rhs.mParent && lhs.mLocal == rhs.mLocal;
    }

    constexpr bool
    operator!=(const Position<WorldSpace, VoxelCoord>& lhs, const Position<WorldSpace, VoxelCoord>& rhs)
    {
        return !(lhs == rhs);
    }

    template<typename Archive>
    constexpr void serialize(Archive& ar, WorldPosV& position)
    {
        ar(position.mParent, position.mLocal);
    }

    constexpr Position<WorldSpace, EnttyCoord>::Position(const ChunkPosV parent, const LocalPosE local) :
        mParent(parent),
        mLocal (local)
    {}

    constexpr Position<WorldSpace, EnttyCoord>::operator Position<WorldSpace, VoxelCoord>() const
    {
        return { mParent, mLocal };
    }

    constexpr Position<WorldSpace, EnttyCoord>::operator Position<WorldSpace, FloatCoord>() const
    {
        constexpr auto s = glm::vec3(Chunk::Size);

        const decltype(mParent)::data_type c = mParent;
        const decltype(mLocal) ::data_type i = mLocal;

        return { glm::vec3(c) * s + i * s };
    }

    constexpr bool
    operator==(const Position<WorldSpace, EnttyCoord>& lhs, const Position<WorldSpace, EnttyCoord>& rhs)
    {
        return lhs.mParent == rhs.mParent && lhs.mLocal == rhs.mLocal;
    }

    constexpr bool
    operator!=(const Position<WorldSpace, EnttyCoord>& lhs, const Position<WorldSpace, EnttyCoord>& rhs)
    {
        return !(lhs == rhs);
    }

    template<typename Archive>
    constexpr void serialize(Archive& ar, WorldPosE& position)
    {
        ar(position.mParent, position.mLocal);
    }

    constexpr Position<WorldSpace, FloatCoord>::Position(const glm::fvec3 position) : mPosition(position) {}

    constexpr Position<WorldSpace, FloatCoord>::Position(const float x, const float y, const float z) :
        Position(glm::fvec3(x, y, z))
    {}

    constexpr Position<WorldSpace, FloatCoord>::Position(const float value) : mPosition(value, value, value) {}

    constexpr Position<WorldSpace, FloatCoord>::operator glm::fvec3() const
    {
        return mPosition;
    }

    inline Position<WorldSpace, FloatCoord>::operator Position<WorldSpace, VoxelCoord>() const
    {
        constexpr float size   = Chunk::Size;

        const auto    pf = glm::ivec3(glm::floor(mPosition));
        const int32_t cx = glm::floor(mPosition.x / size);
        const int32_t cy = glm::floor(mPosition.y / size);
        const int32_t cz = glm::floor(mPosition.z / size);

        const uint32_t px = (Chunk::Size + pf.x % Chunk::Size) % Chunk::Size;
        const uint32_t py = (Chunk::Size + pf.y % Chunk::Size) % Chunk::Size;
        const uint32_t pz = (Chunk::Size + pf.z % Chunk::Size) % Chunk::Size;

        return {{ cx, cy, cz }, { px, py, pz }};
    }

    inline Position<WorldSpace, FloatCoord>::operator Position<WorldSpace, EnttyCoord>() const
    {
        constexpr float size = Chunk::Size;

        const int32_t cx = glm::floor(mPosition.x / size);
        const int32_t cy = glm::floor(mPosition.y / size);
        const int32_t cz = glm::floor(mPosition.z / size);

        // split fractional and integer part
        // remove chunk info to position (e.g., remove c_ * size)
        // make position positive (with + size)
        float fix, fiy, fiz;
        const float fx = glm::modf(size + mPosition.x - cx * size, fix);
        const float fy = glm::modf(size + mPosition.y - cy * size, fiy);
        const float fz = glm::modf(size + mPosition.z - cz * size, fiz);

        // Short way maybe
        // glm::ivec3 c, i;
        // glm::fvec3 f, p, fi;
        // c = glm::floor(mPosition / size);
        // f = glm::modf(size + mPosition - glm::vec3(c) * size, fi);
        // i = fi;
        // i %= Chunk::Size;
        // p = (glm::vec3(i) + f) / size;
        //
        // return { c, p };

        // cast fix & fiz to int to enable % operation
        int32_t ix = fix;
        int32_t iy = fiy;
        int32_t iz = fiz;

        // remove added size (for positive position)
        ix %= Chunk::Size;
        iy %= Chunk::Size;
        iz %= Chunk::Size;

        const float px = (ix + fx) / size;
        const float py = (iy + fy) / size;
        const float pz = (iz + fz) / size;

        return {{ cx, cy, cz }, { px, py, pz }};
    }

    inline Position<WorldSpace, FloatCoord>::operator Position<ChunkSpace, VoxelCoord>() const
    {
        return { glm::floor(mPosition / glm::vec3(Chunk::Size)) };
    }

    constexpr bool
    operator==(const Position<WorldSpace, FloatCoord>& lhs, const Position<WorldSpace, FloatCoord>& rhs)
    {
        return glm::all(glm::epsilonEqual(lhs.mPosition, rhs.mPosition, std::numeric_limits<float>::epsilon()));
    }

    constexpr bool
    operator!=(const Position<WorldSpace, FloatCoord>& lhs, const Position<WorldSpace, FloatCoord>& rhs)
    {
        return !(lhs == rhs);
    }

    template<typename Archive>
    constexpr void serialize(Archive& ar, WorldPosF& position)
    {
        ar(position.mPosition);
    }

    inline auto format_as(const ChunkPosV& position)
    {
        return fmt::format("ChunkPosV({})", static_cast<ChunkPosV::data_type>(position));
    }

    inline auto format_as(const LocalPosV& position)
    {
        return fmt::format("LocalPosV({})", static_cast<LocalPosV::data_type>(position));
    }

    inline auto format_as(const LocalPosE& position)
    {
        return fmt::format("LocalPosE({})", static_cast<LocalPosE::data_type>(position));
    }

    inline auto format_as(const WorldPosV& position)
    {
        auto [ parent, local ] = position;
        return fmt::format("WorldPosV({}, {})", parent, local );
    }

    inline auto format_as(const WorldPosE& position)
    {
        auto [ parent, local ] = position;
        return fmt::format("WorldPosE({}, {})", parent, local );
    }

    inline auto format_as(const WorldPosF& position)
    {
        return fmt::format("WorldPosF({})", static_cast<glm::vec3>(position) );
    }

}
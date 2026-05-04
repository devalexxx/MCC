// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

namespace Mcc
{

    constexpr TranslationV operator-(const ChunkPosV& lhs, const ChunkPosV& rhs)
    {
        using T = ChunkPosV::data_type;
        const auto v = static_cast<T>(lhs) - static_cast<T>(rhs);
        return { v * T(Chunk::Size) };
    }

    constexpr TranslationV operator-(const LocalPosV& lhs, const LocalPosV& rhs)
    {

        return { glm::ivec3(glm::uvec3(lhs)) - glm::ivec3(glm::uvec3(rhs)) };
    }

    constexpr TranslationV operator-(const WorldPosV& lhs, const WorldPosV& rhs)
    {
        auto [ lhsParent, lhsLocal ] = lhs;
        auto [ rhsParent, rhsLocal ] = rhs;

        return { lhsParent - rhsParent + (lhsLocal - rhsLocal) };
    }

    constexpr TranslationE operator-(const LocalPosE& lhs, const LocalPosE& rhs)
    {
        return { glm::vec3(lhs) - glm::vec3(rhs) };
    }

    constexpr TranslationE operator-(const WorldPosE& lhs, const WorldPosE& rhs)
    {
        auto [ lhsParent, lhsLocal ] = lhs;
        auto [ rhsParent, rhsLocal ] = rhs;

        return { TranslationE(lhsParent - rhsParent) + (lhsLocal - rhsLocal) };
    }

    constexpr TranslationF operator-(const WorldPosF& lhs, const WorldPosF& rhs)
    {
        return { glm::vec3(lhs) - glm::vec3(rhs) };
    }

    constexpr Position<ChunkSpace, VoxelCoord>
    operator+(const Position<ChunkSpace, VoxelCoord>& lhs, const Translation<VoxelCoord>& rhs)
    {
        const Position<ChunkSpace, VoxelCoord>::data_type cLhs = lhs;
        const Translation<VoxelCoord>::data_type          cRhs = rhs;
        return { cLhs + cRhs };
    }

    constexpr Position<ChunkSpace, VoxelCoord>
    operator-(const Position<ChunkSpace, VoxelCoord>& lhs, const Translation<VoxelCoord>& rhs)
    {
        const Position<ChunkSpace, VoxelCoord>::data_type cLhs = lhs;
        const Translation<VoxelCoord>::data_type          cRhs = rhs;
        return { cLhs - cRhs };
    }

    static std::tuple<LocalPosV, TranslationV> ComputeOverlap(const glm::ivec3& inV)
    {
        glm::ivec3 v = inV;
        glm::ivec3 c(0, 0, 0);
        c.x = glm::floor(static_cast<float>(v.x) / Chunk::Size);
        c.y = glm::floor(static_cast<float>(v.y) / Chunk::Size);
        c.z = glm::floor(static_cast<float>(v.z) / Chunk::Size);

        v.x %= Chunk::Size;
        v.y %= Chunk::Size;
        v.z %= Chunk::Size;

        const uint32_t px = (Chunk::Size + v.x % Chunk::Size) % Chunk::Size;
        const uint32_t py = (Chunk::Size + v.y % Chunk::Size) % Chunk::Size;
        const uint32_t pz = (Chunk::Size + v.z % Chunk::Size) % Chunk::Size;

        return {{ px, py, pz }, { c }};
    }

    static std::tuple<LocalPosE, TranslationV> ComputeOverlap(const glm::vec3& v)
    {
        glm::ivec3 c(0, 0, 0);
        c.x = glm::floor(v.x);
        c.y = glm::floor(v.y);
        c.z = glm::floor(v.z);

        const glm::vec3 norm = v - glm::vec3(c) + glm::vec3(0, 0, 0);
        glm::vec3 _, rest = glm::modf(norm, _);

        return { rest, c };
    }

    template<typename C, typename R>
    R operator+(const Position<LocalSpace, C>& lhs, const Translation<C>& rhs)
    {
        using ttp = Translation<C>         ::data_type;
        using ptp = Position<LocalSpace, C>::data_type;

        ttp l = ptp(lhs);
        ttp r = ttp(rhs);
        ttp v = l + r;

        return ComputeOverlap(v);
    }

    template<typename C, typename R>
    R operator-(const Position<LocalSpace, C>& lhs, const Translation<C>& rhs)
    {
        using ttp = Translation<C>         ::data_type;
        using ptp = Position<LocalSpace, C>::data_type;

        ttp l = ptp(lhs);
        ttp r = ttp(rhs);
        ttp v = l - r;

        return ComputeOverlap(v);
    }

    template<typename C>
    constexpr Position<WorldSpace, C> operator+(const Position<WorldSpace, C>& lhs, const Translation<C>& rhs)
    {
        auto [ lhsParent, lhsLocal ] = lhs;
        auto [ result   , rest     ] = lhsLocal + rhs;
        return { lhsParent + rest, result };
    }

    template<typename C>
    constexpr Position<WorldSpace, C> operator-(const Position<WorldSpace, C>& lhs, const Translation<C>& rhs)
    {
        auto [ lhsParent, lhsLocal ] = lhs;
        auto [ result   , rest     ] = lhsLocal - rhs;
        return { lhsParent + rest, result };
    }

    template<>
    constexpr WorldPosF operator+<FloatCoord>(const WorldPosF& lhs, const TranslationF& rhs)
    {
        return { glm::vec3(lhs) + glm::vec3(rhs) };
    }

    template<>
    constexpr WorldPosF operator-<FloatCoord>(const WorldPosF& lhs, const TranslationF& rhs)
    {
        return { glm::vec3(lhs) - glm::vec3(rhs) };
    }

    template<typename C, typename U>
    constexpr WorldPos<C> ComputeLerp<WorldPos<C>, U>::operator()(const WorldPos<C>& lhs, const WorldPos<C>& rhs, const U& t) const
    {
        auto diff = rhs - lhs;
        auto step = diff * t;
        return { lhs + step };
    }

    template<typename C>
    constexpr float ComputeLength<Translation<C>>::operator()(const Translation<C>& v) const
    {
        return Length(static_cast<Translation<C>::data_type>(v));
    }

    constexpr float ComputeLength<TranslationV>::operator()(const TranslationV& v) const
    {
        return Length(static_cast<TranslationF>(v));
    }

    template<typename C>
    constexpr float ComputeDistance<WorldPos<C>, WorldPos<C>>::operator()(const WorldPos<C>& lhs, const WorldPos<C>& rhs) const
    {
        return Length(rhs - lhs);
    }

    constexpr WorldPosF ComputeFloor<WorldPosF>::operator()(const WorldPosF& v) const
    {
        return { glm::floor(glm::vec3(v)) };
    }

    constexpr WorldPosE ComputeFloor<WorldPosE>::operator()(const WorldPosE& v) const
    {
        return static_cast<WorldPosE>(static_cast<WorldPosV>(v));
    }

    constexpr TranslationF ComputeFloor<TranslationF>::operator()(const TranslationF& v) const
    {
        return { glm::floor(glm::vec3(v)) };
    }

    constexpr TranslationE ComputeFloor<TranslationE>::operator()(const TranslationE& v) const
    {
        return static_cast<TranslationV>(v);
    }

    constexpr WorldPosF ComputeCeil<WorldPosF>::operator()(const WorldPosF& v) const
    {
        return { glm::ceil(glm::vec3(v)) };
    }

    constexpr WorldPosE ComputeCeil<WorldPosE>::operator()(const WorldPosE& v) const
    {
        return static_cast<WorldPosE>(static_cast<WorldPosV>(v) + TranslationV(1, 1, 1));
    }

    constexpr TranslationF ComputeCeil<TranslationF>::operator()(const TranslationF& v) const
    {
        return { glm::floor(glm::vec3(v)) };
    }

    constexpr TranslationE ComputeCeil<TranslationE>::operator()(const TranslationE& v) const
    {
        return static_cast<TranslationV>(v) + TranslationV(1, 1, 1);
    }

    template<typename C>
    constexpr Translation<C> ComputeAbs<Translation<C>>::operator()(const Translation<C>& v) const
    {
        return Abs(static_cast<Translation<C>::type>(v));
    }

}

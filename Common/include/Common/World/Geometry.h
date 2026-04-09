// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_WORLD_GEOMETRY_H
#define MCC_COMMON_WORLD_GEOMETRY_H

#include "Common/World/Position.h"
#include "Common/World/Translation.h"

namespace Mcc
{

    // Translation = Position - Position
    constexpr TranslationV operator-(const ChunkPosV& lhs, const ChunkPosV& rhs);

    constexpr TranslationV operator-(const LocalPosV& lhs, const LocalPosV& rhs);
    constexpr TranslationV operator-(const WorldPosV& lhs, const WorldPosV& rhs);

    constexpr TranslationE operator-(const LocalPosE& lhs, const LocalPosE& rhs);
    constexpr TranslationE operator-(const WorldPosE& lhs, const WorldPosE& rhs);

    constexpr TranslationF operator-(const WorldPosF& lhs, const WorldPosF& rhs);

    // Position = Position +/- Translation
    constexpr ChunkPosV operator+(const ChunkPosV& lhs, const TranslationV& rhs);
    constexpr ChunkPosV operator-(const ChunkPosV& lhs, const TranslationV& rhs);

    template<typename C>
    using LR = std::tuple<Position<LocalSpace, C>, TranslationV>;

    template<typename C, typename R = std::tuple<Position<LocalSpace, C>, TranslationV>>
    constexpr R operator+(const Position<LocalSpace, C>& lhs, const Translation<C>& rhs);
    template<typename C, typename R = std::tuple<Position<LocalSpace, C>, TranslationV>>
    constexpr R operator-(const Position<LocalSpace, C>& lhs, const Translation<C>& rhs);

    template<typename C>
    constexpr Position<WorldSpace, C> operator+(const Position<WorldSpace, C>& lhs, const Translation<C>& rhs);
    template<typename C>
    constexpr Position<WorldSpace, C> operator-(const Position<WorldSpace, C>& lhs, const Translation<C>& rhs);

    template<>
    constexpr WorldPosF operator+<FloatCoord>(const WorldPosF& lhs, const TranslationF& rhs);
    template<>
    constexpr WorldPosF operator-<FloatCoord>(const WorldPosF& lhs, const TranslationF& rhs);

    // Lerp
    template<typename C, typename U>
    struct ComputeLerp<WorldPos<C>, U>
    {
        WorldPos<C> operator()(const WorldPos<C>& lhs, const WorldPos<C>& rhs, const U& t) const;
    };

}

#include "Common/World/Geometry.inl"

#endif

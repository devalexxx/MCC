// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_WORLD_TRANSLATION_H
#define MCC_COMMON_WORLD_TRANSLATION_H

#include "Common/World/Coordinate.h"
#include "Common/World/Position.h"

#include <glm/glm.hpp>

namespace Mcc
{

    template<typename>
    class Translation {};

    using TranslationV = Translation<VoxelCoord>;
    using TranslationE = Translation<EnttyCoord>;
    using TranslationF = Translation<FloatCoord>;

    template<>
    class Translation<VoxelCoord>
    {
      public:
        using data_type = glm::ivec3;

        constexpr Translation() = default;
        constexpr Translation(data_type translation);
        constexpr Translation(auto x, auto y, auto z);

        constexpr operator TranslationE() const;
        constexpr operator TranslationF() const;
        constexpr operator data_type   () const;

        template<std::size_t>
        friend constexpr auto get(Translation t);

        friend constexpr bool operator==(const Translation& lhs, const Translation& rhs);
        friend constexpr bool operator!=(const Translation& lhs, const Translation& rhs);

      private:
        data_type mTranslation;
    };

    // TODO: add chunk to keep float precision
    template<>
    class Translation<EnttyCoord>
    {
      public:
        using data_type = glm::vec3;

        constexpr Translation() = default;
        constexpr Translation(data_type translation);
        constexpr Translation(auto x, auto y, auto z);

        inline    operator TranslationV() const;
        constexpr operator TranslationF() const;
        explicit constexpr operator data_type() const;

        template<std::size_t>
        friend constexpr auto get(Translation t);

        friend inline bool operator==(const Translation& lhs, const Translation& rhs);
        friend inline bool operator!=(const Translation& lhs, const Translation& rhs);

      private:
        data_type mTranslation;
    };

    template<>
    class Translation<FloatCoord>
    {
      public:
        using data_type = glm::vec3;

        constexpr Translation() = default;
        constexpr Translation(data_type translation);
        constexpr Translation(auto x, auto y, auto z);

        inline    operator TranslationV() const;
        constexpr operator TranslationE() const;
        explicit constexpr operator data_type() const;

        template<std::size_t>
        friend constexpr auto get(Translation t);

        friend inline bool operator==(const Translation& lhs, const Translation& rhs);
        friend inline bool operator!=(const Translation& lhs, const Translation& rhs);

      private:
        data_type mTranslation;
    };

    template<std::size_t I>
    constexpr auto get(TranslationV t)
    {
             if constexpr (I == 0) return t.mTranslation.x;
        else if constexpr (I == 1) return t.mTranslation.y;
        else                       return t.mTranslation.z;
    }

    template<std::size_t I>
    constexpr auto get(TranslationE t)
    {
             if constexpr (I == 0) return t.mTranslation.x;
        else if constexpr (I == 1) return t.mTranslation.y;
        else                       return t.mTranslation.z;
    }

    template<std::size_t I>
    constexpr auto get(TranslationF t)
    {
             if constexpr (I == 0) return t.mTranslation.x;
        else if constexpr (I == 1) return t.mTranslation.y;
        else                       return t.mTranslation.z;
    }

    inline auto format_as(const TranslationV& translation);
    inline auto format_as(const TranslationE& translation);
    inline auto format_as(const TranslationF& translation);

    // Translation = Translation +/- Translation
    constexpr TranslationV operator+(const TranslationV& lhs, const TranslationV& rhs);
    constexpr TranslationV operator-(const TranslationV& lhs, const TranslationV& rhs);

    constexpr TranslationE operator+(const TranslationE& lhs, const TranslationE& rhs);
    constexpr TranslationE operator-(const TranslationE& lhs, const TranslationE& rhs);

    constexpr TranslationF operator+(const TranslationF& lhs, const TranslationF& rhs);
    constexpr TranslationF operator-(const TranslationF& lhs, const TranslationF& rhs);

    // Translation = Translation * value
    inline TranslationV operator*(const TranslationV& lhs, const auto& rhs);
    constexpr TranslationE operator*(const TranslationE& lhs, const auto& rhs);
    constexpr TranslationF operator*(const TranslationF& lhs, const auto& rhs);

    // Translation = Translation / T where Translation::type / T exist
    template<typename C>
    constexpr Translation<C> operator/(const Translation<C>& lhs, const auto& rhs);

    // Utility method
    template<typename C>
    constexpr Translation<C> SelectAxis(const Translation<C>& translation, bool x, bool y, bool z);


}

#include "Common/World/Translation.inl"

#endif

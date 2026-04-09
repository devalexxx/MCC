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
        using type = glm::ivec3;

        constexpr Translation() = default;
        constexpr Translation(glm::ivec3 translation);
        constexpr Translation(int32_t x, int32_t y, int32_t z);

        constexpr operator TranslationE() const;
        constexpr operator TranslationF() const;
        constexpr operator glm::ivec3  () const;

        friend constexpr bool operator==(const Translation& lhs, const Translation& rhs);
        friend constexpr bool operator!=(const Translation& lhs, const Translation& rhs);

      private:
        glm::ivec3 mTranslation;
    };

    template<>
    class Translation<EnttyCoord>
    {
      public:
        using type = glm::vec3;

        constexpr Translation() = default;
        constexpr Translation(glm::vec3 translation);
        constexpr Translation(float x, float y, float z);

        constexpr operator TranslationV() const;
        constexpr operator TranslationF() const;
        constexpr operator glm::vec3   () const;

        friend constexpr bool operator==(const Translation& lhs, const Translation& rhs);
        friend constexpr bool operator!=(const Translation& lhs, const Translation& rhs);

      private:
        glm::vec3 mTranslation;
    };

    template<>
    class Translation<FloatCoord>
    {
      public:
        using type = glm::vec3;

        constexpr Translation() = default;
        constexpr Translation(glm::vec3 translation);
        constexpr Translation(float x, float y, float z);

        constexpr operator TranslationV() const;
        constexpr operator TranslationE() const;
        constexpr operator glm::vec3   () const;

        friend constexpr bool operator==(const Translation& lhs, const Translation& rhs);
        friend constexpr bool operator!=(const Translation& lhs, const Translation& rhs);

      private:
        glm::vec3 mTranslation;
    };

    constexpr auto format_as(const TranslationV& translation);
    constexpr auto format_as(const TranslationE& translation);
    constexpr auto format_as(const TranslationF& translation);

    // Translation = Translation +/- Translation
    constexpr TranslationV operator+(const TranslationV& lhs, const TranslationV& rhs);
    constexpr TranslationV operator-(const TranslationV& lhs, const TranslationV& rhs);

    constexpr TranslationE operator+(const TranslationE& lhs, const TranslationE& rhs);
    constexpr TranslationE operator-(const TranslationE& lhs, const TranslationE& rhs);

    constexpr TranslationF operator+(const TranslationF& lhs, const TranslationF& rhs);
    constexpr TranslationF operator-(const TranslationF& lhs, const TranslationF& rhs);

    // Translation = Translation * value
    constexpr TranslationV operator*(const TranslationV& lhs, const auto& rhs);
    constexpr TranslationE operator*(const TranslationE& lhs, const auto& rhs);
    constexpr TranslationF operator*(const TranslationF& lhs, const auto& rhs);

}

#include "Common/World/Translation.inl"

#endif

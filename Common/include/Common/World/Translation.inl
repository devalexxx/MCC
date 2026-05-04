// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

namespace Mcc
{

    constexpr Translation<VoxelCoord>::Translation(const glm::ivec3 translation) :
        mTranslation(translation)
    {}

    constexpr Translation<VoxelCoord>::Translation(const int32_t x, const int32_t y, const int32_t z) :
        mTranslation(x, y, z)
    {}

    constexpr Translation<VoxelCoord>::operator Translation<EnttyCoord>() const
    {
        return { glm::vec3(mTranslation) / glm::vec3(Chunk::Size, Chunk::Height, Chunk::Size) };
    }

    constexpr Translation<VoxelCoord>::operator Translation<FloatCoord>() const
    {
        return { glm::vec(mTranslation) };
    }

    constexpr Translation<VoxelCoord>::operator glm::ivec3() const
    {
        return mTranslation;
    }

    constexpr bool operator==(const Translation<VoxelCoord>& lhs, const Translation<VoxelCoord>& rhs)
    {
        return lhs.mTranslation == rhs.mTranslation;
    }

    constexpr bool operator!=(const Translation<VoxelCoord>& lhs, const Translation<VoxelCoord>& rhs)
    {
        return !(lhs == rhs);
    }

    constexpr Translation<EnttyCoord>::Translation(const glm::vec3 translation) :
        mTranslation(translation)
    {}

    constexpr Translation<EnttyCoord>::Translation(const float x, const float y, const float z) :
        mTranslation(x, y, z)
    {}

    constexpr Translation<EnttyCoord>::operator Translation<VoxelCoord>() const
    {
        return { glm::floor(glm::vec3(mTranslation) * glm::vec3(Chunk::Size, Chunk::Height, Chunk::Size)) };
    }

    constexpr Translation<EnttyCoord>::operator Translation<FloatCoord>() const
    {
        return { glm::vec3(mTranslation) * glm::vec3(Chunk::Size, Chunk::Height, Chunk::Size) };
    }

    constexpr Translation<EnttyCoord>::operator glm::vec3() const
    {
        return mTranslation;
    }

    constexpr bool operator==(const Translation<EnttyCoord>& lhs, const Translation<EnttyCoord>& rhs)
    {
        return glm::all(glm::epsilonEqual(lhs.mTranslation, rhs.mTranslation, std::numeric_limits<float>::epsilon()));
    }

    constexpr bool operator!=(const Translation<EnttyCoord>& lhs, const Translation<EnttyCoord>& rhs)
    {
        return !(lhs == rhs);
    }

    constexpr Translation<FloatCoord>::Translation(const glm::vec3 translation) :
        mTranslation(translation)
    {}

    constexpr Translation<FloatCoord>::Translation(const float x, const float y, const float z) :
        mTranslation(x, y, z)
    {}

    constexpr Translation<FloatCoord>::operator Translation<VoxelCoord>() const
    {
        return { glm::floor(mTranslation) };
    }

    constexpr Translation<FloatCoord>::operator Translation<EnttyCoord>() const
    {
        return { mTranslation / glm::vec3(Chunk::Size, Chunk::Height, Chunk::Size) };
    }

    constexpr Translation<FloatCoord>::operator glm::vec3() const
    {
        return mTranslation;
    }

    constexpr bool operator==(const Translation<FloatCoord>& lhs, const Translation<FloatCoord>& rhs)
    {
        return glm::all(glm::epsilonEqual(lhs.mTranslation, rhs.mTranslation, std::numeric_limits<float>::epsilon()));
    }

    constexpr bool operator!=(const Translation<FloatCoord>& lhs, const Translation<FloatCoord>& rhs)
    {
        return !(lhs == rhs);
    }

    constexpr auto format_as(const TranslationV& translation)
    {
        return fmt::format("WorldPosF({})", static_cast<glm::ivec3>(translation) );
    }

    constexpr auto format_as(const TranslationE& translation)
    {
        return fmt::format("TranslationE({})", static_cast<glm::vec3>(translation) );
    }

    constexpr auto format_as(const TranslationF& translation)
    {
        return fmt::format("TranslationF({})", static_cast<glm::vec3>(translation) );
    }

    constexpr TranslationV operator+(const TranslationV& lhs, const TranslationV& rhs)
    {
        return { glm::ivec3(lhs) + glm::ivec3(rhs) };
    }

    constexpr TranslationV operator-(const TranslationV& lhs, const TranslationV& rhs)
    {
        return { glm::ivec3(lhs) - glm::ivec3(rhs) };
    }

    constexpr TranslationE operator+(const TranslationE& lhs, const TranslationE& rhs)
    {
        return { glm::vec3(lhs) + glm::vec3(rhs) };
    }

    constexpr TranslationE operator-(const TranslationE& lhs, const TranslationE& rhs)
    {
        return { glm::vec3(lhs) - glm::vec3(rhs) };
    }

    constexpr TranslationF operator+(const TranslationF& lhs, const TranslationF& rhs)
    {
        return { glm::vec3(lhs) + glm::vec3(rhs) };
    }

    constexpr TranslationF operator-(const TranslationF& lhs, const TranslationF& rhs)
    {
        return { glm::vec3(lhs) - glm::vec3(rhs) };
    }

    constexpr TranslationV operator*(const TranslationV& lhs, const auto& rhs)
    {
        return { glm::ivec3(glm::floor(glm::vec3(glm::ivec3(lhs)) * static_cast<float>(rhs))) };
    }

    constexpr TranslationE operator*(const TranslationE& lhs, const auto& rhs)
    {
        return { glm::vec3(lhs) * static_cast<float>(rhs) };
    }

    constexpr TranslationF operator*(const TranslationF& lhs, const auto& rhs)
    {
        return { glm::vec3(lhs) * static_cast<float>(rhs) };
    }

    template<typename C>
    constexpr Translation<C> operator/(const Translation<C>& lhs, const auto& rhs)
    {
        return { static_cast<typename Translation<C>::type>(lhs) / rhs };
    }

    template<typename C>
    constexpr Translation<C> SelectAxis(const Translation<C>& translation, bool x, bool y, bool z)
    {
        typename Translation<C>::type t = translation;
        return {
            x ? t.x : 0,
            y ? t.y : 0,
            z ? t.z : 0
        };
    }

}

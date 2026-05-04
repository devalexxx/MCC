// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

namespace Mcc
{

    constexpr Translation<VoxelCoord>::Translation(const glm::ivec3 translation) :
        mTranslation(translation)
    {}

    constexpr Translation<VoxelCoord>::Translation(auto x, auto y, auto z) :
        mTranslation(x, y, z)
    {}

    constexpr Translation<VoxelCoord>::operator Translation<EnttyCoord>() const
    {
        using T = Translation<EnttyCoord>::data_type;
        return { T(mTranslation) / T(Chunk::Size) };
    }

    constexpr Translation<VoxelCoord>::operator Translation<FloatCoord>() const
    {
        return { mTranslation };
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

    constexpr Translation<EnttyCoord>::Translation(auto x, auto y, auto z) :
        mTranslation(x, y, z)
    {}

    inline Translation<EnttyCoord>::operator Translation<VoxelCoord>() const
    {
        return { glm::floor(mTranslation * data_type(Chunk::Size)) };
    }

    constexpr Translation<EnttyCoord>::operator Translation<FloatCoord>() const
    {
        using T = Translation<FloatCoord>::data_type;
        return { T(mTranslation) * T(Chunk::Size) };
    }

    constexpr Translation<EnttyCoord>::operator data_type() const
    {
        return mTranslation;
    }

    inline bool operator==(const Translation<EnttyCoord>& lhs, const Translation<EnttyCoord>& rhs)
    {
        return glm::all(glm::epsilonEqual(lhs.mTranslation, rhs.mTranslation, std::numeric_limits<float>::epsilon()));
    }

    inline bool operator!=(const Translation<EnttyCoord>& lhs, const Translation<EnttyCoord>& rhs)
    {
        return !(lhs == rhs);
    }

    constexpr Translation<FloatCoord>::Translation(const glm::vec3 translation) :
        mTranslation(translation)
    {}

    constexpr Translation<FloatCoord>::Translation(auto x, auto y, auto z) :
        mTranslation(x, y, z)
    {}

    inline Translation<FloatCoord>::operator Translation<VoxelCoord>() const
    {
        return { glm::floor(mTranslation) };
    }

    constexpr Translation<FloatCoord>::operator Translation<EnttyCoord>() const
    {
        using T = Translation<EnttyCoord>::data_type;
        return { T(mTranslation) / T(Chunk::Size) };
    }

    constexpr Translation<FloatCoord>::operator data_type() const
    {
        return mTranslation;
    }

    inline bool operator==(const Translation<FloatCoord>& lhs, const Translation<FloatCoord>& rhs)
    {
        return glm::all(glm::epsilonEqual(lhs.mTranslation, rhs.mTranslation, std::numeric_limits<float>::epsilon()));
    }

    inline bool operator!=(const Translation<FloatCoord>& lhs, const Translation<FloatCoord>& rhs)
    {
        return !(lhs == rhs);
    }

    inline auto format_as(const TranslationV& translation)
    {
        return fmt::format("TranslationV({})", static_cast<TranslationV::data_type>(translation) );
    }

    inline auto format_as(const TranslationE& translation)
    {
        return fmt::format("TranslationE({})", static_cast<TranslationE::data_type>(translation) );
    }

    inline auto format_as(const TranslationF& translation)
    {
        return fmt::format("TranslationF({})", static_cast<TranslationF::data_type>(translation) );
    }

    constexpr TranslationV operator+(const TranslationV& lhs, const TranslationV& rhs)
    {
        return { TranslationV::data_type(lhs) + TranslationV::data_type(rhs) };
    }

    constexpr TranslationV operator-(const TranslationV& lhs, const TranslationV& rhs)
    {
        return { TranslationV::data_type(lhs) - TranslationV::data_type(rhs) };
    }

    constexpr TranslationE operator+(const TranslationE& lhs, const TranslationE& rhs)
    {
        return { TranslationE::data_type(lhs) + TranslationE::data_type(rhs) };
    }

    constexpr TranslationE operator-(const TranslationE& lhs, const TranslationE& rhs)
    {
        return { TranslationE::data_type(lhs) - TranslationE::data_type(rhs) };
    }

    constexpr TranslationF operator+(const TranslationF& lhs, const TranslationF& rhs)
    {
        return { TranslationF::data_type(lhs) + TranslationF::data_type(rhs) };
    }

    constexpr TranslationF operator-(const TranslationF& lhs, const TranslationF& rhs)
    {
        return { TranslationF::data_type(lhs) - TranslationF::data_type(rhs) };
    }

    TranslationV operator*(const TranslationV& lhs, const auto& rhs)
    {
        return { TranslationV::data_type(glm::floor(glm::vec3(TranslationV::data_type(lhs)) * static_cast<float>(rhs))) };
    }

    constexpr TranslationE operator*(const TranslationE& lhs, const auto& rhs)
    {
        return { TranslationE::data_type(lhs) * static_cast<float>(rhs) };
    }

    constexpr TranslationF operator*(const TranslationF& lhs, const auto& rhs)
    {
        return { TranslationF::data_type(lhs) * static_cast<float>(rhs) };
    }

    template<typename C>
    constexpr Translation<C> operator/(const Translation<C>& lhs, const auto& rhs)
    {
        return { static_cast<Translation<C>::data_type>(lhs) / rhs };
    }

    template<typename C>
    constexpr Translation<C> SelectAxis(const Translation<C>& translation, bool x, bool y, bool z)
    {
        auto t = static_cast<Translation<C>::data_type>(translation);
        return {
            x ? t.x : 0,
            y ? t.y : 0,
            z ? t.z : 0
        };
    }

}

// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include <glm/ext/quaternion_common.hpp>
namespace Mcc
{

    template<typename T, size_t S, glm::qualifier Q, typename U>
    constexpr glm::vec<S, T, Q>
    ComputeLerp<glm::vec<S, T, Q>, U>::operator()(const glm::vec<S, T, Q>& lhs, const glm::vec<S, T, Q>& rhs, const U& t) const
    {
        return glm::lerp(lhs, rhs, t);
    }

    template<typename T, typename U>
    constexpr T Lerp(const T& lhs, const T& rhs, const U& t)
    {
        return ComputeLerp<T, U>{}(lhs, rhs, t);
    }

    template<typename T, glm::qualifier Q, typename U>
    constexpr glm::qua<T, Q>
    ComputeSLerp<glm::qua<T, Q>, U>::operator()(const glm::qua<T, Q>& lhs, const glm::qua<T, Q>& rhs, const U& t) const
    {
        return glm::slerp(lhs, rhs, t);
    }

    template<typename T, typename U>
    constexpr T SLerp(const T& lhs, const T& rhs, const U& t)
    {
        return ComputeSLerp<T, U>{}(lhs, rhs, t);
    }

    template<typename T, size_t S, glm::qualifier Q>
    constexpr float ComputeLength<glm::vec<S, T, Q>>::operator()(const glm::vec<S, T, Q>& v) const
    {
        return glm::length(v);
    }

    template<typename T>
    constexpr auto Length(const T& v)
    {
        return ComputeLength<T>{}(v);
    }

    template<typename T1, typename T2, size_t S, glm::qualifier Q>
    constexpr float
    ComputeDistance<glm::vec<S, T1, Q>, glm::vec<S, T2, Q>>::operator()(const glm::vec<S, T1, Q>& lhs, const glm::vec<S, T2, Q>& rhs) const
    {
        return glm::distance(lhs, rhs);
    }

    template<typename T1, typename T2>
    constexpr auto Distance(const T1& lhs, const T2& rhs)
    {
        return ComputeDistance<T1, T2>{}(lhs, rhs);
    }

    template<typename T, size_t S, glm::qualifier Q>
    constexpr glm::vec<S, T, Q> ComputeFloor<glm::vec<S, T, Q>>::operator()(const glm::vec<S, T, Q>& v) const
    {
        return glm::floor(v);
    }

    template<typename T>
    constexpr auto Floor(const T& v)
    {
        return ComputeFloor<T>{}(v);
    }

    template<typename T, size_t S, glm::qualifier Q>
    constexpr glm::vec<S, T, Q> ComputeCeil<glm::vec<S, T, Q>>::operator()(const glm::vec<S, T, Q>& v) const
    {
        return glm::ceil(v);
    }

    template<typename T>
    constexpr auto Ceil(const T& v)
    {
        return ComputeCeil<T>{}(v);
    }

    template<typename T, size_t S, glm::qualifier Q>
    constexpr glm::vec<S, T, Q> ComputeAbs<glm::vec<S, T, Q>>::operator()(const glm::vec<S, T, Q>& v) const
    {
        return glm::abs(v);
    }

    template<typename T>
    constexpr auto Abs(const T& v)
    {
        return ComputeAbs<T>{}(v);
    }

}

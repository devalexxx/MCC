// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_UTILS_MATH_UTILS_H
#define MCC_COMMON_UTILS_MATH_UTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Mcc
{

    template<typename, typename>
    struct ComputeLerp {};

    template<typename T, size_t S, glm::qualifier Q, typename U>
    struct ComputeLerp<glm::vec<S, T, Q>, U>
    {
        constexpr glm::vec<S, T, Q> operator()(const glm::vec<S, T, Q>& lhs, const glm::vec<S, T, Q>& rhs, const U& t) const;
    };

    template<typename T, typename U>
    constexpr T Lerp(const T& lhs, const T& rhs, const U& t);

    template<typename, typename>
    struct ComputeSLerp {};

    template<typename T, glm::qualifier Q, typename U>
    struct ComputeSLerp<glm::qua<T, Q>, U>
    {
        constexpr glm::qua<T, Q> operator()(const glm::qua<T, Q>& lhs, const glm::qua<T, Q>& rhs, const U& t) const;
    };

    template<typename T, typename U>
    constexpr T SLerp(const T& lhs, const T& rhs, const U& t);

    template<typename>
    struct ComputeLength {};

    template<typename T, size_t S, glm::qualifier Q>
    struct ComputeLength<glm::vec<S, T, Q>>
    {
        constexpr float operator()(const glm::vec<S, T, Q>& v) const;
    };

    template<typename T>
    constexpr auto Length(const T& v);

    template<typename, typename>
    struct ComputeDistance {};

    template<typename T1, typename T2, size_t S, glm::qualifier Q>
    struct ComputeDistance<glm::vec<S, T1, Q>, glm::vec<S, T2, Q>>
    {
        constexpr float operator()(const glm::vec<S, T1, Q>& lhs, const glm::vec<S, T2, Q>& rhs) const;
    };

    template<typename T1, typename T2>
    constexpr auto Distance(const T1& lhs, const T2& rhs);

    template<typename>
    struct ComputeFloor {};

    template<typename T, size_t S, glm::qualifier Q>
    struct ComputeFloor<glm::vec<S, T, Q>>
    {
        constexpr glm::vec<S, T, Q> operator()(const glm::vec<S, T, Q>& v) const;
    };

    template<typename T>
    constexpr auto Floor(const T& v);

    template<typename>
    struct ComputeCeil {};

    template<typename T, size_t S, glm::qualifier Q>
    struct ComputeCeil<glm::vec<S, T, Q>>
    {
        constexpr glm::vec<S, T, Q> operator()(const glm::vec<S, T, Q>& v) const;
    };

    template<typename T>
    constexpr auto Ceil(const T& v);

    template<typename T>
    struct ComputeAbs {};

    template<typename T, size_t S, glm::qualifier Q>
    struct ComputeAbs<glm::vec<S, T, Q>>
    {
        constexpr glm::vec<S, T, Q> operator()(const glm::vec<S, T, Q>& v) const;
    };

    template<typename T>
    constexpr auto Abs(const T& v);

}

#include "Common/Utils/MathUtils.inl"

#endif

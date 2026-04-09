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
        glm::vec<S, T, Q> operator()(const glm::vec<S, T, Q>& lhs, const glm::vec<S, T, Q>& rhs, const U& t) const
        {
            return glm::lerp(lhs, rhs, t);
        }
    };

    template<typename, typename>
    struct ComputeSLerp {};

    template<typename T, glm::qualifier Q, typename U>
    struct ComputeSLerp<glm::qua<T, Q>, U>
    {
        glm::qua<T, Q> operator()(const glm::qua<T, Q>& lhs, const glm::qua<T, Q>& rhs, const U& t) const
        {
            return glm::lerp(lhs, rhs, t);
        }
    };

    template<typename T, typename U>
    constexpr T Lerp(const T& lhs, const T& rhs, const U& t);

    template<typename T, typename U>
    constexpr T SLerp(const T& lhs, const T& rhs, const U& t);

}

#include "Common/Utils/MathUtils.inl"

#endif

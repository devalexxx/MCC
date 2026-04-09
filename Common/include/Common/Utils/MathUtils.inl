// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include <glm/ext/quaternion_common.hpp>
namespace Mcc
{

    template<typename T, typename U>
    constexpr T Lerp(const T& lhs, const T& rhs, const U& t)
    {
        return ComputeLerp<T, U>{}(lhs, rhs, t);
    }

    template<typename T, typename U>
    constexpr T SLerp(const T& lhs, const T& rhs, const U& t)
    {
        return ComputeSLerp<T, U>{}(lhs, rhs, t);
    }

}

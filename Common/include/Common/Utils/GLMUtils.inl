// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include <fmt/format.h>

namespace glm
{

    constexpr auto format_as(const ivec2& v)
    {
        return fmt::format("glm::ivec2({}, {})", v.x, v.y);
    }

    constexpr auto format_as(const fvec3& v)
    {
        return fmt::format("glm::fvec3({}, {}, {})", v.x, v.y, v.z);
    }

    constexpr auto format_as(const uvec3& v)
    {
        return fmt::format("glm::uvec3({}, {}, {})", v.x, v.y, v.z);
    }

    constexpr auto format_as(const ivec3& v)
    {
        return fmt::format("glm::ivec3({}, {}, {})", v.x, v.y, v.z);
    }

    template<class Archive>
    constexpr void serialize(Archive& ar, vec3& vec)
    {
        ar(vec.x, vec.y, vec.z);
    }

    template<class Archive>
    constexpr void serialize(Archive& ar, uvec3& vec)
    {
        ar(vec.x, vec.y, vec.z);
    }

    template<class Archive>
    constexpr void serialize(Archive& ar, ivec3& vec)
    {
        ar(vec.x, vec.y, vec.z);
    }

    template<class Archive>
    constexpr void serialize(Archive& ar, ivec2& vec)
    {
        ar(vec.x, vec.y);
    }

    template<class Archive>
    constexpr void serialize(Archive& ar, quat& quat)
    {
        ar(quat.x, quat.y, quat.z, quat.w);
    }

}

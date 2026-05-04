// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_UTILS_GLM_UTILS_H
#define MCC_COMMON_UTILS_GLM_UTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace glm
{

    inline auto format_as(const ivec2& v);
    inline auto format_as(const fvec3& v);
    inline auto format_as(const uvec3& v);
    inline auto format_as(const ivec3& v);

    template<class Archive>
    constexpr void serialize(Archive& ar, vec3& vec);

    template<class Archive>
    constexpr void serialize(Archive& ar, uvec3& vec);

    template<class Archive>
    constexpr void serialize(Archive& ar, ivec3& vec);

    template<class Archive>
    constexpr void serialize(Archive& ar, ivec2& vec);

    template<class Archive>
    constexpr void serialize(Archive& ar, quat& quat);

}

namespace std
{

    template<>
    struct hash<glm::vec3>
    {
        inline size_t operator()(const glm::vec3& v) const noexcept
        {
            return std::hash<float>()(v.x) ^ std::hash<float>()(v.y) << 1 ^ std::hash<float>()(v.z) << 2;
        }
    };

    template<>
    struct hash<glm::ivec2>
    {
        inline size_t operator()(const glm::ivec2& v) const noexcept
        {
            return std::hash<int>()(v.x) ^ std::hash<int>()(v.y) << 1;
        }
    };

    template<>
    struct hash<glm::ivec3>
    {
        inline size_t operator()(const glm::ivec3& v) const noexcept
        {
            return std::hash<int>()(v.x) ^ std::hash<int>()(v.y) << 1 ^ std::hash<int>()(v.z) << 2;
        }
    };

    template<>
    struct hash<glm::uvec3>
    {
        inline size_t operator()(const glm::uvec3& v) const noexcept
        {
            return std::hash<uint32_t>()(v.x) ^ std::hash<uint32_t>()(v.y) << 1 ^ std::hash<uint32_t>()(v.z) << 2;
        }
    };

}

#include "Common/Utils/GLMUtils.inl"

#endif

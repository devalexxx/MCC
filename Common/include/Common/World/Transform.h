// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_WORLD_TRANSFORM_H
#define MCC_COMMON_WORLD_TRANSFORM_H

#include "Common/World/Position.h"

namespace Mcc
{

    template<typename C>
    struct Transform
    {
        Position<WorldSpace, C> position;
        glm::quat               rotation;
        glm::vec3               scale;

        template<typename C2>
        constexpr operator Transform<C2>() const;
    };

    template<typename Archive, typename C>
    constexpr void serialize(Archive& ar, Transform<C>& transform);

}

#include "Common/World/Transform.inl"

#endif

// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_UTILS_RAYCAST_H
#define MCC_COMMON_UTILS_RAYCAST_H

#include "Common/World/Position.h"

#include <glm/glm.hpp>

#include <flecs.h>

#include <optional>
#include <variant>

namespace Mcc
{

    struct BlockHit
    {
        WorldPosV position;
        WorldPosV previous;
    };

    struct EntityHit
    {
        flecs::entity entity;
    };

    using HitObject = std::variant<BlockHit, EntityHit>;

    struct HitResult
    {
        HitObject object;
        float     distance;
    };

    std::optional<HitResult> Raycast(
        const flecs::world& world, const WorldPosE& from, glm::vec3 direction, float maxDistance
    );

}

#endif

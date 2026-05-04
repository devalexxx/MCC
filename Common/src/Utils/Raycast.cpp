// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Utils/Raycast.h"

#include "Common/Utils/BlockUtils.h"
#include "Common/World/Translation.h"
#include "Common/World/Geometry.h"
#include "Common/WorldContext.h"

namespace Mcc
{

    std::optional<HitResult> Raycast(
        const flecs::world& world, const WorldPosE& from, glm::vec3 direction, float maxDistance
    )
    {
        auto ctx = WorldContext<>::Get(world);

        auto chunkEntityIt = ctx->chunkMapping.find(get<0>(from));
        if (chunkEntityIt == ctx->chunkMapping.end())
        {
            return std::nullopt;
        }

        auto cChunkEntity = world.entity(chunkEntityIt->second);
        auto cChunkPtr    = world.entity(cChunkEntity).get<CChunkPtr>();

        // ensure non zero component
        for (size_t i = 0; i < 3; ++i)
        {
            direction[i] = direction[i] != 0 ? direction[i] : glm::epsilon<float>();
        }

        WorldPosV    previous = from;
        WorldPosV    position = from;
        TranslationV step  (
            direction.x >= 0 ? 1 : -1,
            direction.y >= 0 ? 1 : -1,
            direction.z >= 0 ? 1 : -1
        );

        TranslationV step2  (
            direction.x >= 0 ? 1 : 0,
            direction.y >= 0 ? 1 : 0,
            direction.z >= 0 ? 1 : 0
        );


        WorldPosV    nextVoxel    = position + step2;
        TranslationF dToNextVoxel = WorldPosE(nextVoxel) - from;
        TranslationF tMax         = dToNextVoxel / direction;
        TranslationF tDelta       = glm::abs(1.f / direction);

        float distance = 0.f;
        while (distance < maxDistance)
        {
            bool isX = get<0>(tMax) < get<1>(tMax) && get<0>(tMax) < get<2>(tMax);
            bool isY = !isX && get<1>(tMax) < get<2>(tMax);
            bool isZ = !isX && !isY;

            if (isY && get<1, 1>(position) + 1 >= Chunk::Height)
                return std::nullopt;

            position = position + SelectAxis(step  , isX, isY, isZ);
            tMax     = tMax     + SelectAxis(tDelta, isX, isY, isZ);

            distance = Distance(static_cast<WorldPosF>(from), static_cast<WorldPosF>(position));
            if (distance > maxDistance)
                break;

            if (get<0>(previous) != get<0>(position))
            {
                auto newChunkEntityMapIt = ctx->chunkMapping.find(get<0>(position));
                if (newChunkEntityMapIt == ctx->chunkMapping.end())
                {
                    continue;
                }

                cChunkEntity = world.entity(newChunkEntityMapIt->second);
                cChunkPtr    = cChunkEntity.get<CChunkPtr>();
            }

            auto newBlockEntityT = cChunkPtr->Get(glm::uvec3(get<1>(position)));
            if (!world.is_valid(newBlockEntityT))
            {
                continue;
            }

            if (auto newBlockEntity = world.entity(newBlockEntityT); IsSolid(newBlockEntity))
            {
                return {{
                    .object   = { BlockHit { .position = position, .previous = previous }},
                    .distance = distance
                }};
            }

            previous = position;
        }

        return std::nullopt;
    }

}

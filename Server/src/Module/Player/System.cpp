// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/Player/System.h"

#include "Server/Module/EntityReplication/Component.h"
#include "Server/Module/Player/Component.h"
#include "Server/Module/UserSession/Module.h"
#include "Server/WorldContext.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Utils/ChunkHelper.h"

#include <ranges>

namespace Mcc
{

    void ProcessPlayerInputs(const flecs::entity entity, CUserInputQueue& queue)
    {
        const auto* ctx = ServerWorldContext::Get(entity.world());

        if (queue.empty())
        {
            return;
        }

        entity.get([&](CTransform& transform, CEntityDataMap& data) {
            const auto position = transform.position;
            for (; !queue.empty(); queue.pop_front())
            {
                auto& input = queue.front();
                Helper::ApplyXAxis   (input, transform);
                Helper::ApplyMovement(input, transform, ctx->settings.userSpeed, input.meta.dt);

                if (queue.size() == 1)
                {
                    data.insert_or_assign("last-input-processed", std::to_string(input.meta.id));
                }
            }

            entity.world()
                .event<OnPlayerMoveEvent>()
                .ctx({ .prev = position, .curr = transform.position })
                .entity(entity)
                .emit();
        });

        entity.add<TEntityDirty>();
    }

    void HandlePlayerCreation(const flecs::entity entity)
    {
        entity.world()
            .event<OnPlayerCreatedEvent>()
            .id<CTransform>()
            .entity(entity)
            .emit();
    }

}
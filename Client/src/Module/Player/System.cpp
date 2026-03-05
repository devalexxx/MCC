// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/Player/System.h"

#include "Client/ClientNetworkManager.h"
#include "Client/Module/Camera/Component.h"
#include "Client/Module/EntityReplication/Component.h"
#include "Client/Module/Player/Component.h"
#include "Client/WorldContext.h"

namespace Mcc
{

    void
    ApplyAndSendPlayerInput(const flecs::iter& it, const size_t row, UserInput& current, CUserInputQueue& queue)
    {
        const auto* ctx = ClientWorldContext::Get(it.world());

        if (!Helper::IsNull(current))
        {
            constexpr float speed = 5.f;
            const float     delta = it.delta_time();

            current.meta.id = UserInput::Meta::GetNextID();
            current.meta.dt = delta;
            ctx->networkManager.Send<OnPlayerInput>({ current }, ENET_PACKET_FLAG_RELIABLE, 0);

            const auto entity = it.entity(row);
            entity.get([&](CTransform& transform) {
                Helper::ApplyXAxis(current, transform);
                Helper::ApplyMovement(current, transform, delta, speed);
            });

            it.world().query_builder<CTransform>().with<RCameraFollow>(entity).build().each(
                [&](CTransform& transform) { Helper::ApplyBothAxis(current, transform); }
            );

            queue.push_back(current);
            current.axis = {};
        }
    }

}
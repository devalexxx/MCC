// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/Player/Module.h"

#include "Server/Module/EntityReplication/Component.h"
#include "Server/Module/Player/Component.h"
#include "Server/Module/Player/System.h"
#include "Server/Module/UserSession/Module.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Entity/Module.h"
#include "Common/Phase.h"
#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"
#include "Common/WorldContext.h"

namespace Mcc
{

    PlayerModule::PlayerModule(flecs::world& world) : BaseModule(world)
    {}

    void PlayerModule::RegisterComponent(flecs::world& world)
    {
        world.component<OnPlayerCreatedEvent>();
        world.component<OnPlayerMoveEvent>();
    }

    void PlayerModule::RegisterSystem(flecs::world& world)
    {
        world.system<UserInputQueue>("ProcessPlayerInputsSystem")
            .kind<Phase::OnUpdate>()
            .with<UserEntityTag>()
            .each(ProcessPlayerInputs);

        world.system("HandlePlayerCreationSystem")
            .kind<Phase::OnSetup>()
            .with<EntityCreatedTag>()
            .with<UserEntityTag>()
            .each(HandlePlayerCreation);
    }

    void PlayerModule::RegisterHandler(flecs::world& world)
    {
        const auto* ctx = WorldContext<>::Get(world);
        ctx->networkManager.Subscribe<From<OnPlayerInput>>([&](const auto& event) {
            OnPlayerInputHandler(world, event);
        });
    }


    void PlayerModule::OnPlayerInputHandler(const flecs::world& world, const From<OnPlayerInput>& from)
    {
        const auto* ctx     = WorldContext<>::Get(world);
        const auto* session = UserSession::Get(from.peer);
        const auto  rHandle = session->pInfo.handle;
        const auto  lHandle = ctx->networkMapping.GetLHandle(session->pInfo.handle);

        if (!lHandle.has_value())
        {
            MCC_LOG_WARN("The network id {} isn't associated to a local entity", rHandle);
            return;
        }

        if (!world.is_alive(*lHandle))
        {
            MCC_LOG_WARN("The local entity associated to the network id {} isn't alive", rHandle);
            return;
        }

        world.entity(*lHandle).get([&from](UserInputQueue& queue) {
            queue.data.push_back(from.packet.input);
        });
    }

}
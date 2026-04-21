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
    {
        const auto* ctx = WorldContext<>::Get(world);
        ctx->networkManager.Subscribe<From<OnPlayerInput>>(OnPlayerInputHandler, world);
    }

    void PlayerModule::RegisterComponent(flecs::world& world)
    {
        world.component<OnPlayerCreatedEvent>();
        world.component<OnPlayerMoveEvent>();
    }

    void PlayerModule::RegisterPrefab(flecs::world& /* world */) {}

    void PlayerModule::RegisterSystem(flecs::world& world)
    {
        world.system<CUserInputQueue>("ProcessPlayerInputs")
            .kind<Phase::OnUpdate>()
            .with<TUserEntity>()
            .each(ProcessPlayerInputs);

        world.system("HandlePlayerCreation")
            .kind<Phase::PreUpdate>()
            .with<TEntityCreated>()
            .with<TUserEntity>()
            .each(HandlePlayerCreation);
    }

    void PlayerModule::RegisterObserver(flecs::world& /* world */) {}

    void PlayerModule::OnPlayerInputHandler(const From<OnPlayerInput>& from, const flecs::world& world)
    {
        const auto* ctx     = WorldContext<>::Get(world);
        const auto* session = UserSession::Get(from.peer);
        const auto  rHandle = session->pInfo.handle;
        const auto  lHandle = ctx->networkMapping.GetLHandle(session->pInfo.handle);

        if (!lHandle.has_value())
        {
            MCC_LOG_WARN("[OnPlayerInputHandler] Entity({}) isn't associated to a local entity", rHandle);
            return;
        }

        if (!world.is_alive(*lHandle))
        {
            MCC_LOG_WARN("[OnPlayerInputHandler] Entity({}) isn't alive", rHandle);
            return;
        }

        world.entity(*lHandle).get([&from](CUserInputQueue& queue) {
            queue.push_back(from.packet.input);
        });
    }

}
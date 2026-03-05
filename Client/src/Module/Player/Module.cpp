// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/Player/Module.h"

#include "Client/Module/Camera/Component.h"
#include "Client/Module/Camera/Module.h"
#include "Client/Module/EntityReplication/Component.h"
#include "Client/Module/EntityReplication/Module.h"
#include "Client/Module/Player/Component.h"
#include "Client/Module/Player/System.h"
#include "Client/Scene/Scene.h"
#include "Client/WorldContext.h"

#include "Common/Phase.h"

#include <algorithm>

namespace Mcc
{

    PlayerModule::PlayerModule(flecs::world& world) :
        BaseModule(world),
        mKeyEventHandlerID(0),
        mCursorPosEventHandlerID(0)
    {
        const auto* ctx = ClientWorldContext::Get(world);
        ctx->networkManager.Subscribe<OnEntitiesCreated>(OnEntitiesCreatedHandler, world);
        ctx->networkManager.Subscribe<OnEntitiesUpdated>(OnEntitiesUpdatedHandler, world);
    }

    void PlayerModule::RegisterComponent(flecs::world& world)
    {
        world.component<TPlayerEntity>();

        world.component<PPlayerEntity>();

        AutoRegister<CCurrentPlayerInput>::Register(world, "CCurrentPlayerInput");
    }

    void PlayerModule::RegisterPrefab(flecs::world& world)
    {
        world.prefab<PPlayerEntity>()
            .is_a<PUserEntity>()
            .add<TPlayerEntity>()
            .add<TInterpolationExcluded>()
            .set<CCurrentPlayerInput>({});
    }

    void PlayerModule::RegisterSystem(flecs::world& world)
    {
        world.system<CCurrentPlayerInput, CUserInputQueue>("ApplyAndSendPlayerInput")
            .kind<Phase::OnUpdate>()
            .with<TPlayerEntity>()
            .each(ApplyAndSendPlayerInput)
            .add<GameScene>();
    }

    void PlayerModule::RegisterObserver(flecs::world& /* world */) {}

    void PlayerModule::SetInputHandler(flecs::world& world)
    {
        if (mKeyEventHandlerID != 0 || mCursorPosEventHandlerID != 0)
            return;

        const auto ctx = ClientWorldContext::Get(world);
        mKeyEventHandlerID       = ctx->window.Subscribe<KeyEvent>      (OnKeyEventHandler, world);
        mCursorPosEventHandlerID = ctx->window.Subscribe<CursorPosEvent>(OnCursorPosEventHandler, world);
    }

    void PlayerModule::ClearInputHandler(const flecs::world& world)
    {
        if (mKeyEventHandlerID == 0 || mCursorPosEventHandlerID == 0)
            return;

        const auto ctx = ClientWorldContext::Get(world);
        ctx->window.Withdraw(mKeyEventHandlerID);
        ctx->window.Withdraw(mCursorPosEventHandlerID);

        mKeyEventHandlerID       = 0;
        mCursorPosEventHandlerID = 0;
    }

    void PlayerModule::OnEntitiesCreatedHandler(const OnEntitiesCreated& event, const flecs::world& world)
    {
        const auto* ctx = ClientWorldContext::Get(world);

        if (const auto state = std::ranges::find_if(
                event.states, [&](const EntityState& s) { return s.handle == ctx->playerInfo.handle; }
            );
            state != event.states.cend())
        {
            const auto handle = ctx->networkMapping.GetLHandle(ctx->playerInfo.handle);
            if (!handle.has_value())
            {
                MCC_LOG_ERROR(
                    "[OnEntitiesCreatedHandler] No local entity associated to the player network id, make sure to import EntityReplicationModule before PlayerModule"
                );
                return;
            }

            const auto entity = world.entity(*handle).is_a<PPlayerEntity>();
            world.entity()
                .is_a<PCameraFollow>()
                .set<CCameraSettings>({
                    .fov=glm::radians(ctx->settings.fov),
                    .zNear=0.1f,
                    .zFar=static_cast<float>(ctx->settings.renderDistance) * 256.f
                })
                .set<CCameraFollowSettings>({ { 0, 2, 0 } })
                .add<RCameraFollow>(entity)
                .add<TActiveCamera>()
                .child_of<SceneRoot>();
        }
    }

    void PlayerModule::OnEntitiesUpdatedHandler(const OnEntitiesUpdated& event, const flecs::world& world)
    {
        const auto* ctx = ClientWorldContext::Get(world);

        if (const auto state = std::ranges::find_if(
                event.states, [&](const EntityState& s) { return s.handle == ctx->playerInfo.handle; }
            );
            state != event.states.cend())
        {
            if (const auto lipIt = state->extra.find("last-input-processed"); lipIt != state->extra.cend())
            {
                const auto id     = std::stoul(lipIt->second);
                const auto handle = ctx->networkMapping.GetLHandle(state->handle);
                if (!handle.has_value())
                {
                    MCC_LOG_WARN("[OnEntitiesUpdatedHandler] Entity({}) isn't associated to a local entity", state->handle);
                    return;
                }

                const auto entity = world.entity(*handle);
                auto&      iQueue = entity.get_mut<CUserInputQueue>();

                // Drop inputs already processed by the server
                for (; !iQueue.empty(); iQueue.pop_front())
                {
                    if (const auto& input = iQueue.front(); input.meta.id == id)
                    {
                        iQueue.pop_front();
                        break;
                    }
                }

                MCC_ASSERT(
                    iQueue.empty() || (!iQueue.empty() && iQueue.front().meta.id - id == 1),
                    "The difference between front input in queue and last input processes by the server should be equal to 1"
                );

                // Empty snapshot queue
                auto& sQueue = entity.get_mut<CSnapshotQueue>();
                while (!sQueue.empty()) { sQueue.pop_back(); }

                // Set the last received transform and reapply all inputs unprocessed by the server
                auto tr = state->transform;
                for (auto& input: iQueue)
                {
                    Helper::ApplyXAxis(input, tr);
                    Helper::ApplyMovement(input, tr, ctx->serverInfo.userSpeed, input.meta.dt);
                }
                entity.set(tr);
            }
        }
    }

    void PlayerModule::OnKeyEventHandler(const KeyEvent& event, const flecs::world& world)
    {
        const auto* ctx    = ClientWorldContext::Get(world);
        const auto  handle = ctx->networkMapping.GetLHandle(ctx->playerInfo.handle);
        if (!handle.has_value())
        {
            MCC_LOG_WARN("[OnKeyEventHandler] Player entity({}) isn't associated to a local entity", ctx->playerInfo.handle);
            return;
        }

        const auto entity = world.entity(*handle);
        auto&      input  = entity.get_mut<CCurrentPlayerInput>();
        if (event.action == GLFW_PRESS || event.action == GLFW_RELEASE)
        {
            switch (event.key)
            {
                case GLFW_KEY_W:
                    input.movement.forward = event.action == GLFW_PRESS;
                    break;
                case GLFW_KEY_S:
                    input.movement.backward = event.action == GLFW_PRESS;
                    break;
                case GLFW_KEY_A:
                    input.movement.left = event.action == GLFW_PRESS;
                    break;
                case GLFW_KEY_D:
                    input.movement.right = event.action == GLFW_PRESS;
                    break;
                case GLFW_KEY_SPACE:
                    input.movement.up = event.action == GLFW_PRESS;
                    break;
                case GLFW_KEY_LEFT_SHIFT:
                    input.movement.down = event.action == GLFW_PRESS;
                    break;
                default:
                    break;
            }
        }
    }

    void PlayerModule::OnCursorPosEventHandler(const CursorPosEvent& event, const flecs::world& world)
    {
        auto [w, h] = event.window.GetWindowSize();

        const auto* ctx    = ClientWorldContext::Get(world);
        const auto  handle = ctx->networkMapping.GetLHandle(ctx->playerInfo.handle);
        if (!handle.has_value())
        {
            MCC_LOG_WARN("[OnCursorPosEventHandler] Player entity({}) isn't associated to a local entity", ctx->playerInfo.handle);
            return;
        }

        const auto entity = world.entity(*handle);
        auto&      input  = entity.get_mut<CCurrentPlayerInput>();
        if (event.x >= 0 && event.x <= w && event.y >= 0 && event.y <= h && event.window.IsFocused())
        {
            event.window.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            input.axis.x += ctx->settings.sensitivity * ((static_cast<float>(w) / 2.f) - static_cast<float>(event.x));
            input.axis.y += ctx->settings.sensitivity * ((static_cast<float>(h) / 2.f) - static_cast<float>(event.y));

            event.window.SetCursorPosition(w / 2, h / 2);
        }
    }

}
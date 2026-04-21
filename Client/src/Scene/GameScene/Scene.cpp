// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Scene/Scene.h"

#include "Client/Graphics/Window/Event.h"
#include "Client/Module/ServerSession/Component.h"
#include "Client/Scene/GameScene/Component.h"
#include "Client/Scene/GameScene/Scene.h"
#include "Client/Scene/GameScene/System.h"
#include "Client/WorldContext.h"

#include "Common/Module/Network/Component.h"

namespace Mcc
{

    SceneModule<GameScene>::SceneModule(flecs::world& world) : BaseModule(world)
    {
        world.add<TClient>();

        const auto ctx = ClientWorldContext::Get(world);
        ctx->window.Subscribe<KeyEvent>(KeyEventHandler, world);
    }

    void SceneModule<GameScene>::RegisterComponent(flecs::world& world)
    {
        GameState::Register(world);
    }

    void SceneModule<GameScene>::RegisterPrefab(flecs::world& /* world */) {}

    void SceneModule<GameScene>::RegisterSystem(flecs::world& world)
    {
        static DebugContext debugContext {
            .fpsHistory      = std::vector(100, 0.f),
            .fpsHistorySize  = 100,
            .fpsHistoryIndex = 0,

            .incomingBandwidthHistory = { 100ul, .2f },
            .outgoingBandwidthHistory = { 100ul, .2f },
            .incomingDataTotal        = 0
        };

        world.system("SetupStateSystem")
            .kind<Phase::OnLoad>()
            .run(SetupGameStateSystem)
            .add<GameScene>();

        world.system("ConnectToServerSystem")
            .kind<Phase::OnLoad>()
            .run(ConnectToServerSystem)
            .add<GameScene>();

        world.system("DisplayEscapeMenuSystem")
            .kind<Phase::OnDrawGui>()
            .with<GameState, GameState::InMenu>().src<SceneRoot>()
            .each(DisplayEscapeMenuSystem)
            .add<GameScene>();

        world.system("DisplayLoadScreenSystem")
            .kind<Phase::OnDrawGui>()
            .with<GameState, GameState::Load>().src<SceneRoot>()
            .each(DisplayLoadScreenSystem)
            .add<GameScene>();

        world.system("DisplayDebugMenuSystem")
            .kind<Phase::OnDrawGui>()
            .ctx(&debugContext)
            .run(DisplayDebugMenuSystem)
            .add<GameScene>();

        world.system("JoinPendingMeshTaskSystem")
            .kind<Phase::OnUpdate>()
            .with<GameState, GameState::Shutdown>().src<SceneRoot>()
            .each(JoinPendingMeshTaskSystem)
            .add<GameScene>();

        world.system("ClearGameInfoSystem")
            .kind<Phase::OnQuit>()
            .run(ClearGameInfoSystem)
            .add<GameScene>();

        world.system("DisconnectFromServerSystem")
            .kind<Phase::OnQuit>()
            .with<SrvConnState, SrvConnState::Connected>().src<SceneRoot>()
            .run(DisconnectFromServerSystem)
            .add<GameScene>();
    }

    void SceneModule<GameScene>::RegisterObserver(flecs::world& world)
    {
        GameState::InGame  ::OnEnter(world).run(OnEnterGameStateInGame);
        GameState::InGame  ::OnExit (world).run(OnExitGameStateInGame);
        GameState::Shutdown::OnEnter(world).run(OnEnterGameStateShutdown);

        SessionState::Active   ::OnEnter(world).run(OnEnterSessionStateActive);
        SessionState::Active   ::OnExit (world).run(OnSessionOrConnLost);
        SrvConnState::Connected::OnExit (world).run(OnSessionOrConnLost);
    }

    void SceneModule<GameScene>::KeyEventHandler(const KeyEvent& event, const flecs::world& world)
    {
        if (event.action == GLFW_PRESS)
        {
            switch (event.key)
            {
                case GLFW_KEY_ESCAPE:
                    if (GameState::InGame::IsActive(world))
                    {
                        GameState::InMenu::Enter(world);
                        break;
                    }

                    if (GameState::InMenu::IsActive(world))
                    {
                        GameState::InGame::Enter(world);
                        break;
                    }
                default:
                    break;
            }
        }
    }

}

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
        world.add<ClientTag>();
    }

    void SceneModule<GameScene>::RegisterComponent(flecs::world& world)
    {
        GameState::Register(world);
    }

    void SceneModule<GameScene>::RegisterSystem(flecs::world& world)
    {
        static DebugContext debugContext {
            .fpsHistory      = std::vector(100, 0.f),
            .fpsHistorySize  = 100,
            .fpsHistoryIndex = 0
        };

        world.system("SetupStateSystem")
            .kind<Phase::OnLoad>()
            .run(SetupStateSystem)
            .add<GameScene>();

        world.system("ConnectToServerSystem")
            .kind<Phase::OnLoad>()
            .run(ConnectToServerSystem)
            .add<GameScene>();

        world.system("DisplayEscapeMenuSystem")
            .kind<Phase::OnDrawGui>()
            .with<GameState, GameState::InMenu>()
            .src<SceneRoot>()
            .each(DisplayEscapeMenuSystem)
            .add<GameScene>();

        world.system("DisplayLoadScreenSystem")
            .kind<Phase::OnDrawGui>()
            .with<GameState, GameState::Load>()
            .src<SceneRoot>()
            .each(DisplayLoadScreenSystem)
            .add<GameScene>();

        world.system("DisplayDebugMenuSystem")
            .kind<Phase::OnDrawGui>()
            .ctx(&debugContext)
            .run(DisplayDebugMenuSystem)
            .add<GameScene>();

        world.system("JoinPendingMeshTaskSystem")
            .kind<Phase::OnUpdate>()
            .with<GameState, GameState::Shutdown>()
            .src<SceneRoot>()
            .each(JoinPendingMeshTaskSystem)
            .add<GameScene>();

        world.system("ClearGameInfoSystem")
            .kind<Phase::OnQuit>()
            .run(ClearGameInfoSystem)
            .add<GameScene>();

        world.system("DisconnectFromServerSystem")
            .kind<Phase::OnQuit>()
            .run(DisconnectFromServerSystem)
            .add<GameScene>();
    }

    void SceneModule<GameScene>::RegisterHandler(flecs::world& world)
    {
        GameState::InGame  ::OnEnter(world).run(OnEnterGameStateInGame);
        GameState::InGame  ::OnExit (world).run(OnExitGameStateInGame);
        GameState::Shutdown::OnEnter(world).run(OnEnterGameStateShutdown);

        ServerConnectionState::Connected::OnEnter(world).run([](flecs::iter& it) {
            GameState::InGame::Enter(it.world());
            it.fini();
        });

        ServerConnectionState::Error::OnEnter(world).run([](flecs::iter& it) {
            it.world().should_quit();
            it.fini();
        });

        const auto ctx = ClientWorldContext::Get(world);
        ctx->window.Subscribe<KeyEvent>(KeyEventHandler, world);
    }

    void SceneModule<GameScene>::KeyEventHandler(const KeyEvent& event, const flecs::world& world)
    {
        if (event.action == GLFW_PRESS)
        {
            switch (event.key)
            {
                case GLFW_KEY_ESCAPE:
                    if (GameState::InGame::IsActive(world))
                        GameState::InMenu::Enter(world);
                    else
                        GameState::InGame::Enter(world);
                default:
                    break;
            }
        }
    }

}

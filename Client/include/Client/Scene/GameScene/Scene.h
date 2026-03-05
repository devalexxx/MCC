// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_SCENE_GAME_SCENE_SCENE_H
#define MCC_CLIENT_SCENE_GAME_SCENE_SCENE_H

#include "Client/Graphics/Window/Event.h"
#include "Client/Module/Camera/Module.h"
#include "Client/Module/EntityRenderer/Module.h"
#include "Client/Module/EntityReplication/Module.h"
#include "Client/Module/ImGui/Module.h"
#include "Client/Module/Player/Module.h"
#include "Client/Module/ServerSession/Module.h"
#include "Client/Module/TerrainRenderer/Module.h"
#include "Client/Module/TerrainReplication/Module.h"

#include "Common/Module/Base/Module.h"
#include "Common/SceneImporter.h"
#include "Common/State.h"

#include <flecs.h>

namespace Mcc
{
    REGISTER_STATE(StateTargetScene, GameState, Load, InGame, InMenu, Shutdown)

    struct GameScene;

    template<>
    struct SceneModule<GameScene> final :
        BaseModule<
            SceneModule<GameScene>,
            CameraModule,
            EntityRendererModule,
            EntityReplicationModule,
            ImGuiModule,
            PlayerModule,
            TerrainRendererModule,
            TerrainReplicationModule,
            ServerSessionModule
        >
    {
        SceneModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterPrefab   (flecs::world& world) override;
        void RegisterSystem   (flecs::world& world) override;
        void RegisterObserver (flecs::world& world) override;

        static void KeyEventHandler(const KeyEvent& event, const flecs::world& world);
    };

}

#endif

// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_SCENE_GAME_SCENE_SCENE_H
#define MCC_SERVER_SCENE_GAME_SCENE_SCENE_H

#include "Server/Module/TerrainReplication/Module.h"
#include "Server/Module/EntityReplication/Module.h"
#include "Server/Module/Player/Module.h"
#include "Server/Module/TerrainGeneration/Module.h"
#include "Server/Module/UserSession/Module.h"

#include "Common/Module/Base/Module.h"
#include "Common/SceneImporter.h"

#include "flecs.h"

namespace Mcc
{

    struct GameScene;

    template<>
    struct SceneModule<GameScene> final :
        BaseModule<
            SceneModule<GameScene>,
            UserSessionModule,
            PlayerModule,
            EntityReplicationModule,
            TerrainGenerationModule,
            TerrainReplicationModule
        >
    {
        SceneModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterSystem(flecs::world& world) override;
        void RegisterHandler(flecs::world& world) override;
    };

}

#endif

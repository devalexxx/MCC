// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_SCENE_SCENE_H
#define MCC_SERVER_SCENE_SCENE_H

#include "Server/Scene/GameScene/Scene.h"

#include "Common/SceneImporter.h"

namespace Mcc
{

    struct SSceneImporter;

    struct GameScene : Scene<GameScene, SSceneImporter>
    {};

    using Importer = SceneImporter<GameScene>;

    struct SSceneImporter : Importer
    {
        using Importer::SceneImporter;
    };

}

#endif

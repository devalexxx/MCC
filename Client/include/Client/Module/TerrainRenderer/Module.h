// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_TERRAIN_RENDERER_MODULE_H
#define MCC_CLIENT_MODULE_TERRAIN_RENDERER_MODULE_H

#include "Client/Graphics/OpenGL/OpenGLProgram.h"

#include "Common/Module/Terrain/Module.h"

#include <flecs.h>

namespace Mcc
{

    struct TerrainRendererModule final : BaseModule<TerrainRendererModule, TerrainModule, RendererModule>
    {
        explicit TerrainRendererModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterPrefab   (flecs::world& world) override;
        void RegisterSystem   (flecs::world& world) override;
        void RegisterObserver (flecs::world& world) override;

        OpenGLProgram program;
    };

}

#endif

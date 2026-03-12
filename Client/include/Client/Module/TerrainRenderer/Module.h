// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_TERRAIN_RENDERER_MODULE_H
#define MCC_CLIENT_MODULE_TERRAIN_RENDERER_MODULE_H

#include "Client/Graphics/OpenGL/OpenGLProgram.h"
#include "Client/Graphics/OpenGL/OpenGLTexture2DArray.h"

#include "Common/Module/Terrain/Module.h"
#include "Common/Utils/SafeAccess.h"

#include <flecs.h>

#include <unordered_set>
#include <unordered_map>
#include <string>

namespace Mcc
{

    struct TerrainRendererModule final : BaseModule<TerrainRendererModule, TerrainModule, RendererModule>
    {
        explicit TerrainRendererModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterPrefab   (flecs::world& world) override;
        void RegisterSystem   (flecs::world& world) override;
        void RegisterObserver (flecs::world& world) override;

        flecs::entity programEntity;
        flecs::entity textureArrayEntity;

        std::shared_ptr<OpenGLProgram>        program;
        std::shared_ptr<OpenGLTexture2DArray> textureArray;

        std::shared_ptr<SafeAccess<std::unordered_map<std::string, size_t>>> textureIndex;
        std::shared_ptr<SafeAccess<std::unordered_set<std::string>>>         textureToLoad;
    };

}

#endif

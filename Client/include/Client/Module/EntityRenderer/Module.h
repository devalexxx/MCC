// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_ENTITY_RENDERER_MODULE_H
#define MCC_CLIENT_MODULE_ENTITY_RENDERER_MODULE_H

#include "Client/Graphics/Buffer.h"
#include "Client/Graphics/Program.h"
#include "Client/Graphics/VertexArray.h"
#include "Client/Module/Renderer/Module.h"

#include "Common/Module/Base/Module.h"

#include <flecs.h>

namespace Mcc
{

    struct EntityRendererModule final : BaseModule<EntityRendererModule, RendererModule>
    {
        explicit EntityRendererModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterPrefab   (flecs::world& world) override;
        void RegisterSystem   (flecs::world& world) override;
        void RegisterObserver (flecs::world& world) override;

        VertexArray vertexArray;
        Buffer      vertexBuffer;
        Buffer      indexBuffer;
        size_t      indexCount;
        Program     program;
    };
}

#endif

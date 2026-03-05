// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_RENDERER_MODULE_H
#define MCC_CLIENT_MODULE_RENDERER_MODULE_H

#include "Common/Module/Base/Module.h"

#include <glm/fwd.hpp>

#include <flecs.h>

#include <functional>

namespace Mcc
{

    struct RendererModule final : BaseModule<RendererModule>
    {
        RendererModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterPrefab   (flecs::world& world) override;
        void RegisterSystem   (flecs::world& world) override;
        void RegisterObserver (flecs::world& world) override;

        static std::tuple<glm::vec3, glm::mat4, glm::mat4> GetView(const flecs::world& world);
    };

}

#endif

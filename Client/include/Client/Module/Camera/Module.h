// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_CAMERA_MODULE_H
#define MCC_CLIENT_MODULE_CAMERA_MODULE_H

#include "Common/Module/Base/Module.h"
#include "Common/Module/Entity/Module.h"

#include <flecs.h>

namespace Mcc
{

    struct CameraModule final : BaseModule<CameraModule, EntityModule>
    {
        explicit CameraModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterPrefab   (flecs::world& world) override;
        void RegisterSystem   (flecs::world& world) override;
        void RegisterObserver (flecs::world& world) override;

        static flecs::entity GetActiveCamera(const flecs::world& world);
    };

}

#endif

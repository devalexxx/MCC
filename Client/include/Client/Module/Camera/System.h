// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_CAMERA_SYSTEM_H
#define MCC_CLIENT_MODULE_CAMERA_SYSTEM_H

#include "Client/Module/Camera/Component.h"

#include "Common/Module/Entity/Component.h"

#include <flecs.h>

namespace Mcc
{

    void CameraFollowSystem(
        const flecs::iter& it, size_t, CEntityTransform& transform, const CCameraFollowSettings& settings
    );

}

#endif

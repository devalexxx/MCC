// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_CAMERA_COMPONENT_H
#define MCC_CLIENT_MODULE_CAMERA_COMPONENT_H

#include "Common/World/Translation.h"

namespace Mcc
{

    struct TCamera       {};
    struct TCameraFollow {};
    struct TActiveCamera {};

    struct RCameraFollow {};

    struct PCamera       {};
    struct PCameraFollow {};

    struct CCameraSettings
    {
        float fov;
        float zNear;
        float zFar;
    };

    struct CCameraFollowSettings
    {
        TranslationF offset;
    };

}

#endif

// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/Camera/System.h"

#include "Common/World/Geometry.h"

namespace Mcc
{

    void CameraFollowSystem(const flecs::iter& it, size_t, CEntityTransform& transform, const CCameraFollowSettings& settings)
    {
        const auto target = it.pair(3).second();

        target.get([&](const CEntityTransform& targetTransform)
        {
            transform.position = targetTransform.position + TranslationE(settings.offset);
        });
    }

}
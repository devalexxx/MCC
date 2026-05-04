// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_PLAYER_SYSTEM_H
#define MCC_CLIENT_MODULE_PLAYER_SYSTEM_H

#include "Common/Module/Entity/Component.h"
#include "Common/Network/Packet.h"

#include <flecs.h>

namespace Mcc
{
    // System
    void SetupFacingBlockSystem (flecs::iter& it);
    void ApplyAndSendPlayerInput(const flecs::iter& it, size_t row, UserInput& current, CUserInputQueue& queue);
    void SetFacingBlockSystem   (flecs::iter& it, size_t row, const CEntityTransform& transform);
    void DrawCrosshairSystem    (flecs::iter& it);

}

#endif

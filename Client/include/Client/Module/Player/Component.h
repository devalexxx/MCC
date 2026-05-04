// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_PLAYER_COMPONENT_H
#define MCC_CLIENT_MODULE_PLAYER_COMPONENT_H

#include "Common/Module/Entity/Component.h"
#include "Common/Utils/FlecsUtils.h"
#include "Common/Utils/Raycast.h"

namespace Mcc
{

    struct TPlayerEntity {};

    struct PPlayerEntity {};

    struct CFacingInfo
    {
        WorldPosV   previous;
        WorldPosV   position;
        const char* blockName;
        float       distance;
    };

    namespace _ { struct PlayerModuleTag {}; };
    using CCurrentPlayerInput = ComponentWrapper<UserInput, _::PlayerModuleTag>;

}

#endif

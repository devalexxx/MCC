// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_PLAYER_COMPONENT_H
#define MCC_SERVER_MODULE_PLAYER_COMPONENT_H

#include "Common/World/Position.h"

namespace Mcc
{

    struct OnPlayerCreatedEvent {};
    struct OnPlayerMoveEvent
    {
        WorldPosE prev;
        WorldPosE curr;
    };

}

#endif

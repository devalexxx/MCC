// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_PLAYER_COMPONENT_H
#define MCC_SERVER_MODULE_PLAYER_COMPONENT_H

#include <glm/vec3.hpp>

namespace Mcc
{

    struct OnPlayerCreatedEvent {};
    struct OnPlayerMoveEvent
    {
        glm::vec3 prev;
        glm::vec3 curr;
    };

}

#endif

// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_SERVER_SESSION_SYSTEM_H
#define MCC_CLIENT_MODULE_SERVER_SESSION_SYSTEM_H

#include "Client/Module/ServerSession/Component.h"

#include "flecs.h"

namespace Mcc
{

    void SetupSessionStateSystem        (const flecs::iter& it);
    void HandleConnectionResultSystem   (const flecs::iter& it, size_t, SrvConnResult&    result);
    void HandleDisconnectionResultSystem(const flecs::iter& it, size_t, SrvDConnResult& result);

}

#endif

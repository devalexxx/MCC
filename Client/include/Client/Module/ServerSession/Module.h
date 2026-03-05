// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_SERVER_SESSION_MODULE_H
#define MCC_CLIENT_MODULE_SERVER_SESSION_MODULE_H

#include "Common/Module/Base/Module.h"
#include "Common/Network/Event.h"
#include "Common/Network/Packet.h"

#include <flecs.h>

namespace Mcc
{

    struct ServerSessionModule final : BaseModule<ServerSessionModule>
    {
        explicit ServerSessionModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterPrefab   (flecs::world& world) override;
        void RegisterSystem   (flecs::world& world) override;
        void RegisterObserver (flecs::world& world) override;

        static void Connect   (const flecs::world& world);
        static void Disconnect(const flecs::world& world);

      private:
        static void OnWaitingInfoHandler       (const OnWaitingInfo&, const flecs::world& world);
        static void OnConnectionAcceptedHandler(const OnConnectionAccepted& packet, const flecs::world& world);
        static void OnConnectionRefusedHandler (const OnConnectionRefused& packet, const flecs::world& world);
        static void OnDisconnectEventHandler   (const DisconnectEvent& event, const flecs::world& world);
    };

}

#endif

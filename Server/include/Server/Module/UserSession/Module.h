// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_USER_SESSION_MODULE_H
#define MCC_SERVER_MODULE_USER_SESSION_MODULE_H

#include "Common/Module/Base/Module.h"
#include "Common/Module/Network/Module.h"
#include "Common/Module/Entity/Module.h"
#include "Common/Network/Event.h"
#include "Common/Network/Packet.h"
#include "Common/Network/Packet/Session.h"
#include "Common/Utils/SafeAccess.h"

#include <flecs.h>

namespace Mcc
{
    struct CNetProps;

    using EntitySet = std::unordered_set<flecs::entity_t>;

    struct UserSession
    {
        static UserSession* Get(const ENetPeer* peer);

        PlayerInfo pInfo;
        ClientInfo cInfo;

        ENetPeer*             peer;
        SafeAccess<EntitySet> replicatedChunks;
    };

    struct UserSessionModule final : BaseModule<UserSessionModule, NetworkModule, EntityModule>
    {
        UserSessionModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterPrefab   (flecs::world& world) override;
        void RegisterSystem   (flecs::world& world) override;
        void RegisterObserver (flecs::world& world) override;

      private:
        static void OnConnectEventHandler   (const ConnectEvent& event, const flecs::world& world);
        static void OnClientInfoHandler     (const From<OnClientInfo>& from, const flecs::world& world);
        static void OnDisconnectEventHandler(const DisconnectEvent& event, const flecs::world& world);
    };

}

#endif

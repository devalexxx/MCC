// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_CLIENT_NETWORK_MANAGER_H
#define MCC_CLIENT_CLIENT_NETWORK_MANAGER_H

#include "Common/Network/NetworkManager.h"

namespace Mcc
{

    class CommandLineStore;

    class ClientNetworkManager final : public NetworkManager
    {
      public:
        explicit ClientNetworkManager(const CommandLineStore& cmdLineStore);
        ~ClientNetworkManager() override;

        int Setup() override;

        int Connect();
        int Disconnect() const;

        const ENetPeer* GetPeer() const;
        ENetPeer*       GetPeer();

        const ENetHost* GetHost() const;
        ENetHost*       GetHost();

        template<typename T>
        int Send(T data, enet_uint32 flag, enet_uint8 channel) const;

      private:
        ENetPeer* mPeer;
    };

}

#include <Client/ClientNetworkManager.inl>

#endif

// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_SERVER_NETWORK_MANAGER_H
#define MCC_SERVER_SERVER_NETWORK_MANAGER_H

#include "Common/Network/NetworkManager.h"

namespace Mcc
{

    class CommandLineStore;

    class ServerNetworkManager final : public NetworkManager
    {
      public:
        explicit ServerNetworkManager(const CommandLineStore& cmdLineStore);

        int Setup() override;

        template<typename T>
        int Send(ENetPeer* peer, T data, enet_uint32 flag, enet_uint8 channel) const;
        template<typename T>
        int Send(std::vector<ENetPeer*> peers, T data, enet_uint32 flag, enet_uint8 channel) const;

        template<typename T>
        void Broadcast(T data, enet_uint32 flag, enet_uint8 channel) const;

      private:
        size_t mMaxPeer;

        friend class ServerApplication;
    };

}

#include <Server/ServerNetworkManager.inl>

#endif
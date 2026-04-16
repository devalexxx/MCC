// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_NETWORK_NETWORK_MANAGER_H
#define MCC_COMMON_NETWORK_NETWORK_MANAGER_H

#include "Common/Network/EventManager.h"

#include <enet/enet.h>

#include <mutex>

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 50505

namespace Mcc
{

    class MCC_LIB_API NetworkManager : public NetworkEventManager
    {
      public:
        NetworkManager(const char* host, unsigned short port);
        virtual ~NetworkManager();

        void Poll();

        virtual int Setup() = 0;

      protected:
        static ENetPacket* CreatePacket(ByteSpan&& data, enet_uint32 flag);

        template<typename T>
        int Send(ENetPeer* peer, T data, enet_uint32 flag, enet_uint8 channel) const;
        template<typename T>
        int Send(std::vector<ENetPeer*> peers, T data, enet_uint32 flag, enet_uint8 channel) const;

        int CreateHost(const ENetAddress* addr, size_t peers, size_t channels, enet_uint32 in, enet_uint32 out);

        ENetAddress mAddr;
        ENetHost*   mHost;

        mutable std::mutex                        mMutex;
        mutable std::queue<std::function<void()>> mCommandQueue;
    };

}

#include <Common/Network/NetworkManager.inl>

#endif
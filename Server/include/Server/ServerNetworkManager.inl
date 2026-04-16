// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include <Hexis/Core/TypeName.h>

namespace Mcc
{

    template<typename T>
    int ServerNetworkManager::Send(std::vector<ENetPeer*> peers, T data, enet_uint32 flag, enet_uint8 channel) const
    {
        return NetworkManager::Send(std::move(peers), std::move(data), flag, channel);
    }

    template<typename T>
    int ServerNetworkManager::Send(ENetPeer* peer, T data, enet_uint32 flag, enet_uint8 channel) const
    {
        return NetworkManager::Send(peer, std::move(data), flag, channel);
    }

    template<typename T>
    void ServerNetworkManager::Broadcast(T data, enet_uint32 flag, enet_uint8 channel) const
    {
        std::lock_guard guard(mMutex);

        PacketOutputStream          stream;
        cereal::BinaryOutputArchive archive(stream);

        size_t type = PacketList::IndexOf<T>;
        archive(type);
        archive(data);

        auto packet = CreatePacket(stream.GetBuffer(), flag);
        if (packet)
        {
            mCommandQueue.push([=, this] { enet_host_broadcast(mHost, channel, packet); });
            return;
        }
        MCC_LOG_WARN("Failed to create packet {}", Hx::TypeName<T>());
    }

}
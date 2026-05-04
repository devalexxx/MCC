// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Network/EventManager.h"

#include "Common/Utils/Benchmark.h"
#include "Common/Utils/Logging.h"

#include "zlib.h"

namespace Mcc
{

    NetworkEventManager::NetworkEventManager() : EventManager<NetworkEventTag>()
    {
        PacketList::Apply<HandlerMapper>({ *this });
    }

    void NetworkEventManager::DispatchPacket(ENetPeer* peer, const ENetPacket* packet)
    {
        PacketInputStream          stream (reinterpret_cast<char*>(packet->data), packet->dataLength);
        cereal::BinaryInputArchive archive(stream);

        size_t index;
        archive(index);

        if (index < PacketList::Count)
            mHandlers[index](peer, packet, *this, std::move(archive));
        else
            Dispatch<MalformedPacketEvent>({ packet });
    }

}
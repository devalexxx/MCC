// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Utils/Benchmark.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    template<typename T>
    int NetworkManager::Send(const std::vector<ENetPeer*> peers, T data, const enet_uint32 flag, enet_uint8 channel) const
    {
        std::lock_guard guard(mMutex);

        PacketOutputStream          stream;
        cereal::BinaryOutputArchive archive(stream);

        size_t type = PacketList::IndexOf<T>;
        archive(type);

        auto f = [&] { archive(data); };
        MCC_BENCH_TIME(Serialization, f)();

        if (const auto packet = CreatePacket(stream.GetBuffer(), flag))
        {
            mCommandQueue.push([packet, channel, ps = std::move(peers)]
            {
                for (const auto peer: ps)
                {
                    if (!peer) continue;
                    if (peer->state != ENET_PEER_STATE_CONNECTED) continue;

                    enet_peer_send(peer, channel, packet);
                }
            });
            return EXIT_SUCCESS;
        }

        return EXIT_FAILURE;
    }

    template<typename T>
    int NetworkManager::Send(ENetPeer* peer, T data, enet_uint32 flag, enet_uint8 channel) const
    {
        std::lock_guard guard(mMutex);

        PacketOutputStream          stream;
        cereal::BinaryOutputArchive archive(stream);

        size_t type = PacketList::IndexOf<T>;
        archive(type);

        auto f = [&] { archive(data); };
        MCC_BENCH_TIME(Serialization, f)();

        if (const auto packet = CreatePacket(stream.GetBuffer(), flag))
        {
            mCommandQueue.push([=] { enet_peer_send(peer, channel, packet); });
            return EXIT_SUCCESS;
        }

        return EXIT_FAILURE;
    }

}
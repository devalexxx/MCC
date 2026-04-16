// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Network/NetworkManager.h"

#include "Common/Network/Event.h"
#include "Common/Utils/Benchmark.h"
#include "Common/Utils/Logging.h"

#include "zlib.h"

namespace Mcc
{

    NetworkManager::NetworkManager(const char* host, unsigned short port) : mAddr(), mHost(nullptr)
    {
        if (enet_initialize())
        {
            MCC_LOG_ERROR("Failed to initialize Enet");
            exit(EXIT_FAILURE);
        }
        atexit(enet_deinitialize);

        enet_address_set_host(&mAddr, host);
        mAddr.port = port;
    }

    NetworkManager::~NetworkManager()
    {
        if (mHost)
            enet_host_destroy(mHost);
    }


    void NetworkManager::Poll()
    {
        for (; !mCommandQueue.empty(); mCommandQueue.pop()) { mCommandQueue.front()(); }

        ENetEvent event;
        while (enet_host_service(mHost, &event, 0) > 0)
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_CONNECT: {
                    Dispatch<ConnectEvent>({ event.peer });
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    Dispatch<DisconnectEvent>({ event.peer });
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    DispatchPacket(event.peer, event.packet);
                    enet_packet_destroy(event.packet);
                    break;
                }
                default:
                    MCC_LOG_ERROR("Unhandled network event type");
                    break;
            }
        }
    }

    ENetPacket* NetworkManager::CreatePacket(ByteSpan&& data, enet_uint32 flag)
    {
        const size_t         baseLength = data.size();
        size_t               length     = compressBound(data.size());
        std::vector<uint8_t> buffer(length);
        if (
            MCC_BENCH_TIME(Compression, compress)(
                buffer.data(),
                reinterpret_cast<uLongf*>(&length),
                reinterpret_cast<uint8_t*>(data.data()),
                data.size()
            )
        != Z_OK)
        {
            MCC_LOG_ERROR("Failed to compress data");
        }
        buffer.resize(length);

        std::vector<uint8_t> packet(length + sizeof(length));
        std::memcpy(packet.data(), &baseLength, sizeof(length));
        std::memcpy(packet.data() + sizeof(length), buffer.data(), buffer.size());

        return enet_packet_create(packet.data(), length + sizeof(length), flag);
    }

    int
    NetworkManager::CreateHost(const ENetAddress* addr, size_t peers, size_t channels, enet_uint32 in, enet_uint32 out)
    {
        mHost = enet_host_create(addr, peers, channels, in, out);
        if (!mHost)
        {
            MCC_LOG_ERROR("Failed to create ENet host");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

}
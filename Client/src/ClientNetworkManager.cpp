// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/ClientNetworkManager.h"

#include "Common/CommandLineStore.h"
#include "Common/Utils/Logging.h"

#include <charconv>

namespace Mcc
{

    ClientNetworkManager::ClientNetworkManager(const CommandLineStore& cmdLineStore) :
        NetworkManager(DEFAULT_HOST, DEFAULT_PORT),
        mPeer(nullptr)
    {
        CommandLineStore::OptParameter param;
        if ((param = cmdLineStore.GetParameter("host").or_else([&] { return cmdLineStore.GetParameter("h"); })
            ).has_value())
            enet_address_set_host(&mAddr, param->data());

        if ((param = cmdLineStore.GetParameter("port").or_else([&] { return cmdLineStore.GetParameter("p"); })
            ).has_value())
            std::from_chars(param->data(), param->data() + param->size(), mAddr.port);
    }

    ClientNetworkManager::~ClientNetworkManager()
    {
        if (mPeer)
            enet_peer_reset(mPeer);
    }

    int ClientNetworkManager::Setup()
    {
        return CreateHost(nullptr, 1, 2, 0, 0);
    }

    /**
     *
     * @return 0 if success, 1 if failure
     */
    int ClientNetworkManager::Connect()
    {
        ENetEvent event;
        mPeer = enet_host_connect(mHost, &mAddr, 2, 0);
        if (!mPeer)
        {
            MCC_LOG_ERROR("No available peers for initiating connection");
            return EXIT_FAILURE;
        }

        if (enet_host_service(mHost, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
        {
            Dispatch<ConnectEvent>({ event.peer });
        }
        else
        {
            return EXIT_FAILURE;
        }
        enet_host_flush(mHost);

        return EXIT_SUCCESS;
    }

    /**
     *
     * @return 0 if success, 1 if failure
     */
    int ClientNetworkManager::Disconnect() const
    {
        ENetEvent event;
        if (mPeer->state != ENET_PEER_STATE_DISCONNECTED)
        {
            enet_peer_disconnect(mPeer, 0);
            while (enet_host_service(mHost, &event, 3000) > 0)
            {
                switch (event.type)
                {
                    case ENET_EVENT_TYPE_RECEIVE:
                        enet_packet_destroy(event.packet);
                        break;
                    case ENET_EVENT_TYPE_DISCONNECT:
                        return EXIT_SUCCESS;
                    default:
                        break;
                }
            }
            MCC_LOG_ERROR("Failed to disconnect properly");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const ENetPeer* ClientNetworkManager::GetPeer() const
    {
        return mPeer;
    }


}
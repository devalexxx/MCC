// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/ServerNetworkManager.h"

#include "Common/CommandLineStore.h"

#include <charconv>

namespace Mcc
{

    ServerNetworkManager::ServerNetworkManager(const CommandLineStore& cmdLineStore) :
        NetworkManager(DEFAULT_HOST, DEFAULT_PORT),
        mMaxPeer(32)
    {
        if (const auto param = cmdLineStore.GetParameter("host"); param)
            enet_address_set_host(&mAddr, param->data());

        if (const auto param = cmdLineStore.GetParameter("port"); param)
            std::from_chars(param->data(), param->data() + param->size(), mAddr.port);

        if (const auto param = cmdLineStore.GetParameter("peers"); param)
            std::from_chars(param->data(), param->data() + param->size(), mMaxPeer);
    }

    int ServerNetworkManager::Setup()
    {
        return CreateHost(&mAddr, mMaxPeer, 2, 0, 0);
    }

}
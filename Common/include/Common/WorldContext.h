// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_WORLD_CONTEXT_H
#define MCC_COMMON_WORLD_CONTEXT_H

#include "Common/AssetRegistry.h"
#include "Common/Network/NetworkHandle.h"
#include "Common/Network/NetworkManager.h"
#include "Common/Network/Packet.h"
#include "Common/World/Position.h"
#include "Common/World/Hash.h"

#include "Hexis/Core/TaskScheduler.h"

#include <unordered_map>


namespace Mcc
{

    struct MCC_LIB_API NetworkMapping
    {
        using LHandle = flecs::entity_t;
        using RHandle = NetworkHandle;

        std::unordered_map<LHandle, RHandle> localToRemote {};
        std::unordered_map<RHandle, LHandle> remoteToLocal {};

        void Set(RHandle rHandle, LHandle lHandle);

        void RRemove(RHandle handle);
        void LRemove(LHandle handle);

        [[nodiscard]] std::optional<LHandle> GetLHandle(RHandle handle) const;
        [[nodiscard]] std::optional<RHandle> GetRHandle(LHandle handle) const;
    };

    using ChunkMapping = std::unordered_map<ChunkPosV, flecs::entity_t>;

    template<typename NetworkManager = NetworkManager>
    struct WorldContext
    {
        static WorldContext* Get(const flecs::world& world);

        NetworkManager&    networkManager;
        AssetRegistry&     assetRegistry;
        Hx::TaskScheduler& scheduler;

        NetworkMapping networkMapping;
        ChunkMapping   chunkMapping;
    };
}

#include "Common/WorldContext.inl"

#endif

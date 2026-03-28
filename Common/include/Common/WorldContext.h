// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_WORLD_CONTEXT_H
#define MCC_COMMON_WORLD_CONTEXT_H

#include "Common/Network/NetworkHandle.h"
#include "Common/Network/NetworkManager.h"
#include "Common/Network/Packet.h"
#include "Common/AssetRegistry.h"

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

    // clang-format off
    struct MCC_LIB_API IVec3Hasher
    {
        size_t operator()(const glm::ivec3& v) const
        {
            return std::hash<int>()(v.x) ^ std::hash<int>()(v.y) << 1 ^ std::hash<int>()(v.z) << 2;
        }
    };

    template<typename T>
    concept IsNetworkManager = std::is_base_of_v<NetworkManager, T> || std::is_same_v<NetworkManager, T>;

    template<IsNetworkManager NManager = NetworkManager>
    struct WorldContext
    {
        static WorldContext* Get(const flecs::world& world);

        NManager&          networkManager;
        NetworkMapping     networkMapping;
        Hx::TaskScheduler& scheduler;
        AssetRegistry&     assetRegistry;

        std::unordered_map<glm::ivec3, flecs::entity_t, IVec3Hasher> chunkMap;
    };

// clang-format on
}

#include "Common/WorldContext.inl"

#endif

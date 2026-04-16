// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_NETWORK_PACKET_H
#define MCC_COMMON_NETWORK_PACKET_H

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Terrain/Component.h"
#include "Common/Network/NetworkHandle.h"
#include "Common/Network/Packet/Session.h"
#include "Common/Network/PacketStream.h"
#include "Common/World/Position.h"

#include <Hexis/Core/TypeList.h>

#include <vector>

namespace Mcc
{

    template<typename T>
    struct From
    {
        ENetPeer* peer;
        T         packet;
    };

    // clang-format off
    using PacketList = Hx::TypeList<
        // Session
        OnWaitingInfo,
        OnClientInfo,
        OnConnectionAccepted,
        OnConnectionRefused,
        OnClientInfoChanged,

        struct OnChunk,
        struct OnChunkUpdated,

        struct OnBlockBreak,
        struct OnBlockPlace,

        struct OnPlayerInput,
        struct OnEntitiesCreated,
        struct OnEntitiesDestroyed,
        struct OnEntitiesUpdated
    >;
    // clang-format on

    struct EntityState
    {
        NetworkHandle handle;

        CEntityTransform transform;
        CEntityDataMap   data;
    };

    template<typename Archive>
    void serialize(Archive& ar, EntityState& packet);

    struct OnPlayerInput
    {
        UserInput input;
    };

    struct OnEntitiesCreated
    {
        std::vector<EntityState> states;
    };

    struct OnEntitiesDestroyed
    {
        std::vector<NetworkHandle> handles;
    };

    struct OnEntitiesUpdated
    {
        std::vector<EntityState> states;
    };

    struct BlockData
    {
        NetworkHandle handle;
        CBlockType    type;
        CBlockMeta    meta;
        CBlockAsset   asset;
    };

    struct OnChunk
    {
        NetworkHandle          handle;
        ChunkPosV              position;
        RLEChunkData           data;
        std::vector<BlockData> blocks;
    };

    struct OnChunkUpdated
    {
        struct Update
        {
            NetworkHandle            blockHandle;
            LocalPosV                position;
            std::optional<BlockData> blockOpt;
        };

        NetworkHandle       chunkHandle;
        std::vector<Update> updates;
    };

    struct OnBlockBreak
    {
        NetworkHandle chunkHandle;
        LocalPosV     position;
    };

    struct OnBlockPlace
    {
        NetworkHandle chunkHandle;
        LocalPosV     position;
        NetworkHandle blockHandle;
    };

    template<typename Archive>
    void serialize(Archive& ar, BlockData& packet);
    template<typename Archive>
    void serialize(Archive& ar, OnChunk& packet);

    template<typename Archive>
    void serialize(Archive& ar, OnChunkUpdated::Update& packet);
    template<typename Archive>
    void serialize(Archive& ar, OnChunkUpdated& packet);

    template<typename Archive>
    void serialize(Archive& ar, OnBlockBreak& packet);
    template<typename Archive>
    void serialize(Archive& ar, OnBlockPlace& packet);

    template<typename Archive>
    void serialize(Archive& ar, OnPlayerInput& packet);
    template<typename Archive>
    void serialize(Archive& ar, OnEntitiesCreated& packet);
    template<typename Archive>
    void serialize(Archive& ar, OnEntitiesDestroyed& packet);
    template<typename Archive>
    void serialize(Archive& ar, OnEntitiesUpdated& packet);

}

#include <Common/Network/Packet.inl>

#endif
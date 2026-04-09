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

#include <string>
#include <unordered_map>
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
        struct OnChunkBatch,

        struct OnBlock,
        struct OnBlockBatch,

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

    struct OnChunk
    {
        NetworkHandle handle   {};
        ChunkPosV     position {};
        RLEChunkData  data;
    };

    struct OnChunkBatch : std::vector<OnChunk>
    {};

    struct OnBlock
    {
        NetworkHandle handle {};
        CBlockType    type   {};
        CBlockMeta    meta;
        CBlockAsset   asset;
    };

    struct OnBlockBatch : std::vector<OnBlock>
    {};

    template<typename Archive>
    void serialize(Archive& ar, OnChunk& packet);
    template<typename Archive>
    void serialize(Archive& ar, OnBlock& packet);

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
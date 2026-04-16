// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include <cereal/archives/binary.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>

#include "Common/Utils/GLMUtils.h"

namespace Mcc
{

    template<typename Archive>
    void serialize(Archive& ar, EntityState& packet)
    {
        ar(packet.handle, packet.transform, packet.data);
    }

    template<typename Archive>
    void serialize(Archive& ar, BlockData& packet)
    {
        ar(packet.handle, packet.type, packet.meta, packet.asset);
    }

    template<typename Archive>
    void serialize(Archive& ar, OnChunk& packet)
    {
        ar(packet.handle, packet.position, packet.data, packet.blocks);
    }

    template<typename Archive>
    void serialize(Archive& ar, OnPlayerInput& packet)
    {
        ar(packet.input);
    }

    template<typename Archive>
    void serialize(Archive& ar, OnEntitiesCreated& packet)
    {
        ar(packet.states);
    }

    template<typename Archive>
    void serialize(Archive& ar, OnEntitiesDestroyed& packet)
    {
        ar(packet.handles);
    }

    template<typename Archive>
    void serialize(Archive& ar, OnEntitiesUpdated& packet)
    {
        ar(packet.states);
    }

}
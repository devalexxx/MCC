// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Utils/HexisUtils.h"

#include <cereal/types/variant.hpp>
#include <cereal/types/string.hpp>

namespace Mcc
{

    template<typename Archive>
    void serialize(Archive& ar, CBlockMeta& packet)
    {
        ar(packet.id);
    }

    template<typename Archive>
    void serialize(Archive& ar, CBlockAsset& packet)
    {
        ar(packet.texture);
    }

}
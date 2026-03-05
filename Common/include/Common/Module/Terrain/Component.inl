// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

namespace Mcc
{

    template<typename Archive>
    void serialize(Archive& ar, CBlockMeta& packet)
    {
        ar(packet.id);
    }

}
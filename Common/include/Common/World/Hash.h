// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_WORLD_HASH_H
#define MCC_COMMON_WORLD_HASH_H

#include "Common/World/Position.h"

namespace std
{

    template<>
    struct hash<Mcc::ChunkPosV>
    {
        inline size_t operator()(const Mcc::ChunkPosV& v) const noexcept
        {
            return std::hash<glm::ivec2>()(v);
        }
    };

    template<>
    struct hash<Mcc::LocalPosV>
    {
        inline size_t operator()(const Mcc::LocalPosV& v) const noexcept
        {
            return std::hash<glm::uvec3>()(v);
        }
    };

    template<>
    struct hash<Mcc::LocalPosE>
    {
        inline size_t operator()(const Mcc::LocalPosE& v) const noexcept
        {
            return std::hash<glm::vec3>()(v);
        }
    };

    template<>
    struct hash<Mcc::WorldPosV>
    {
        inline size_t operator()(const Mcc::WorldPosV& v) const noexcept
        {
            auto [ c, i ] = v;
            return std::hash<decltype(c)>{}(c) ^ std::hash<decltype(i)>{}(i) << 1;
        }
    };

    template<>
    struct hash<Mcc::WorldPosE>
    {
        inline size_t operator()(const Mcc::WorldPosE& v) const noexcept
        {
            auto [ c, i ] = v;
            return std::hash<decltype(c)>{}(c) ^ std::hash<decltype(i)>{}(i) << 1;
        }
    };

    template<>
    struct hash<Mcc::WorldPosF>
    {
        inline size_t operator()(const Mcc::WorldPosF& v) const noexcept
        {
            return std::hash<glm::vec3>()(v);
        }
    };

}

#endif

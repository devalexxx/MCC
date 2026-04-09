// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Utils/ChunkHelper.h"

#include "Common/World/Chunk.h"

namespace Mcc::Helper
{

    bool IsInCircle(const glm::vec2& c, const glm::vec2& p, const long radius)
    {
        const auto fR = static_cast<float>(radius);
        return (p.x - c.x) * (p.x - c.x) + (p.y - c.y) * (p.y - c.y) <= fR * fR;
    }

    void ForInCircle(const long x, const long y, const int radius, std::function<void(long x, long y)>&& fn)
    {
        for (long i = x - radius; i < x + radius; ++i)
        {
            const auto ySpan = std::lround(radius * std::sin(std::acos((x - i) / radius)));
            for (long j = y - ySpan; j < y + ySpan; ++j) { fn(i, j); }
        }
    }

}
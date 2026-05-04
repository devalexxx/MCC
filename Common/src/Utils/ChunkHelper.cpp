// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Utils/ChunkHelper.h"

namespace Mcc::Helper
{

    bool IsInCircle(const glm::ivec2& c, const glm::ivec2& p, const long radius)
    {
        const auto r2 = radius * radius;
        const auto dx = p.x - c.x;
        const auto dy = p.y - c.y;
        return dx * dx + dy + dy <= r2;
    }

    bool IsInSphere(const glm::ivec3& c, const glm::ivec3& p, const long radius)
    {
        const int r2 = radius * radius;
        const int dx = p.x - c.x;
        const int dy = p.y - c.y;
        const int dz = p.z - c.z;
        return dx * dx + dy * dy + dz * dz <= r2;
    }

    void ForInCircle(const long x, const long y, const int radius, std::function<void(long x, long y)>&& fn)
    {
        for (long i = x - radius; i < x + radius; ++i)
        {
            const auto ySpan = std::lround(radius * std::sin(std::acos((x - i) / radius)));
            for (long j = y - ySpan; j < y + ySpan; ++j) { fn(i, j); }
        }
    }

    void ForInSphere(const long x, const long y, const long z, const int radius, std::function<void(long x, long y, long z)>&& fn)
    {
        const long r2 = radius * radius;

        for (long i = x - radius; i <= x + radius; ++i)
        {
            const long dx = i - x;
            const long dx2 = dx * dx;

            for (long j = y - radius; j <= y + radius; ++j)
            {
                const long dy = j - y;
                const long dy2 = dy * dy;

                const long remaining = r2 - dx2 - dy2;
                if (remaining < 0)
                    continue;

                const long zSpan = std::sqrt(remaining);
                for (long k = z - zSpan; k <= z + zSpan; ++k)
                {
                    fn(i, j, k);
                }
            }
        }
    }


}
// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/EntityReplication/System.h"

#include "Common/Utils/MathUtils.h"
#include "Common/World/Geometry.h"
#include "Common/SceneImporter.h"
#include "Common/WorldContext.h"

namespace Mcc
{

    void EntityInterpolationSystem(CEntityTransform& transform, CSnapshotQueue& queue)
    {
        constexpr auto delay      = std::chrono::milliseconds(50);
        const auto     delayedNow = TimeClock::now() - delay;

        // Cleanup old snapshot
        while (!queue.empty() && queue.back().time < delayedNow - delay) { queue.pop_back(); }

        if (queue.empty())
        {
            return;
        }

        if (queue.size() == 1)
        {
            transform = queue.front().transform;
            return;
        }

        for (size_t i = 0; i < queue.size() - 1; ++i)
        {
            if (queue[i].time >= delayedNow && queue[i + 1].time <= delayedNow)
            {
                const auto& [aTr, aTime] = queue[i];
                const auto& [bTr, bTime] = queue[i + 1];

                const auto elapsed = std::chrono::duration<float>(delayedNow - aTime).count();
                const auto length  = std::chrono::duration<float>(bTime      - aTime).count();

                transform.position = Lerp (aTr.position, bTr.position, elapsed / length);
                transform.rotation = SLerp(aTr.rotation, bTr.rotation, elapsed / length);
                transform.scale    = aTr.scale;
                break;
            }
        }
    }

}
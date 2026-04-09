// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_ENTITY_REPLICATION_COMPONENT_H
#define MCC_CLIENT_MODULE_ENTITY_REPLICATION_COMPONENT_H

#include "Common/Module/Entity/Component.h"

#include <chrono>

namespace Mcc
{

    struct TInterpolationExcluded {};

    using TimeClock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<TimeClock>;

    struct Snapshot
    {
        CEntityTransform transform {};
        TimePoint        time;
    };

    namespace _ { struct EntityReplicationModuleTag {}; };
    using CSnapshotQueue = ComponentWrapper<std::deque<Snapshot>, _::EntityReplicationModuleTag>;

}

#endif

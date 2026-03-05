// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_ENTITY_REPLICATION_SYSTEM_H
#define MCC_CLIENT_MODULE_ENTITY_REPLICATION_SYSTEM_H

#include "Client/Module/EntityReplication/Component.h"

#include "Common/Module/Entity/Component.h"

namespace Mcc
{

    void EntityInterpolationSystem(CTransform& transform, CSnapshotQueue& queue);

}

#endif

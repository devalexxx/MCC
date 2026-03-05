// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_TERRAIN_COMPONENT_H
#define MCC_SERVER_MODULE_TERRAIN_COMPONENT_H

namespace Mcc
{

    struct TChunkDirty     {};
    struct TChunkCreated   {};
    struct TChunkDestroyed {};

    struct TBlockDirty     {};
    struct TBlockCreated   {};
    struct TBlockDestroyed {};

}

#endif
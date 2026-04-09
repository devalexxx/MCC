// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_WORLD_COORDINATE_H
#define MCC_COMMON_WORLD_COORDINATE_H

namespace Mcc
{

    struct WorldSpace {};
    struct ChunkSpace {};
    struct LocalSpace {};

    struct FloatCoord {}; // 1 == 1 voxel
    struct VoxelCoord {}; // 0 -> Chunk::Size or Chunk::Height
    struct EnttyCoord {}; // 0 -> 1 for x and z. y not constrained but 1 == Chunk::Height

}

#endif

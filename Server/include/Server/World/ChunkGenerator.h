// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_WORLD_CHUNK_GENERATOR_H
#define MCC_SERVER_WORLD_CHUNK_GENERATOR_H

#include "Common/World/Chunk.h"

#include "PerlinNoise.hpp"

#include <unordered_map>

namespace Mcc
{

    class ChunkGenerator
    {
      public:
        explicit ChunkGenerator(siv::PerlinNoise::seed_type seed);

        void                   Setup    (const flecs::world& world);
        std::shared_ptr<Chunk> Generate(glm::ivec3 position) const;

      private:
        std::unordered_map<std::string, flecs::entity> mPalette;
        siv::PerlinNoise                               mPerlin;
    };

}

#endif

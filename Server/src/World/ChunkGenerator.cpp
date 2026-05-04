// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/World/ChunkGenerator.h"

#include "Common/Utils/Logging.h"

#include <utility>

namespace Mcc
{

    ChunkGenerator::ChunkGenerator(const siv::PerlinNoise::seed_type seed) : mPerlin(seed)
    {}

    void ChunkGenerator::Setup(const flecs::world& world)
    {
        mPalette.emplace("air"  , world.lookup("mcc:block:air"));
        mPalette.emplace("stone", world.lookup("mcc:block:stone"));
        mPalette.emplace("dirt" , world.lookup("mcc:block:dirt"));
        mPalette.emplace("grass", world.lookup("mcc:block:grass"));
    }

    std::shared_ptr<Chunk> ChunkGenerator::Generate(const glm::ivec3 position) const
    {
        auto chunk = std::make_shared<Chunk>(mPalette.find("air")->second);

        const auto stone = mPalette.find("stone")->second;
        for (int x = 0; std::cmp_less(x, Chunk::Size); ++x)
        {
            for (int z = 0; std::cmp_less(z, Chunk::Size); ++z)
            {
                constexpr auto f      = .01;
                constexpr auto oct    = 2;
                constexpr auto amp    = 4;
                constexpr auto surf   = .5;
                const auto     dx     = static_cast<double>(position.x * Chunk::Size + x);
                const auto     dz     = static_cast<double>(position.z * Chunk::Size + z);
                const double   noise  = surf + mPerlin.octave2D_01(dx * f, dz * f, oct) * amp;
                const auto     height = noise * Chunk::Size;

                for (int y = 0; std::cmp_less(y, Chunk::Size); ++y)
                {
                    if (position.y * Chunk::Size + y < height)
                        chunk->Set({ x, y, z }, stone);
                }
            }
        }

        return std::move(chunk);
    }

}
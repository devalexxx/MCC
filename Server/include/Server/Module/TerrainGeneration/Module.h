// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_TERRAIN_GENERATION_MODULE_H
#define MCC_SERVER_MODULE_TERRAIN_GENERATION_MODULE_H

#include "Server/World/ChunkGenerator.h"

#include "Common/Module/Base/Module.h"
#include "Common/Module/Terrain/Module.h"

#include <flecs.h>

namespace Mcc
{

    class TerrainGenerationModule final : public BaseModule<TerrainGenerationModule, TerrainModule>
    {
      public:
        TerrainGenerationModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterSystem(flecs::world& world) override;
        void RegisterHandler(flecs::world& world) override;

        void          InitializeGenerator(const flecs::world& world);
        flecs::entity LaunchGenerationTask(const flecs::world& world, const glm::ivec3& position) const;

      private:
        ChunkGenerator mGenerator;
    };

}

#endif

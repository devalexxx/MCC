// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/EntityRenderer/Module.h"

#include "Client/Module/EntityRenderer/System.h"
#include "Client/Module/Renderer/Module.h"
#include "Client/Scene/Scene.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Phase.h"

namespace Mcc
{

    EntityRendererModule::EntityRendererModule(flecs::world& world) :
        BaseModule(world),
        program(std::make_shared<OpenGLProgram>()),
        texture(std::make_shared<OpenGLTexture2D>()),
        programEntity(flecs::entity::null()),
        textureEntity(flecs::entity::null())
    {}

    void EntityRendererModule::RegisterComponent(flecs::world& /* world */) {}

    void EntityRendererModule::RegisterPrefab(flecs::world& /* world */) {}

    void EntityRendererModule::RegisterSystem(flecs::world& world)
    {
        world.system("SetupEntityMesh")
            .kind<Phase::OnLoad>()
            .run(SetupEntityMeshSystem);

        world.system<const CEntityTransform>("UpdateRenderTransform")
            .kind<Phase::PostUpdate>()
            .each(UpdateRenderTransformSystem);
    }

    void EntityRendererModule::RegisterObserver(flecs::world& world)
    {
        world.observer<TEntity>()
            .event(flecs::OnAdd)
            .run(SetEntityMeshObserver);
    }

}
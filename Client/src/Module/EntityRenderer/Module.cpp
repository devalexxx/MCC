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
        vertexBuffer(GL_ARRAY_BUFFER),
        indexBuffer(GL_ELEMENT_ARRAY_BUFFER),
        indexCount(0)
    {}

    void EntityRendererModule::RegisterComponent(flecs::world& /* world */) {}

    void EntityRendererModule::RegisterPrefab(flecs::world& /* world */) {}

    void EntityRendererModule::RegisterSystem(flecs::world& world)
    {
        world.system("SetupEntityMesh")
            .kind<Phase::OnLoad>()
            .run(SetupEntityMeshSystem);

        world.system<const CTransform>("RenderUserEntity")
            .kind<Phase::OnDraw>()
            .with<TNetworkEntity>()
            .run(RenderUserEntitySystem)
            .add<GameScene>();
    }

    void EntityRendererModule::RegisterObserver(flecs::world& /* world */) {}

}
// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/Renderer/Module.h"

#include "Client/Module/Camera/Component.h"
#include "Client/Module/Camera/Module.h"
#include "Client/Module/Renderer/Component.h"
#include "Client/Module/Renderer/System.h"
#include "Client/WorldContext.h"

#include "Common/World/Geometry.h"
#include "Common/World/Transform.h"
#include "Common/Phase.h"

namespace Mcc
{

    RendererModule::RendererModule(flecs::world& world) : BaseModule(world)
    {}

    void RendererModule::RegisterComponent(flecs::world& world)
    {
        world.component<TRenderable>();

        world.component<CRendererSettings>()
            .add(flecs::Singleton);

        world.component<ROpenGLMesh>()
            .add(flecs::Exclusive)
            .add(flecs::Relationship)
            .add(flecs::DontFragment);

        world.component<ROpenGLProgram>()
            .add(flecs::Exclusive)
            .add(flecs::Relationship);

        world.component<ROpenGLTexture>()
            .add(flecs::Exclusive)
            .add(flecs::Relationship);

        world.component<COpenGLMesh>();

        world.component<WorldPosF>("WorldPosF")
            .opaque(
                world.component()
                    .member<float>("x")
                    .member<float>("y")
                    .member<float>("z")
            )
            .serialize([](const flecs::serializer* s, const WorldPosF* data)
            {
                const auto v = glm::vec3(*data);
                s->member("x");
                s->value(v.x);
                s->member("y");
                s->value(v.y);
                s->member("z");
                s->value(v.z);
                return 0;
            })
            .ensure_member([](WorldPosF*, const char* member) -> void*
            {
                static glm::vec3 fake1;

                const auto str = std::string_view(member);
                if (str == "x") return &fake1.x;
                if (str == "y") return &fake1.y;
                if (str == "z") return &fake1.z;

                return nullptr;
            });

        world.component<CRenderTransform>("CRenderTransform")
            .member("position", &CRenderTransform::position)
            .member("rotation", &CRenderTransform::rotation)
            .member("scale"   , &CRenderTransform::scale);

        AutoRegister<COpenGLProgram>::Register(world, "COpenGLProgram");
        AutoRegister<COpenGLTexture>::Register(world, "COpenGLTexture");

        world.component<CRenderQueue>("CRenderQueue")
            .add(flecs::Singleton);
    }

    void RendererModule::RegisterPrefab(flecs::world& /* world */) {}

    void RendererModule::RegisterSystem(flecs::world& world)
    {
        world.system("SetupRenderer")
            .kind<Phase::OnLoad>()
            .run(SetupRendererSystem);

        world.system("PollWindowEvent")
            .kind<Phase::OnPoll>()
            .run(PollWindowEventSystem);

        world.system("ClearFrame")
            .kind<Phase::OnClear>()
            .run(ClearFrameSystem);

        // TODO: add PreDraw phase
        world.system("UpdateRenderQueue")
            .kind<Phase::OnClear>()
            .with<ROpenGLProgram>(flecs::Wildcard)
            .with<ROpenGLTexture>(flecs::Wildcard)
            .with<ROpenGLMesh>   (flecs::Wildcard)
            .with<CRenderTransform>()
            .with<TRenderable>()
            .run(UpdateRenderQueueSystem);

        world.system<const CRenderQueue, const CRendererSettings>("DrawFrame")
            .kind<Phase::OnDraw>()
            .each(DrawFrameSystem);

        world.system("RenderFrame")
            .kind<Phase::OnRender>()
            .run(RenderFrameSystem);
    }

    void RendererModule::RegisterObserver(flecs::world& /* world */) {}

    std::tuple<glm::vec3, glm::mat4, glm::mat4> RendererModule::GetView(const flecs::world& world)
    {
        const auto* ctx    = ClientWorldContext::Get(world);
        const auto  camera = CameraModule::GetActiveCamera(world);
        if (!camera.is_valid())
            return {{}, {}, {}};

        auto& transform = camera.get<CEntityTransform>();
        auto& settings  = camera.get<CCameraSettings>();

        WorldPosF position = transform.position;

        const glm::vec3 eye    = position;
        const glm::vec3 center = position + TranslationF(transform.rotation * glm::forward);
        const glm::vec3 up     = glm::normalize(glm::cross(transform.rotation * glm::right, transform.rotation * glm::forward));
        const glm::mat4 view   = glm::lookAt(eye, center, up);
        const glm::mat4 proj   = glm::perspective(settings.fov, ctx->window.GetAspectRatio(), settings.zNear, settings.zFar);

        return { position, view, proj };
    }


}
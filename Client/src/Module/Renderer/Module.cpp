// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/Renderer/Module.h"

#include "Client/Module/Camera/Component.h"
#include "Client/Module/Renderer/Component.h"
#include "Client/Module/Renderer/System.h"
#include "Client/WorldContext.h"

#include "Common/Module/Core/Component.h"
#include "Common/Module/Entity/Component.h"
#include "Common/Phase.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    RendererModule::RendererModule(flecs::world& world) : BaseModule(world)
    {}

    void RendererModule::RegisterComponent(flecs::world& world)
    {
        world.component<CRendererSettings>()
            .add(flecs::Singleton);

        world.component<ROpenGLMesh>()
            .add(flecs::Exclusive)
            .add(flecs::Relationship);

        world.component<ROpenGLProgram>()
            .add(flecs::Exclusive)
            .add(flecs::Relationship);

        world.component<ROpenGLTexture>()
            .add(flecs::Exclusive)
            .add(flecs::Relationship);

        world.component<COpenGLMesh>();
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
            .kind<Phase::OnSetup>()
            .run(PollWindowEventSystem);

        world.system("ClearFrame")
            .kind<Phase::OnClear>()
            .run(ClearFrameSystem);

        // TODO: add PreDraw phase
        world.system("UpdateRenderQueue")
            .kind<Phase::OnClear>()
            .with<CTransform>()
            .with<ROpenGLProgram>(flecs::Wildcard)
            .with<ROpenGLTexture>(flecs::Wildcard)
            .with<ROpenGLMesh>   (flecs::Wildcard)
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
        const auto* ctx = ClientWorldContext::Get(world);

        CTransform      cTransform {};
        CCameraSettings cSettings {};
        world.query_builder<const CTransform, const CCameraSettings>()
            .with<TActiveCamera>()
            .build()
            .run([&](flecs::iter& it) {
                bool isSet = false;
                while (it.next())
                {
                    if (!isSet)
                    {
                        auto t = it.field<const CTransform>(0);
                        auto s = it.field<const CCameraSettings>(1);

                        if (it.count() > 1)
                            MCC_LOG_WARN("More than one camera active");

                        if (it.count() <= 0)
                        {
                            MCC_LOG_ERROR("No active camera");
                        }
                        else
                        {
                            cTransform = t[0];
                            cSettings  = s[0];
                            isSet      = true;
                        }
                    }
                }
            });

        const glm::vec3 up   = glm::normalize(glm::cross(cTransform.rotation * glm::right, cTransform.rotation * glm::forward));
        const glm::mat4 view = glm::lookAt(cTransform.position, cTransform.position + cTransform.rotation * glm::forward, up);
        const glm::mat4 proj = glm::perspective(cSettings.fov, ctx->window.GetAspectRatio(), cSettings.zNear, cSettings.zFar);

        return { cTransform.position, view, proj };
    }


}
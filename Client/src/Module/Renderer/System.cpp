// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/Renderer/System.h"

#include "Client/Graphics/OpenGL/OpenGLCommon.h"
#include "Client/Module/Renderer/Component.h"
#include "Client/Module/Renderer/Module.h"
#include "Client/WorldContext.h"

#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp>

namespace Mcc
{

    void SetupRendererSystem(flecs::iter& it)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        it.world().set<CRenderQueue>({});
        it.world().set<CRendererSettings>({
            .wireframe = false
        });

        IgnoreIter(it);
    }

    void PollWindowEventSystem(flecs::iter& it)
    {
        Window::PollEvents();

        IgnoreIter(it);
    }

    void ClearFrameSystem(flecs::iter& it)
    {
        glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        glCheck(glClearColor(1., 1., 1., 1.));

        IgnoreIter(it);
    }

    void RenderFrameSystem(flecs::iter& it)
    {
        ClientWorldContext::Get(it.world())->window.SwapBuffer();

        IgnoreIter(it);
    }

    void UpdateRenderQueueSystem(flecs::iter& it)
    {
        auto& queue = it.world().get_mut<CRenderQueue>();

        queue.clear();
        while (it.next())
        {
            const auto pEntity = it.pair(1).second();
            const auto tEntity = it.pair(2).second();
            const auto mEntity = it.pair(3).second();

            for (const auto i: it)
            {
                const auto entity  = it.entity(i);
                auto [pIt, pInserted] = queue      .try_emplace(pEntity.id(), CRenderQueue::mapped_type {});
                auto [tIt, tInserted] = pIt->second.try_emplace(tEntity.id(), CRenderQueue::mapped_type::mapped_type {});
                auto [mIt, mInserted] = tIt->second.try_emplace(mEntity.id(), std::unordered_set { entity.id() });
                if (!mInserted)
                    mIt->second.emplace(entity.id());
            }

        }
    }

    void DrawFrameSystem(const flecs::iter& it, size_t /* row */, const CRenderQueue& queue, const CRendererSettings& settings)
    {
        static auto defaultRenderFunc = [](const size_t size) {
            glCheck(glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr));
        };
        static auto wireframeRenderFunc = [](const size_t size) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_CULL_FACE);
            glCheck(glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr));
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        };

        std::function renderFunc = defaultRenderFunc;
        if (settings.wireframe)
            renderFunc = wireframeRenderFunc;

        const auto world = it.world();
        for (auto& [pEntity, tMap] : queue)
        {
            const auto program = world.entity(pEntity).get<COpenGLProgram>();
            program->Bind();

            const auto&& [vPos, view, proj] = RendererModule::GetView(it.world());
            program->SetUniformMatrix(program->GetUniformLocation("view"), view);
            program->SetUniformMatrix(program->GetUniformLocation("proj"), proj);
            program->SetUniformVector(program->GetUniformLocation("vPos"), vPos);

            const auto modelLocation    = program->GetUniformLocation("model");
            const auto invModelLocation = program->GetUniformLocation("invModel");

            for (auto& [tEntity, mMap]: tMap)
            {
                const auto texture = world.entity(tEntity).get<COpenGLTexture>();
                OpenGLTexture::SetActiveTexture(0);
                texture->Bind();
                for (auto& [mEntity, tVec]: mMap)
                {
                    const auto& mesh = world.entity(mEntity).get<COpenGLMesh>();
                    mesh.vertexArray.Bind();
                    mesh.indexBuffer.Bind();
                    for (auto& trEntity : tVec)
                    {
                        const auto& [position, rotation, scale] = world.entity(trEntity).get<CTransform>();
                        const auto model =  glm::translate(
                            glm::mat4(1.), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.), scale
                        );
                        program->SetUniformMatrix(modelLocation   , model);
                        program->SetUniformMatrix(invModelLocation, glm::transpose(glm::inverse(glm::mat3(model))));

                        renderFunc(mesh.indexCount);
                    }
                }
            }
        }
    }

}
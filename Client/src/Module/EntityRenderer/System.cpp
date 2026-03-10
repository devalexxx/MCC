// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/EntityRenderer/System.h"

#include "Client/Graphics/Mesh.h"
#include "Client/Graphics/OpenGL/OpenGLShader.h"
#include "Client/Module/EntityRenderer/Module.h"
#include "Client/Module/Renderer/Module.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Utils/FlecsUtils.h"

#include <glm/gtx/quaternion.hpp>

namespace Mcc
{

    void SetupEntityMeshSystem(flecs::iter& it)
    {
        auto& module = it.world().get_mut<EntityRendererModule>();

        OpenGLShader vertexShader(GL_VERTEX_SHADER, R"""(
			#version 330

			in vec3 inVertex;

			uniform mat4 view;
			uniform mat4 proj;
			uniform mat4 model;
			uniform vec3 color;

			out vec3 passColor;

			void main() {
				gl_Position = proj * view * model * vec4(inVertex, 1.0);
				passColor = color;
			}
		)""");
        OpenGLShader fragmentShader(GL_FRAGMENT_SHADER, R"""(
			#version 330

			in vec3 passColor;

			out vec4 fragment;

			void main() {
				fragment = vec4(passColor, 1.0);
			}
		)""");

        vertexShader  .Create();
        fragmentShader.Create();

        module.program.Create();
        module.program.Attach(vertexShader);
        module.program.Attach(fragmentShader);

        module.program.Link();

        module.program.Detach(vertexShader);
        module.program.Detach(fragmentShader);

        vertexShader  .Delete();
        fragmentShader.Delete();

        module.vertexArray.Create();
        module.vertexArray.Bind();

        auto&& [vertex, index] = Helper::GenerateCapsuleMesh(.5f, 32, 16);
        module.vertexBuffer.Create();
        module.vertexBuffer.SetData(std::span(vertex), GL_STATIC_DRAW);
        module.program.SetVertexAttribPointer("inVertex", 3, GL_FLOAT, sizeof(PackedVertex), 0);

        module.indexBuffer.Create();
        module.indexBuffer.SetData(std::span(index), GL_STATIC_DRAW);

        module.indexCount = index.size();

        IgnoreIter(it);
    }

    void RenderUserEntitySystem(flecs::iter& it)
    {
        static std::unordered_map<flecs::entity_t, glm::vec3> colorMapper {};

        auto& module = it.world().get_mut<EntityRendererModule>();

        const auto modelLocation = module.program.GetUniformLocation("model");
        const auto colorLocation = module.program.GetUniformLocation("color");

        module.program.Bind();

        const auto&& [_, view, proj] = RendererModule::GetView(it.world());
        module.program.SetUniformMatrix(module.program.GetUniformLocation("view"), view);
        module.program.SetUniformMatrix(module.program.GetUniformLocation("proj"), proj);

        module.vertexArray.Bind();
        module.indexBuffer.Bind();

        while (it.next())
        {
            auto t = it.field<const CTransform>(0);

            for (const auto i: it)
            {
                auto entity = it.entity(i).id();
                if (const auto colorIt = colorMapper.find(entity); colorIt == colorMapper.cend())
                {
                    auto color = glm::vec3(
                        static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX,
                        static_cast<float>(std::rand()) / RAND_MAX
                    );
                    colorMapper[entity] = color;
                    module.program.SetUniformVector(colorLocation, color);
                }
                else
                {
                    module.program.SetUniformVector(colorLocation, colorIt->second);
                }

                module.program.SetUniformMatrix(
                    modelLocation, glm::translate(glm::mat4(1.f), t[i].position) * glm::toMat4(t[i].rotation) *
                                       glm::scale(glm::mat4(1.f), t[i].scale)
                );

                // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                // glDisable(GL_CULL_FACE);
                glCheck(glDrawElements(GL_TRIANGLES, module.indexCount, GL_UNSIGNED_INT, nullptr));
                // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
    }

}
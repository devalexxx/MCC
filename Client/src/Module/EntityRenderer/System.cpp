// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Graphics/Mesh.h"
#include "Client/Graphics/Shader.h"
#include "Client/Module/EntityRenderer/Module.h"
#include "Client/Module/Renderer/Module.h"

#include "Common/Module/Entity/Component.h"

#include <glm/gtx/quaternion.hpp>

#include <numeric>

namespace Mcc
{

    void EntityRendererModule::SetupEntityMeshSystem(flecs::iter& it)
    {
        while (it.next()) {}

        const Shader vertexShader(GL_VERTEX_SHADER, R"""(
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
        const Shader fragmentShader(GL_FRAGMENT_SHADER, R"""(
			#version 330

			in vec3 passColor;

			out vec4 fragment;

			void main() {
				fragment = vec4(passColor, 1.0);
			}
		)""");

        mProgram.Create();
        mProgram.Attach(vertexShader);
        mProgram.Attach(fragmentShader);

        mProgram.Link();

        mProgram.Detach(vertexShader);
        mProgram.Detach(fragmentShader);

        mVertexArray.Create();
        mVertexArray.Bind();

        auto&& [vertex, index] = Helper::GenerateCapsuleMesh(.5f, 32, 16);
        mVertexBuffer.Create();
        mVertexBuffer.SetData(std::span(vertex), GL_STATIC_DRAW);
        mProgram.SetVertexAttribPointer("inVertex", 3, GL_FLOAT, sizeof(PackedVertex), 0);

        mIndexBuffer.Create();
        mIndexBuffer.SetData(std::span(index), GL_STATIC_DRAW);

        mIndexCount = index.size();
    }

    void EntityRendererModule::RenderUserEntitySystem(flecs::iter& it)
    {
        static std::unordered_map<flecs::entity_t, glm::vec3> colorMapper {};

        const auto modelLocation = mProgram.GetUniformLocation("model");
        const auto colorLocation = mProgram.GetUniformLocation("color");

        mProgram.Use();

        const auto&& [_, view, proj] = RendererModule::GetView(it.world());
        mProgram.SetUniformMatrix(mProgram.GetUniformLocation("view"), view);
        mProgram.SetUniformMatrix(mProgram.GetUniformLocation("proj"), proj);

        mVertexArray.Bind();
        mIndexBuffer.Bind();

        while (it.next())
        {
            auto t = it.field<const Transform>(0);

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
                    mProgram.SetUniformVector(colorLocation, color);
                }
                else
                {
                    mProgram.SetUniformVector(colorLocation, colorIt->second);
                }

                mProgram.SetUniformMatrix(
                    modelLocation, glm::translate(glm::mat4(1.f), t[i].position) * glm::toMat4(t[i].rotation) *
                                       glm::scale(glm::mat4(1.f), t[i].scale)
                );

                // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                // glDisable(GL_CULL_FACE);
                glCheck(glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, nullptr));
                // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
    }

}
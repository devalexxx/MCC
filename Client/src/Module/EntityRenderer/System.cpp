// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/EntityRenderer/System.h"

#include "Client/Graphics/Mesh.h"
#include "Client/Graphics/OpenGL/OpenGLShader.h"
#include "Client/Module/EntityRenderer/Module.h"
#include "Client/Module/Renderer/Component.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Utils/FlecsUtils.h"

namespace Mcc
{

    void SetupEntityMeshSystem(flecs::iter& it)
    {
        OpenGLShader vertexShader(GL_VERTEX_SHADER, R"""(
			#version 330

			in vec3 inVertex;
            in vec3 inTexCoord;

			uniform mat4 view;
			uniform mat4 proj;
			uniform mat4 model;

			out vec3 passColor;
            out vec3 passTexCoord;

			void main() {
				gl_Position  = proj * view * model * vec4(inVertex, 1.0);
				passColor    = vec3(1.0, 0.0, 0.0);
                passTexCoord = inTexCoord;
			}
		)""");
        OpenGLShader fragmentShader(GL_FRAGMENT_SHADER, R"""(
			#version 330
            #extension GL_ARB_texture_query_lod : enable

			in vec3 passColor;
            in vec3 passTexCoord;

            uniform mat3      invModel;
            uniform vec3      vPos;
            uniform sampler2D tex;

			out vec4 fragment;

			void main() {
                float mipmapLevel = 0;
            #ifdef GL_ARB_texture_query_lod
                mipmapLevel = textureQueryLOD(tex, passTexCoord.xy).x;
            #endif
                vec4 pixel = textureLod(tex, passTexCoord.xy, mipmapLevel);
				fragment   = vec4(passColor, 1.0) * pixel;
			}
		)""");

        const auto world  = it.world();
        auto&      module = it.world().get_mut<EntityRendererModule>();

        vertexShader  .Create();
        fragmentShader.Create();

        module.program->Create();
        module.program->Attach(vertexShader);
        module.program->Attach(fragmentShader);

        module.program->Link();

        module.program->Detach(vertexShader);
        module.program->Detach(fragmentShader);

        vertexShader  .Delete();
        fragmentShader.Delete();

        module.programEntity = world.entity().set<COpenGLProgram>(module.program);

        const Image image(128, 128, 3, PixelFormat::RGB, 255);
        module.texture->Create();
        module.texture->SetData(image);
        module.texture->GenerateMipmap();

        module.textureEntity = world.entity().set<COpenGLTexture>(module.texture);

        COpenGLMesh mesh;

        mesh.vertexArray.Create();
        mesh.vertexArray.Bind();

        auto&& [vertex, index] = Helper::GenerateCapsuleMesh(.5f, 32, 16);
        mesh.vertexBuffer.Create();
        mesh.vertexBuffer.SetData(std::span(vertex), GL_STATIC_DRAW);

        module.program->SetVertexAttribPointer("inVertex"  , 3, GL_FLOAT, sizeof(PackedVertex), 0);
        module.program->SetVertexAttribPointer("inTexCoord", 3, GL_FLOAT, sizeof(PackedVertex), 6 * sizeof(float));

        mesh.indexBuffer.Create();
        mesh.indexBuffer.SetData(std::span(index), GL_STATIC_DRAW);

        mesh.indexCount = index.size();

        module.capsuleEntity = world.entity().set<COpenGLMesh>(std::move(mesh));

        IgnoreIter(it);
    }

    void SetEntityMeshObserver(flecs::iter& it)
    {
        const auto module = it.world().get_mut<EntityRendererModule>();

        while (it.next())
        {
            for (const auto i: it)
            {
                auto entity = it.entity(i);
                entity.add<ROpenGLProgram>(module.programEntity);
                entity.add<ROpenGLTexture>(module.textureEntity);
                entity.add<ROpenGLMesh>   (module.capsuleEntity);
            }
        }
    }

}
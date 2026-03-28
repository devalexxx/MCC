// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/EntityRenderer/System.h"

#include "Client/Graphics/Mesh.h"
#include "Client/Graphics/OpenGL/OpenGLShader.h"
#include "Client/Module/EntityRenderer/Module.h"
#include "Client/Module/Renderer/Component.h"
#include "Client/WorldContext.h"

#include "Common/Utils/FlecsUtils.h"

namespace Mcc
{

    void SetupEntityMeshSystem(flecs::iter& it)
    {
        const auto  world  = it.world();
        const auto* ctx    = ClientWorldContext::Get(world);
        auto&       module = it.world().get_mut<EntityRendererModule>();

        const auto vertexShader   = ctx->assetRegistry.Get<OpenGLShader>("shader://entity.vert", false);
        const auto fragmentShader = ctx->assetRegistry.Get<OpenGLShader>("shader://entity.frag", false);

        module.program->Create();
        module.program->Attach(*vertexShader.get());
        module.program->Attach(*fragmentShader.get());

        module.program->Link();

        module.program->Detach(*vertexShader.get());
        module.program->Detach(*fragmentShader.get());

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
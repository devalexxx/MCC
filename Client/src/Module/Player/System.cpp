// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/Player/System.h"

#include "Client/ClientNetworkManager.h"
#include "Client/Graphics/Mesh.h"
#include "Client/Graphics/OpenGL/OpenGLProgram.h"
#include "Client/Graphics/OpenGL/OpenGLShader.h"
#include "Client/Graphics/OpenGL/OpenGLTexture2D.h"
#include "Client/Module/Camera/Component.h"
#include "Client/Module/Camera/Module.h"
#include "Client/Module/EntityReplication/Component.h"
#include "Client/Module/Player/Component.h"
#include "Client/Module/Player/Module.h"
#include "Client/Module/Renderer/Component.h"
#include "Client/WorldContext.h"

#include "Common/Utils/BlockUtils.h"
#include "Common/Utils/Raycast.h"

#include <Hexis/Core/VariantUtils.h>

#include <imgui.h>

namespace Mcc
{

    void SetupFacingBlockSystem(flecs::iter& it)
    {
        const auto fEntityPtr = it.ctx<flecs::entity>();
        const auto world      = it.world();
        const auto ctx        = ClientWorldContext::Get(world);

        *fEntityPtr = world.entity();
        const auto fEntity = *fEntityPtr;

        const auto vert = ctx->assetRegistry.Get<OpenGLShader>("shader://outline.vert", false);
        const auto frag = ctx->assetRegistry.Get<OpenGLShader>("shader://outline.frag", false);

        const auto prog = ctx->assetRegistry.Add<OpenGLProgram>("rt://debugProgram");
        prog->Create();
        prog->Attach(*vert.get());
        prog->Attach(*frag.get());
        prog->Link();
        prog->Detach(*vert.get());
        prog->Detach(*frag.get());

        const auto pEntity = world.entity().set<COpenGLProgram>(prog);
        fEntity.add<ROpenGLProgram>(pEntity);

        const auto image   = Image(32, 32, 3, PixelFormat::RGBA, 255);
        const auto texture = std::make_shared<OpenGLTexture2D>();
        texture->Create();
        texture->SetData(image);

        const auto tEntity = world.entity().set<COpenGLTexture>(texture);
        fEntity.add<ROpenGLTexture>(tEntity);

        const auto [vertex, index] = Helper::GenerateCubeMesh();

        COpenGLMesh rMesh;
        rMesh.vertexArray.Create();
        rMesh.vertexArray.Bind();

        rMesh.vertexBuffer.Create();
        rMesh.vertexBuffer.SetData(std::span(vertex), GL_STATIC_DRAW);
        prog->SetVertexAttribPointer("inVertex", 3, GL_FLOAT, sizeof(PackedVertex), 0);

        rMesh.indexBuffer.Create();
        rMesh.indexBuffer.SetData(std::span(index), GL_STATIC_DRAW);

        rMesh.indexCount = index.size();

        const auto mEntity = world.entity().emplace<COpenGLMesh>(std::move(rMesh));
        fEntity.add<ROpenGLMesh>(mEntity);

        IgnoreIter(it);
    }

    void
    ApplyAndSendPlayerInput(const flecs::iter& it, const size_t row, UserInput& current, CUserInputQueue& queue)
    {
        const auto* ctx = ClientWorldContext::Get(it.world());

        if (!Helper::IsNull(current))
        {
            const float speed = ctx->serverInfo.userSpeed;
            const float delta = it.delta_time();

            current.meta.id = UserInput::Meta::GetNextID();
            current.meta.dt = delta;
            ctx->networkManager.Send<OnPlayerInput>({ current }, ENET_PACKET_FLAG_RELIABLE, 0);

            const auto entity = it.entity(row);
            entity.get([&](CEntityTransform& transform)
            {
                Helper::ApplyXAxis(current, transform);
                Helper::ApplyMovement(current, transform, delta, speed);
            });

            it.world()
                .query_builder<CEntityTransform>()
                .with<RCameraFollow>(entity)
                .build()
                .each([&](CEntityTransform& transform)
                {
                    Helper::ApplyBothAxis(current, transform);
                });

            queue.push_back(current);
            current.axis = {};
        }
    }

    void SetFacingBlockSystem(flecs::iter& it, size_t /* row */, const CEntityTransform& transform)
    {
        const auto fEntity = *it.ctx<flecs::entity>();
        const auto world   = it.world();
        const auto camera  = CameraModule::GetActiveCamera(world);
        if (!camera.is_valid())
            return;

        const auto cameraTransform = camera.get<CEntityTransform>();
        const auto cameraDirection = glm::normalize(cameraTransform.rotation * glm::forward);

        if (auto result = Raycast(world, cameraTransform.position, cameraDirection, 10))
        {
            std::visit(
                Hx::Overloaded {
                    [&](const BlockHit& hit)
                    {
                        fEntity.add<TRenderable>();
                        fEntity.set<CFacingInfo>({
                            .previous = hit.previous, .position = hit.position, .distance = result->distance
                        });
                    },
                    [&](const EntityHit&)
                    {
                        MCC_LOG_DEBUG("Entity hit with a distance of {}", result->distance);
                    }
                },
                result->object
            );
            return;
        }

        fEntity.remove<CFacingInfo>();
        fEntity.remove<TRenderable>();
    }

    void DrawCrosshairSystem(flecs::iter& it)
    {
        const auto ctx = ClientWorldContext::Get(it.world());
        auto [x, y] = ctx->window.GetWindowSize();

        ImGui::Begin("#CH", nullptr, ImGuiWindowFlags_NoMove | ImGuiInputTextFlags_ReadOnly | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);
        const auto draw = ImGui::GetBackgroundDrawList();
        draw->AddCircle(ImVec2(x / 2, y / 2), 6 , IM_COL32(255, 0, 0, 255), 100, 0.0f);
        ImGui::End();

        IgnoreIter(it);
    }

}
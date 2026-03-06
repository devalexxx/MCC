// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/TerrainRenderer/System.h"

#include "Client/Graphics/Mesh.h"
#include "Client/Graphics/Program.h"
#include "Client/Graphics/Shader.h"
#include "Client/Module/EntityReplication/Component.h"
#include "Client/Module/Renderer/Module.h"
#include "Client/Module/TerrainRenderer/Component.h"
#include "Client/Module/TerrainRenderer/Module.h"
#include "Client/Scene/Scene.h"
#include "Client/WorldContext.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Terrain/Component.h"
#include "Common/SceneImporter.h"
#include "Common/Utils/Benchmark.h"
#include "Common/Utils/ChunkHelper.h"
#include "Common/WorldContext.h"

#include <Hexis/Core/EnumArray.h>
#include <glm/ext/matrix_transform.hpp>

#include <numeric>
#include <utility>

namespace Mcc
{

    namespace _
    {

        static Hx::EnumArray<BlockFace, std::array<glm::vec3, 6>> sQuadVertices {
            { { BlockFace::Front,
                { { { -1.f, 1.f, 1.f },
                    { 1.f, -1.f, 1.f },
                    { 1.f, 1.f, 1.f },
                    { -1.f, 1.f, 1.f },
                    { -1.f, -1.f, 1.f },
                    { 1.f, -1.f, 1.f } } } },
             { BlockFace::Back,
                { { { 1.f, 1.f, -1.f },
                    { -1.f, -1.f, -1.f },
                    { -1.f, 1.f, -1.f },
                    { 1.f, 1.f, -1.f },
                    { 1.f, -1.f, -1.f },
                    { -1.f, -1.f, -1.f } } } },
             { BlockFace::Left,
                { { { -1.f, -1.f, 1.f },
                    { -1.f, 1.f, -1.f },
                    { -1.f, -1.f, -1.f },
                    { -1.f, -1.f, 1.f },
                    { -1.f, 1.f, 1.f },
                    { -1.f, 1.f, -1.f } } } },
             { BlockFace::Right,
                { { { 1.f, 1.f, 1.f },
                    { 1.f, -1.f, -1.f },
                    { 1.f, 1.f, -1.f },
                    { 1.f, 1.f, 1.f },
                    { 1.f, -1.f, 1.f },
                    { 1.f, -1.f, -1.f } } } },
             { BlockFace::Bottom,
                { { { 1.f, -1.f, 1.f },
                    { -1.f, -1.f, -1.f },
                    { 1.f, -1.f, -1.f },
                    { 1.f, -1.f, 1.f },
                    { -1.f, -1.f, 1.f },
                    { -1.f, -1.f, -1.f } } } },
             { BlockFace::Top,
                { { { -1.f, 1.f, 1.f },
                    { 1.f, 1.f, -1.f },
                    { -1.f, 1.f, -1.f },
                    { -1.f, 1.f, 1.f },
                    { 1.f, 1.f, 1.f },
                    { 1.f, 1.f, -1.f } } } } }
        };

        static Hx::EnumArray<BlockFace, glm::vec3> sQuadNormals {
            { { BlockFace::Front, glm::forward },
             { BlockFace::Back, glm::back },
             { BlockFace::Left, glm::left },
             { BlockFace::Right, glm::right },
             { BlockFace::Bottom, glm::down },
             { BlockFace::Top, glm::up } }
        };

        static Hx::EnumArray<BlockFace, glm::ivec3> sNeighbours {
            { { BlockFace::Front, glm::back },
             { BlockFace::Back, glm::forward },
             { BlockFace::Left, glm::left },
             { BlockFace::Right, glm::right },
             { BlockFace::Bottom, glm::down },
             { BlockFace::Top, glm::up } }
        };

        static auto sEmptyChunk = std::make_shared<Chunk>();

        auto Index(const auto x, const auto y, const auto z)
        {
            return x + 1 + ((y + 1) * (Chunk::Size + 2)) + ((z + 1) * (Chunk::Size + 2) * (Chunk::Height + 2));
        }

        bool Solid(const flecs::world& world, const flecs::entity_t entity)
        {
            if (world.is_valid(entity))
            {
                return world.entity(entity).get<BlockType>() == BlockType::Solid;
            }
            return false;
        }

        Mesh BuildChunkMeshImpl(const flecs::world& world, glm::ivec3 position, const std::shared_ptr<Chunk>& chunk)
        {
            if (!world.has<ActiveScene, GameScene>())
                return {};

            const auto* ctx = ClientWorldContext::Get(world);

            std::array array { sEmptyChunk, sEmptyChunk, sEmptyChunk, sEmptyChunk };
            for (auto [i, side]: {
                     std::pair { 0, glm::ivec3(glm::left)    },
                      std::pair { 1, glm::ivec3(glm::right)   },
                     std::pair { 2, glm::ivec3(glm::forward) },
                      std::pair { 3, glm::ivec3(glm::back)    }
            })
            {
                if (const auto it = ctx->chunkMap.find(position + side); it != ctx->chunkMap.end())
                {
                    array[i] = world.entity(it->second).get<ChunkHolder>().chunk;
                }
            }
            auto&& [left, right, front, back] = array;

            Mesh                                                         mesh;
            std::unordered_map<PackedVertex, size_t, PackedVertexHasher> indexMap;

            std::vector mask((Chunk::Size + 2) * (Chunk::Size + 2) * (Chunk::Height + 2), true);

            // Compute border
            for (int xz = 0; std::cmp_less(xz, Chunk::Size); ++xz)
            {
                for (int y = 0; std::cmp_less(y, Chunk::Height); ++y)
                {
                    mask[Index(-1, y, xz)]          = Solid(world, left->Get({ Chunk::Size - 1, y, xz }));
                    mask[Index(Chunk::Size, y, xz)] = Solid(world, right->Get({ 0, y, xz }));

                    mask[Index(xz, y, -1)]          = Solid(world, front->Get({ xz, y, Chunk::Size - 1 }));
                    mask[Index(xz, y, Chunk::Size)] = Solid(world, back->Get({ xz, y, 0 }));
                }
            }

            // Compute chunk mask
            for (size_t x = 0; x < Chunk::Size; ++x)
            {
                for (size_t z = 0; z < Chunk::Size; ++z)
                {
                    for (size_t y = 0; y < Chunk::Height; ++y)
                    {
                        mask[Index(x, y, z)] = Solid(world, chunk->Get({ x, y, z }));
                    }
                }
            }

            constexpr auto chunkSize = glm::vec3(Chunk::Size, Chunk::Height, Chunk::Size);
            for (int x = 0; std::cmp_less(x, Chunk::Size); ++x)
            {
                for (int z = 0; std::cmp_less(z, Chunk::Size); ++z)
                {
                    for (int y = 0; std::cmp_less(y, Chunk::Height); ++y)
                    {
                        if (!mask[Index(x, y, z)])
                            continue;

                        for (auto [face, n]: sNeighbours)
                        {
                            if (mask[Index(x + n.x, y + n.y, z + n.z)])
                                continue;

                            const auto color = world.entity(chunk->Get({ x, y, z })).get<BlockColor>().color;
                            for (const auto& vertex: sQuadVertices[face])
                            {
                                const auto offset = glm::vec3(x, y, z) + ((vertex + 1.f) / 2.f);
                                const auto scaled = offset / chunkSize;
                                const auto final  = scaled * 2.f - 1.f;

                                // Add tris to the mesh
                                const PackedVertex pv = { final, color, {}, sQuadNormals[face] };
                                auto [it, inserted]   = indexMap.try_emplace(pv, mesh.vertex.size());
                                if (inserted)
                                {
                                    mesh.vertex.push_back(pv);
                                }
                                mesh.index.push_back(it->second);
                            }
                        }
                    }
                }
            }

            return mesh;
        }

    }

    void OnPlayerMoveObserver(const flecs::iter& it, size_t, const Transform& transform)
    {
        static int cx = std::numeric_limits<int>::max();
        static int cz = std::numeric_limits<int>::max();

        const auto  world = it.world();
        const auto* ctx   = ClientWorldContext::Get(world);

        const int rRange = static_cast<int>(ctx->settings.renderDistance);
        const int bRange = static_cast<int>(ctx->settings.preloadDistance);

        auto [x, z] = Helper::GetPlayerChunkPosition(transform.position);

        // MCC_LOG_DEBUG("{} {}", x, z);
        if (std::abs(cx - x) > 0 || std::abs(cz - z) > 0)
        {
            Helper::ForInCircle(cx, cz, rRange + 1, [&](long i, long j) {
                if (const auto cit = ctx->chunkMap.find({ i, 0, j }); cit != ctx->chunkMap.end())
                {
                    // MCC_LOG_DEBUG("found");
                    world.entity(cit->second).remove<ShouldRenderChunkTag>();
                }
            });

            Helper::ForInCircle(x, z, bRange, [&](long i, long j) {
                if (const auto cit = ctx->chunkMap.find({ i, 0, j }); cit != ctx->chunkMap.end())
                {
                    world.entity(cit->second).add<CouldRenderChunkTag>();
                    if (Helper::IsInCircle({ x, z }, { i, j }, rRange))
                        world.entity(cit->second).add<ShouldRenderChunkTag>();
                }
            });

            cx = x;
            cz = z;
        }
    }

    void OnChunkCreatedObserver(const flecs::entity entity, const ChunkPosition& p)
    {
        const auto  world = entity.world();
        const auto* ctx   = ClientWorldContext::Get(world);

        if (auto id = ctx->networkMapping.GetLHandle(ctx->playerInfo.handle); id.has_value())
        {
            world.entity(*id).get([&](const Transform& transform) {
                auto [pX, pZ]     = Helper::GetPlayerChunkPosition(transform.position);
                const auto cX     = p.position.x;
                const auto cZ     = p.position.z;
                const auto rRange = static_cast<int>(ctx->settings.renderDistance);
                const auto bRange = static_cast<int>(ctx->settings.preloadDistance);

                if (Helper::IsInCircle({ pX, pZ }, { cX, cZ }, rRange))
                {
                    entity.add<CouldRenderChunkTag>();
                    entity.add<ShouldRenderChunkTag>();
                }
                else if (Helper::IsInCircle({ pX, pZ }, { cX, cZ }, bRange))
                {
                    entity.add<CouldRenderChunkTag>();
                }
            });
        }
    }

    void OnChunkChangedObserver(const flecs::entity entity, const ChunkHolder&)
    {
        entity.add<ShouldBuildMeshTag>();
    }

    void BuildChunkMeshSystem(const flecs::entity entity, const ChunkHolder& holder, const ChunkPosition& pos)
    {
        const auto  world = entity.world();
        const auto* ctx   = ClientWorldContext::Get(world);

        // auto task = ctx->scheduler.EnqueueTask(
        //     Hx::AsShared, std::string("game_group"),
        //     [=] {
        //         return MCC_BENCH_TIME(MeshBuilding, _::BuildChunkMeshImpl)(world, pos.position, holder.chunk);
        //     }
        // );

        auto task = ctx->scheduler
                        .Insert(MCC_BENCH_TIME(MeshBuilding, _::BuildChunkMeshImpl), world, pos.position, holder.chunk)
                        .AsUnique()
                        .SetGroup("game_group")
                        .Enqueue();
        entity.remove<ShouldBuildMeshTag>();
        entity.emplace<MeshHolder>(std::move(task));
    }

    void TerrainRendererModule::SetupChunkRenderingMeshSystem(const flecs::entity entity, MeshHolder& holder) const
    {
        if (holder.pendingMesh.GetState() == Hx::TaskState::Cancelled)
        {
            // TODO
            entity.remove<MeshHolder>();
            return;
        }

        if (holder.pendingMesh.GetState() == Hx::TaskState::Done)
        {
            if ([[maybe_unused]] auto mesh = entity.try_get<ChunkMesh>())
            {
                // TODO
            }
            else
            {
                const auto result = holder.pendingMesh.GetResult();
                MCC_ASSERT(result, "Mesh task data has already been retrieve");
                auto [vertex, index] = result->get();
                VertexArray vArray {};
                Buffer      vBuffer { GL_ARRAY_BUFFER };
                Buffer      iBuffer { GL_ELEMENT_ARRAY_BUFFER };

                mProgram.Use();
                vArray.Create();
                vArray.Bind();

                vBuffer.Create();
                vBuffer.SetData(std::span(vertex), GL_STATIC_DRAW);
                mProgram.SetVertexAttribPointer("inVertex", 3, GL_FLOAT, sizeof(PackedVertex), 0);
                mProgram.SetVertexAttribPointer("inColor", 3, GL_FLOAT, sizeof(PackedVertex), 3 * sizeof(float));
                mProgram.SetVertexAttribPointer("inNormal", 3, GL_FLOAT, sizeof(PackedVertex), 8 * sizeof(float));

                iBuffer.Create();
                iBuffer.SetData(std::span(index), GL_STATIC_DRAW);

                entity.set<ChunkMesh>({ std::move(vArray), std::move(vBuffer), std::move(iBuffer), index.size() });
                entity.remove<MeshHolder>();
            }
        }
    }

    void TerrainRendererModule::SetupChunkProgramSystem(flecs::iter& it)
    {
        while (it.next()) {}

        const Shader vertexShader(GL_VERTEX_SHADER, R"""(
			#version 330

			in vec3 inVertex;
			in vec3 inColor;
            in vec3 inNormal;

			uniform mat4 view;
			uniform mat4 proj;
			uniform mat4 model;

			out vec3 passColor;
            out vec3 passPos;
            out vec3 passNormal;

			void main() {
				gl_Position = proj * view * model * vec4(inVertex, 1.0);
				passColor  = inColor;
                passPos    = vec3(model * vec4(inVertex, 1.0));
                passNormal = inNormal;
			}
		)""");
        const Shader fragmentShader(GL_FRAGMENT_SHADER, R"""(
			#version 330

			in vec3 passColor;
            in vec3 passPos;
            in vec3 passNormal;

            uniform mat3 invModel;
            uniform vec3 viewPos;

			out vec4 fragment;

            vec3 lightPos   = vec3(0, 100, 0);
            vec3 lightColor = vec3(1, 1, 1);

            float ambientStrength  = 0.1;
            float specularStrength = 0.5;

			void main() {
                vec3 normal = invModel * passNormal;
                vec3 norm = normalize(normal);
                vec3 lightDir = normalize(lightPos - passPos);

                // Ambient lighting
                vec3 ambient = ambientStrength * lightColor;

                // Diffuse lighting
                float diff = max(dot(norm, lightDir), 0.0);
                vec3 diffuse = diff * lightColor;

                // Specular lighting
                vec3 viewDir = normalize(viewPos - passPos);
                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
                vec3 specular = specularStrength * spec * lightColor;

				fragment = vec4((ambient + diffuse + specular) * passColor, 1.0);
			}
		)""");

        mProgram.Create();
        mProgram.Attach(vertexShader);
        mProgram.Attach(fragmentShader);

        mProgram.Link();

        mProgram.Detach(vertexShader);
        mProgram.Detach(fragmentShader);
    }

    void TerrainRendererModule::RenderChunkMeshSystem(flecs::iter& it)
    {
        const auto modelLocation    = mProgram.GetUniformLocation("model");
        const auto invModelLocation = mProgram.GetUniformLocation("invModel");

        mProgram.Use();

        const auto&& [p, view, proj] = RendererModule::GetView(it.world());
        mProgram.SetUniformMatrix(mProgram.GetUniformLocation("view"), view);
        mProgram.SetUniformMatrix(mProgram.GetUniformLocation("proj"), proj);

        mProgram.SetUniformVector(mProgram.GetUniformLocation("viewPos"), p);

        while (it.next())
        {
            auto pos  = it.field<const ChunkPosition>(0);
            auto mesh = it.field<const ChunkMesh>(1);

            for (const auto i: it)
            {
                glm::mat4 model = glm::scale(
                    glm::translate(
                        glm::mat4(1.f), glm::vec3(pos[i].position * glm::ivec3(2 * Chunk::Size, 0, 2 * Chunk::Size))
                    ),
                    glm::vec3(Chunk::Size, Chunk::Height, Chunk::Size)
                );

                mProgram.SetUniformMatrix(invModelLocation, glm::transpose(glm::inverse(glm::mat3(model))));
                mProgram.SetUniformMatrix(modelLocation, model);

                mesh[i].vertexArray.Bind();
                mesh[i].indexBuffer.Bind();
                // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                // glDisable(GL_CULL_FACE);
                glCheck(glDrawElements(GL_TRIANGLES, mesh[i].indexCount, GL_UNSIGNED_INT, nullptr));
                // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
    }

}
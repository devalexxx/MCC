// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/TerrainRenderer/System.h"

#include "Client/ClientApplication.h"
#include "Client/Graphics/Mesh.h"
#include "Client/Graphics/OpenGL/OpenGLProgram.h"
#include "Client/Graphics/OpenGL/OpenGLShader.h"
#include "Client/Module/Renderer/Component.h"
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
#include "Common/Utils/SafeAccess.h"

#include <Hexis/Core/EnumArray.h>
#include <Hexis/Core/VariantUtils.h>

#include <numeric>
#include <utility>
#include <set>

namespace Mcc
{

    namespace _
    {

        const Hx::EnumArray<BlockFace, std::array<glm::vec3, 6>>& GetQuadVertices()
        {
            static Hx::EnumArray<BlockFace, std::array<glm::vec3, 6>> sQuadVertices = {{
                { BlockFace::Front, {{
                    { -1.f, 1.f, 1.f }, {  1.f, -1.f, 1.f }, { 1.f,  1.f, 1.f },
                    { -1.f, 1.f, 1.f }, { -1.f, -1.f, 1.f }, { 1.f, -1.f, 1.f }
                }}},
                { BlockFace::Back, {{
                    { 1.f, 1.f, -1.f }, { -1.f, -1.f, -1.f }, { -1.f,  1.f, -1.f },
                    { 1.f, 1.f, -1.f }, {  1.f, -1.f, -1.f }, { -1.f, -1.f, -1.f }
                }}},
                { BlockFace::Left, {{
                    { -1.f, -1.f, 1.f }, { -1.f, 1.f, -1.f }, { -1.f, -1.f, -1.f },
                    { -1.f, -1.f, 1.f }, { -1.f, 1.f,  1.f }, { -1.f,  1.f, -1.f }
                }}},
                { BlockFace::Right, {{
                    { 1.f, 1.f, 1.f }, { 1.f, -1.f, -1.f }, { 1.f,  1.f, -1.f },
                    { 1.f, 1.f, 1.f }, { 1.f, -1.f,  1.f }, { 1.f, -1.f, -1.f }
                }}},
                { BlockFace::Bottom, {{
                    { 1.f, -1.f, 1.f }, { -1.f, -1.f, -1.f }, {  1.f, -1.f, -1.f },
                    { 1.f, -1.f, 1.f }, { -1.f, -1.f,  1.f }, { -1.f, -1.f, -1.f }
                }}},
                { BlockFace::Top, {{
                    { -1.f, 1.f, 1.f }, { 1.f, 1.f, -1.f }, { -1.f, 1.f, -1.f },
                    { -1.f, 1.f, 1.f }, { 1.f, 1.f,  1.f }, {  1.f, 1.f, -1.f }
                }}}
            }};

            return sQuadVertices;
        }

        const Hx::EnumArray<BlockFace, std::array<std::pair<glm::vec3, glm::vec2>, 6>>& GetQuadVerticesAndUvs()
        {
            static Hx::EnumArray<BlockFace, std::array<std::pair<glm::vec3, glm::vec2>, 6>> sQuadVertices = {{
                { BlockFace::Front, {{
                    {{ -1.f,  1.f, 1.f }, { 0.f, 1.f }}, {{  1.f, -1.f, 1.f }, { 1.f, 0.f }},
                    {{  1.f,  1.f, 1.f }, { 1.f, 1.f }}, {{ -1.f,  1.f, 1.f }, { 0.f, 1.f }},
                    {{ -1.f, -1.f, 1.f }, { 0.f, 0.f }}, {{  1.f, -1.f, 1.f }, { 1.f, 0.f }}
                }}},
                { BlockFace::Back, {{
                    {{  1.f,  1.f, -1.f }, { 1.f, 1.f }}, {{ -1.f, -1.f, -1.f }, { 0.f, 1.f }},
                    {{ -1.f,  1.f, -1.f }, { 0.f, 1.f }}, {{  1.f,  1.f, -1.f }, { 1.f, 1.f }},
                    {{  1.f, -1.f, -1.f }, { 1.f, 0.f }}, {{ -1.f, -1.f, -1.f }, { 0.f, 0.f }}
                }}},
                { BlockFace::Left, {{
                    {{ -1.f, -1.f,  1.f }, { 0.f, 1.f }}, {{ -1.f,  1.f, -1.f }, { 1.f, 0.f }},
                    {{ -1.f, -1.f, -1.f }, { 0.f, 0.f }}, {{ -1.f, -1.f,  1.f }, { 0.f, 1.f }},
                    {{ -1.f,  1.f,  1.f }, { 1.f, 1.f }}, {{ -1.f,  1.f, -1.f }, { 1.f, 0.f }}
                }}},
                { BlockFace::Right, {{
                    {{ 1.f,  1.f,  1.f }, { 1.f, 1.f }}, {{ 1.f, -1.f, -1.f }, { 0.f, 0.f }},
                    {{ 1.f,  1.f, -1.f }, { 1.f, 0.f }}, {{ 1.f,  1.f,  1.f }, { 1.f, 1.f }},
                    {{ 1.f, -1.f,  1.f }, { 0.f, 1.f }}, {{ 1.f, -1.f, -1.f }, { 0.f, 0.f }}
                }}},
                { BlockFace::Bottom, {{
                    {{  1.f, -1.f,  1.f }, { 1.f, 1.f }}, {{ -1.f, -1.f, -1.f }, { 0.f, 0.f }},
                    {{  1.f, -1.f, -1.f }, { 1.f, 0.f }}, {{  1.f, -1.f,  1.f }, { 1.f, 1.f, }},
                    {{ -1.f, -1.f,  1.f }, { 0.f, 1.f }}, {{ -1.f, -1.f, -1.f }, { 0.f, 0.f }}
                }}},
                { BlockFace::Top, {{
                    {{ -1.f, 1.f,  1.f }, { 0.f, 1.f }}, {{  1.f, 1.f, -1.f }, { 1.f, 0.f }},
                    {{ -1.f, 1.f, -1.f }, { 0.f, 0.f }}, {{ -1.f, 1.f,  1.f }, { 0.f, 1.f }},
                    {{  1.f, 1.f,  1.f }, { 1.f, 1.f }}, {{  1.f, 1.f, -1.f }, { 1.f, 0.f }}
                }}}
            }};

            return sQuadVertices;
        }

        const Hx::EnumArray<BlockFace, glm::vec3>& GetQuadNormals()
        {
            static Hx::EnumArray<BlockFace, glm::vec3> sQuadNormals = {{
                { BlockFace::Front , glm::forward },
                { BlockFace::Back  , glm::back    },
                { BlockFace::Left  , glm::left    },
                { BlockFace::Right , glm::right   },
                { BlockFace::Bottom, glm::down    },
                { BlockFace::Top   , glm::up      }
            }};

            return sQuadNormals;
        }

        Hx::EnumArray<BlockFace, glm::ivec3>& GetBlockNeighbours()
        {
            static Hx::EnumArray<BlockFace, glm::ivec3> sNeighbours = {{
                { BlockFace::Front , glm::back    },
                { BlockFace::Back  , glm::forward },
                { BlockFace::Left  , glm::left    },
                { BlockFace::Right , glm::right   },
                { BlockFace::Bottom, glm::down    },
                { BlockFace::Top   , glm::up      }
            }};

            return sNeighbours;
        }

        const std::vector<std::pair<size_t, glm::ivec3>>& GetChunkNeighbours()
        {
            static std::vector<std::pair<size_t, glm::ivec3>> sNeighbours = {{
                { 0, glm::ivec3(glm::left)    },
                { 1, glm::ivec3(glm::right)   },
                { 2, glm::ivec3(glm::forward) },
                { 3, glm::ivec3(glm::back)    }
            }};

            return sNeighbours;
        }

        std::shared_ptr<Chunk> GetEmptyChunk()
        {
            static auto sEmptyChunk = std::make_shared<Chunk>();
            return sEmptyChunk;
        }

        auto Index(const auto x, const auto y, const auto z)
        {
            return x + 1 + ((y + 1) * (Chunk::Size + 2)) + ((z + 1) * (Chunk::Size + 2) * (Chunk::Height + 2));
        }

        bool Solid(const flecs::world& world, const flecs::entity_t entity)
        {
            if (world.is_valid(entity))
            {
                return world.entity(entity).get<CBlockType>() == CBlockType::Solid;
            }
            return false;
        }

        Mesh BuildChunkMeshImpl(
            const flecs::world& world, glm::ivec3 position, std::shared_ptr<Chunk> chunk,
            decltype(TerrainRendererModule::textureIndex)  textureIndex,
            decltype(TerrainRendererModule::textureToLoad) textureToLoad
        )
        {
            if (!world.has<ActiveScene, GameScene>())
                return {};

            const auto* ctx = ClientWorldContext::Get(world);

            std::array array { GetEmptyChunk(), GetEmptyChunk(), GetEmptyChunk(), GetEmptyChunk() };
            for (auto [i, side]: GetChunkNeighbours())
            {
                if (const auto it = ctx->chunkMap.find(position + side); it != ctx->chunkMap.end())
                {
                    array[i] = world.entity(it->second).get<CChunkPtr>();
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
                    mask[Index(-1, y, xz)]          = Solid(world, left ->Get({ Chunk::Size - 1, y, xz }));
                    mask[Index(Chunk::Size, y, xz)] = Solid(world, right->Get({ 0, y, xz }));

                    mask[Index(xz, y, -1)]          = Solid(world, front->Get({ xz, y, Chunk::Size - 1 }));
                    mask[Index(xz, y, Chunk::Size)] = Solid(world, back ->Get({ xz, y, 0 }));
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

                        const auto& asset = world.entity(chunk->Get({ x, y, z })).get<CBlockAsset>();
                        const auto& qv    = GetQuadVerticesAndUvs();
                        const auto& qn    = GetQuadNormals();
                        for (auto [face, n]: GetBlockNeighbours())
                        {
                            if (mask[Index(x + n.x, y + n.y, z + n.z)])
                                continue;

                            auto texture = std::visit(
                                Hx::Overloaded {
                                    [](const std::string& string) -> std::string {
                                        return string;
                                    },
                                    [&](const Hx::EnumArray<BlockFace, std::string>& enumArray) -> std::string {
                                        return enumArray[face];
                                    }
                                },
                                asset.texture
                            );
                            auto [tIt, tInserted] = (*textureIndex)->try_emplace(texture, (*textureIndex)->size());
                            auto tIndex = tIt->second;
                            if (tInserted)
                            {
                                (*textureToLoad)->emplace(texture);
                            }

                            for (const auto& [vertex, uv]: qv[face])
                            {
                                const auto offset = glm::vec3(x, y, z) + ((vertex + 1.f) / 2.f);
                                const auto scaled = offset / chunkSize;
                                const auto final  = scaled * 2.f - 1.f;

                                // Add tris to the mesh
                                const PackedVertex pv = {
                                    .vertex=final, .color={ 0.f, 0.f, 0.f }, .uv=glm::vec3(uv, tIndex), .normal=qn[face]
                                };
                                auto [it, inserted] = indexMap.try_emplace(pv, mesh.vertex.size());
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

    void OnPlayerMoveObserver(const flecs::iter& it, size_t, const CTransform& transform)
    {
        static int cx = std::numeric_limits<int>::max();
        static int cz = std::numeric_limits<int>::max();

        const auto  world = it.world();
        const auto* ctx   = ClientWorldContext::Get(world);

        const int rRange = static_cast<int>(ctx->settings.renderDistance);
        const int bRange = static_cast<int>(ctx->settings.preloadDistance);

        auto [x, z] = Helper::GetPlayerChunkPosition(transform.position);

        if (std::abs(cx - x) > 0 || std::abs(cz - z) > 0)
        {
            Helper::ForInCircle(cx, cz, rRange + 1, [&](long i, long j) {
                if (const auto cit = ctx->chunkMap.find({ i, 0, j }); cit != ctx->chunkMap.end())
                {
                    world.entity(cit->second).remove<TShouldRenderChunk>();
                }
            });

            Helper::ForInCircle(x, z, bRange, [&](long i, long j) {
                if (const auto cit = ctx->chunkMap.find({ i, 0, j }); cit != ctx->chunkMap.end())
                {
                    world.entity(cit->second).add<TCouldRenderChunk>();
                    if (Helper::IsInCircle({ x, z }, { i, j }, rRange))
                        world.entity(cit->second).add<TShouldRenderChunk>();
                }
            });

            cx = x;
            cz = z;
        }
    }

    void OnChunkCreatedObserver(const flecs::entity entity, const CChunkPos& p)
    {
        const auto  world = entity.world();
        const auto* ctx   = ClientWorldContext::Get(world);

        if (auto id = ctx->networkMapping.GetLHandle(ctx->playerInfo.handle); id.has_value())
        {
            world.entity(*id).get([&](const CTransform& transform) {
                auto [pX, pZ]     = Helper::GetPlayerChunkPosition(transform.position);
                const auto cX     = p.x;
                const auto cZ     = p.z;
                const auto rRange = static_cast<int>(ctx->settings.renderDistance);
                const auto bRange = static_cast<int>(ctx->settings.preloadDistance);

                if (Helper::IsInCircle({ pX, pZ }, { cX, cZ }, rRange))
                {
                    entity.add<TCouldRenderChunk>();
                    entity.add<TShouldRenderChunk>();
                }
                else if (Helper::IsInCircle({ pX, pZ }, { cX, cZ }, bRange))
                {
                    entity.add<TCouldRenderChunk>();
                }
            });
        }
    }

    void OnChunkChangedObserver(const flecs::entity entity, const CChunkPtr&)
    {
        entity.add<TShouldBuildMesh>();
    }

    void BuildChunkMeshSystem(const flecs::entity entity, const CChunkPtr& ptr, const CChunkPos& pos)
    {
        const auto  world  = entity.world();
        const auto& module = world.get<TerrainRendererModule>();
        const auto* ctx    = ClientWorldContext::Get(world);

        auto task = ctx->scheduler
            .Insert(
                MCC_BENCH_TIME(MeshBuilding, _::BuildChunkMeshImpl),
                world, pos, ptr, module.textureIndex, module.textureToLoad
            )
            .AsUnique()
            .SetGroup("game_group")
            .Enqueue();

        entity.remove<TShouldBuildMesh>();
        entity.emplace<CChunkMeshGenTask>(std::move(task));
    }

    void SetupChunkRenderingMeshSystem(const flecs::entity entity, CChunkMeshGenTask& task)
    {
        if (task.GetState() == Hx::TaskState::Cancelled)
        {
            // TODO
            entity.remove<CChunkMeshGenTask>();
            return;
        }

        if (task.GetState() == Hx::TaskState::Done)
        {
            if ([[maybe_unused]] auto mesh = entity.try_get<COpenGLMesh>())
            {
                // TODO
            }
            else
            {
                const auto& module = entity.world().get_mut<TerrainRendererModule>();
                const auto result  = task.GetResult();
                MCC_ASSERT(result, "Mesh task data has already been retrieve");
                auto [vertex, index] = result->get();
                OpenGLVertexArray vArray {};
                OpenGLBuffer      vBuffer { GL_ARRAY_BUFFER };
                OpenGLBuffer      iBuffer { GL_ELEMENT_ARRAY_BUFFER };

                module.program->Bind();
                vArray.Create();
                vArray.Bind();

                vBuffer.Create();
                vBuffer.SetData(std::span(vertex), GL_STATIC_DRAW);
                module.program->SetVertexAttribPointer("inVertex", 3, GL_FLOAT, sizeof(PackedVertex), 0);
                module.program->SetVertexAttribPointer("inColor", 3, GL_FLOAT, sizeof(PackedVertex), 3 * sizeof(float));
                module.program->SetVertexAttribPointer("inTexCoord", 3, GL_FLOAT, sizeof(PackedVertex), 6 * sizeof(float));
                module.program->SetVertexAttribPointer("inNormal", 3, GL_FLOAT, sizeof(PackedVertex), 9 * sizeof(float));

                iBuffer.Create();
                iBuffer.SetData(std::span(index), GL_STATIC_DRAW);

                entity.set<COpenGLMesh>({
                    .vertexArray =std::move(vArray),
                    .vertexBuffer=std::move(vBuffer),
                    .indexBuffer =std::move(iBuffer),
                    .indexCount  =index.size()
                });
                entity.add<ROpenGLMesh>   (entity);
                entity.add<ROpenGLProgram>(module.programEntity);
                entity.add<ROpenGLTexture>(module.textureArrayEntity);

                entity.remove<CChunkMeshGenTask>();
            }
        }
    }

    void SetupChunkProgramSystem(flecs::iter& it)
    {
        OpenGLShader vertexShader(GL_VERTEX_SHADER, R"""(
			#version 330

			in vec3 inVertex;
			in vec3 inColor;
            in vec3 inTexCoord;
            in vec3 inNormal;

			uniform mat4 view;
			uniform mat4 proj;
			uniform mat4 model;

			out vec3 passColor;
            out vec3 passPos;
            out vec3 passTexCoord;
            out vec3 passNormal;

			void main() {
				gl_Position  = proj * view * model * vec4(inVertex, 1.0);
				passColor    = inColor;
                passPos      = vec3(model * vec4(inVertex, 1.0));
                passNormal   = inNormal;
                passTexCoord = inTexCoord;
			}
		)""");

        OpenGLShader fragmentShader(GL_FRAGMENT_SHADER, R"""(
			#version 330
            #extension GL_ARB_texture_query_lod : enable

			in vec3 passColor;
            in vec3 passPos;
            in vec3 passTexCoord;
            in vec3 passNormal;

            uniform mat3 invModel;
            uniform vec3 vPos;

            uniform sampler2DArray tex;

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
                vec3 viewDir = normalize(vPos - passPos);
                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
                vec3 specular = specularStrength * spec * lightColor;

                float mipmapLevel = 0;
            #ifdef GL_ARB_texture_query_lod
                mipmapLevel = textureQueryLOD(tex, passTexCoord.xy).x;
            #endif
                vec4 pixel = textureLod(tex, passTexCoord, mipmapLevel);
				fragment = vec4(ambient + diffuse + specular, 1.f) * pixel;
			}
		)""");

        const auto word   = it.world();
        auto&      module = word.get_mut<TerrainRendererModule>();

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

        module.programEntity = word.entity().set<COpenGLProgram>(module.program);

        module.textureArray->Create();
        module.textureArray->SetParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        module.textureArray->SetParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        module.textureArray->SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        module.textureArray->SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

        module.textureArrayEntity = word.entity().set<COpenGLTexture>(module.textureArray);

        IgnoreIter(it);
    }

    void LoadTextureSystem(flecs::iter& it)
    {
        IgnoreIter(it);

        const auto  world  = it.world();
        const auto& module = world.get_mut<TerrainRendererModule>();

        if ((*module.textureToLoad)->empty())
            return;

        std::vector<Image>       images;
        std::vector<std::string> paths;

        {
            auto tttProxy = *(*module.textureToLoad);
            images.reserve(tttProxy->size());
            paths .reserve(tttProxy->size());

            std::copy(tttProxy->begin(), tttProxy->end(), std::back_inserter(paths));
            tttProxy->clear();
        }

        for (auto& path: paths)
        {
            images.push_back(STBLoadImage(path.c_str()));
        }

        module.textureArray->AddData(images);
        module.textureArray->GenerateMipmap();
    }

}
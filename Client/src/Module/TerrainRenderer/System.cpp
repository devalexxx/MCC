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
#include "Common/Utils/SafeAccess.h"
#include "Common/World/Translation.h"
#include "Common/World/Geometry.h"
#include "Common/WorldContext.h"

#include <Hexis/Core/EnumArray.h>
#include <Hexis/Core/VariantUtils.h>
#include <glm/gtx/quaternion.hpp>

#include <numeric>
#include <set>
#include <utility>

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
                    {{  1.f,  1.f, -1.f }, { 0.f, 1.f }}, {{ -1.f, -1.f, -1.f }, { 1.f, 0.f }},
                    {{ -1.f,  1.f, -1.f }, { 1.f, 1.f }}, {{  1.f,  1.f, -1.f }, { 0.f, 1.f }},
                    {{  1.f, -1.f, -1.f }, { 0.f, 0.f }}, {{ -1.f, -1.f, -1.f }, { 1.f, 0.f }}
                }}},
                { BlockFace::Left, {{
                    {{ -1.f, -1.f,  1.f }, { 1.f, 0.f }}, {{ -1.f,  1.f, -1.f }, { 0.f, 1.f }},
                    {{ -1.f, -1.f, -1.f }, { 0.f, 0.f }}, {{ -1.f, -1.f,  1.f }, { 1.f, 0.f }},
                    {{ -1.f,  1.f,  1.f }, { 1.f, 1.f }}, {{ -1.f,  1.f, -1.f }, { 0.f, 1.f }}
                }}},
                { BlockFace::Right, {{
                    {{ 1.f,  1.f,  1.f }, { 0.f, 1.f }}, {{ 1.f, -1.f, -1.f }, { 1.f, 0.f }},
                    {{ 1.f,  1.f, -1.f }, { 1.f, 1.f }}, {{ 1.f,  1.f,  1.f }, { 0.f, 1.f }},
                    {{ 1.f, -1.f,  1.f }, { 0.f, 0.f }}, {{ 1.f, -1.f, -1.f }, { 1.f, 0.f }}
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

        const std::vector<std::pair<size_t, glm::ivec2>>& GetChunkNeighbours()
        {
            static std::vector<std::pair<size_t, glm::ivec2>> sNeighbours = {{
                { 0, glm::ivec2(-1, 0) },
                { 1, glm::ivec2( 1, 0) },
                { 2, glm::ivec2(0, -1) },
                { 3, glm::ivec2(0,  1) }
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
            return x + 1 + ((y + 1) * (Chunk::Size + 2)) + ((z + 1) * (Chunk::Size + 2) * (Chunk::Size + 2));
        }

        struct ChunkMeshBuildDesc
        {
            glm::ivec3 position;

            std::shared_ptr<Chunk>                             chunk;
            std::vector<Hx::EnumArray<BlockFace, std::string>> assetPath;

            std::array<std::shared_ptr<Chunk>, 4>     neighbors; // left, right, front, back
            std::unordered_map<flecs::entity_t, bool> solid;
        };

        struct ChunkTextureArrayDesc
        {
            decltype(TerrainRendererModule::textureIndex)  textureIndex;
            decltype(TerrainRendererModule::textureToLoad) textureToLoad;
        };

        Mesh BuildChunkMeshImpl(const flecs::world& world, ChunkMeshBuildDesc cDesc, ChunkTextureArrayDesc tDesc)
        {
            if (!world.has<ActiveScene, GameScene>())
                return {};

            auto&& [left, right, front, back] = cDesc.neighbors;

            // Compute chunk mask
            std::vector mask((Chunk::Size + 2) * (Chunk::Size + 2) * (Chunk::Size + 2), true);
            for (size_t x = 0; x < Chunk::Size; ++x)
            {
                for (size_t y = 0; y < Chunk::Size; ++y)
                {
                    mask[Index(-1, y, x)]          = cDesc.solid[left ->Get({ Chunk::Size - 1, y, x })];
                    mask[Index(Chunk::Size, y, x)] = cDesc.solid[right->Get({ 0, y, x })];

                    mask[Index(x, y, -1)]          = cDesc.solid[front->Get({ x, y, Chunk::Size - 1 })];
                    mask[Index(x, y, Chunk::Size)] = cDesc.solid[back ->Get({ x, y, 0 })];

                    for (size_t z = 0; z < Chunk::Size; ++z)
                    {
                        mask[Index(x, y, z)] = cDesc.solid[cDesc.chunk->Get({ x, y, z })];
                    }
                }
            }

            Mesh                                                         mesh;
            std::unordered_map<PackedVertex, size_t, PackedVertexHasher> indexMap;
            constexpr auto chunkSize = glm::vec3(Chunk::Size);
            for (int x = 0; std::cmp_less(x, Chunk::Size); ++x)
            {
                for (int z = 0; std::cmp_less(z, Chunk::Size); ++z)
                {
                    for (int y = 0; std::cmp_less(y, Chunk::Size); ++y)
                    {
                        if (!mask[Index(x, y, z)])
                            continue;

                        const auto pIndex = cDesc.chunk->GetPaletteIndex({ x, y, z });
                        const auto& qv    = GetQuadVerticesAndUvs();
                        const auto& qn    = GetQuadNormals();
                        for (auto [face, n]: GetBlockNeighbours())
                        {
                            if (mask[Index(x + n.x, y + n.y, z + n.z)])
                                continue;

                            auto texturePath = cDesc.assetPath[pIndex][face];
                            auto [tIt, tInserted] = (*tDesc.textureIndex)->try_emplace(texturePath,(*tDesc.textureIndex)->size());
                            if (tInserted)
                            {
                                (*tDesc.textureToLoad)->push_back(texturePath);
                            }
                            auto textureIndex = tIt->second;

                            for (const auto& [vertex, uv]: qv[face])
                            {
                                const auto offset = glm::vec3(x, y, z) + ((vertex + 1.f) / 2.f);
                                const auto scaled = offset / chunkSize;
                                const auto final  = scaled * 2.f - 1.f;

                                // Add tris to the mesh
                                const PackedVertex pv = {
                                    .vertex=final, .color={ 0.f, 0.f, 0.f }, .uv=glm::vec3(uv, textureIndex), .normal=qn[face]
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

    void OnPlayerMoveObserver(const flecs::iter& it, size_t, const CEntityTransform& transform)
    {
        static int cx = std::numeric_limits<int>::max();
        static int cy = std::numeric_limits<int>::max();
        static int cz = std::numeric_limits<int>::max();

        const auto  world = it.world();
        const auto* ctx   = ClientWorldContext::Get(world);

        const int rRange = static_cast<int>(ctx->settings.renderDistance);
        const int bRange = static_cast<int>(ctx->settings.preloadDistance);

        const auto [x, y, z] = get<0>(transform.position);
        if (std::abs(cx - x) > 0 || std::abs(cy - y) > 0 || std::abs(cz - z) > 0)
        {
            Helper::ForInSphere(cx, cy, cz, rRange + 1, [&](long i, long j, long k)
            {
                if (const auto cit = ctx->chunkMapping.find({ i, j, k }); cit != ctx->chunkMapping.end())
                {
                    const auto e = world.entity(cit->second);
                    e.remove<TShouldRenderChunk>();
                    e.remove<TRenderable>();
                }
            });

            Helper::ForInSphere(x, y, z, bRange, [&](long i, long j, long k)
            {
                if (const auto cit = ctx->chunkMapping.find({ i, j, k }); cit != ctx->chunkMapping.end())
                {
                    const auto e = world.entity(cit->second);
                    e.add<TCouldRenderChunk>();
                    if (Helper::IsInSphere({ x, y, z }, { i, j, k }, rRange))
                    {
                        e.add<TShouldRenderChunk>();
                        e.add<TRenderable>();
                    }
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

        // Setup render transform
        constexpr auto halfChunk = glm::vec3(Chunk::Size) * .5f;
        auto& [position, rotation, scale] = entity.ensure<CRenderTransform>();
        // TODO: set world pos relative to player transform
        position = static_cast<WorldPosF>(p) + TranslationF(halfChunk);
        rotation = glm::quat_identity<float, glm::defaultp>();
        scale    = halfChunk;

        // Setup render info
        const auto rRange = static_cast<int>(ctx->settings.renderDistance);
        const auto bRange = static_cast<int>(ctx->settings.preloadDistance);

        const auto player = PlayerModule::GetPlayer(world);
        if (player == flecs::entity::null()) return;

        auto& transform = player.get<CEntityTransform>();
        const auto cp = get<0>(transform.position);
        const auto cc = p.Underlying();

        entity.add<TShouldBuildMesh>();
        if (Helper::IsInSphere(cp, cc, rRange))
        {
            entity.add<TCouldRenderChunk>();
            entity.add<TShouldRenderChunk>();
            entity.add<TRenderable>();
        }
        else
        if (Helper::IsInSphere(cp, cc, bRange))
        {
            entity.add<TCouldRenderChunk>();
        }
    }

    void LoadTextureSystem(flecs::iter& it)
    {
        IgnoreIter(it);

        const auto  world  = it.world();
        const auto* ctx    = ClientWorldContext::Get(world);
        const auto& module = world.get_mut<TerrainRendererModule>();

        if ((*module.textureToLoad)->empty())
            return;

        std::vector<std::shared_ptr<Image>> images;
        std::vector<std::string>            paths;

        {
            auto tttProxy = *(*module.textureToLoad);
            images.reserve(tttProxy->size());
            paths .reserve(tttProxy->size());
            std::ranges::copy(*tttProxy, std::back_inserter(paths));
            tttProxy->clear();
        }

        for (auto& path: paths)
        {
            auto asset = ctx->assetRegistry.Get<Image>(path, false);
            if (!asset)
            {
                // TODO: temp
                images.push_back(std::make_shared<Image>(64, 64, 4, PixelFormat::RGBA, 0));
                continue;
            }

            images.push_back(asset);
        }

        // TODO: temp (perf issue buffer is get from cg every Add)
        for (auto image: images)
        {
            module.textureArray->AddData(*image.get());
        }
        module.textureArray->GenerateMipmap();
    }

    void SetupChunkProgramSystem(flecs::iter& it)
    {
        const auto  word   = it.world();
        const auto* ctx    = ClientWorldContext::Get(word);
        auto&       module = word.get_mut<TerrainRendererModule>();

        const auto vertexShader   = ctx->assetRegistry.Get<OpenGLShader>("shader://chunk.vert", false);
        const auto fragmentShader = ctx->assetRegistry.Get<OpenGLShader>("shader://chunk.frag", false);

        module.program->Create();
        module.program->Attach(*vertexShader.get());
        module.program->Attach(*fragmentShader.get());

        module.program->Link();

        module.program->Detach(*vertexShader.get());
        module.program->Detach(*fragmentShader.get());

        module.programEntity = word.entity().set<COpenGLProgram>(module.program);

        module.textureArray->Create();
        module.textureArray->SetParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        module.textureArray->SetParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        module.textureArray->SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        module.textureArray->SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

        module.textureArrayEntity = word.entity().set<COpenGLTexture>(module.textureArray);

        IgnoreIter(it);
    }

    void HandleDirtyChunkSystem(const flecs::entity entity)
    {
        entity.add<TShouldBuildMesh>();
    }

    void BuildChunkMeshSystem(const flecs::entity entity, const CChunkPtr& ptr, const CChunkPos& pos)
    {
        const auto  world  = entity.world();
        const auto& module = world.get<TerrainRendererModule>();
        const auto* ctx    = ClientWorldContext::Get(world);

        _::ChunkMeshBuildDesc cDesc;
        cDesc.position = pos;
        cDesc.chunk    = ptr;

        for (auto [i, side]: _::GetChunkNeighbours())
        {
            if (const auto it = ctx->chunkMapping.find(pos + TranslationV(side.x, 0, side.y)); it != ctx->chunkMapping.end())
            {
                cDesc.neighbors[i] = world.entity(it->second).get<CChunkPtr>();

                for (const auto e: cDesc.neighbors[i]->GetPalette())
                {
                    cDesc.solid.try_emplace(e, world.entity(e).get<CBlockType>() == CBlockType::Solid);
                }
            }
            else
            {
                cDesc.neighbors[i] = _::GetEmptyChunk();
                cDesc.solid.try_emplace(0, false);
            }
        }

        for (const auto e: ptr->GetPalette())
        {
            cDesc.solid.try_emplace(e, world.entity(e).get<CBlockType>() == CBlockType::Solid);
        }

        std::ranges::transform(ptr->GetPalette(), std::back_inserter(cDesc.assetPath), [&world](flecs::entity_t e) -> Hx::EnumArray<BlockFace, std::string>
        {
            if (const auto asset = world.entity(e).try_get<CBlockAsset>(); asset)
            {
                return std::visit(
                    Hx::Overloaded {
                        [](const std::string& string) -> Hx::EnumArray<BlockFace, std::string> {
                            return Hx::EnumArray<BlockFace, std::string> {
                                { BlockFace::Left,   string },
                                { BlockFace::Right,  string },
                                { BlockFace::Front,  string },
                                { BlockFace::Back,   string },
                                { BlockFace::Top,    string },
                                { BlockFace::Bottom, string }
                            };
                        },
                        [](const Hx::EnumArray<BlockFace, std::string>& eArray) -> Hx::EnumArray<BlockFace, std::string> {
                            return eArray;
                        },
                    },
                    asset->texture
                );
            }

            return {};
        });

        _::ChunkTextureArrayDesc tDesc;
        tDesc.textureIndex  = module.textureIndex;
        tDesc.textureToLoad = module.textureToLoad;

        auto task = ctx->scheduler
            .Insert(MCC_BENCH_TIME(MeshBuilding, _::BuildChunkMeshImpl), world, std::move(cDesc), std::move(tDesc))
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
            const auto& module = entity.world().get_mut<TerrainRendererModule>();
            module.program->Bind();

            COpenGLMesh* mesh = entity.try_get_mut<COpenGLMesh>();
            if (!mesh)
            {
                mesh = &entity.ensure<COpenGLMesh>();

                mesh->vertexArray .Create();

                mesh->vertexBuffer.Create();
                module.program->SetVertexAttribPointer("inVertex",   3, GL_FLOAT, sizeof(PackedVertex), 0);
                module.program->SetVertexAttribPointer("inColor",    3, GL_FLOAT, sizeof(PackedVertex), 3 * sizeof(float));
                module.program->SetVertexAttribPointer("inTexCoord", 3, GL_FLOAT, sizeof(PackedVertex), 6 * sizeof(float));
                module.program->SetVertexAttribPointer("inNormal",   3, GL_FLOAT, sizeof(PackedVertex), 9 * sizeof(float));

                mesh->indexBuffer.Create();

                entity.add<ROpenGLMesh>   (entity);
                entity.add<ROpenGLProgram>(module.programEntity);
                entity.add<ROpenGLTexture>(module.textureArrayEntity);
            }

            const auto result  = task.GetResult();
            MCC_ASSERT(result, "Mesh task data has already been retrieve");
            auto [vertex, index] = result->get();

            mesh->vertexArray.Bind();

            mesh->vertexBuffer.Bind();
            mesh->vertexBuffer.SetData(std::span(vertex), GL_STATIC_DRAW);

            mesh->indexBuffer.Bind();
            mesh->indexBuffer.SetData(std::span(index), GL_STATIC_DRAW);
            mesh->indexCount = index.size();

            entity.remove<CChunkMeshGenTask>();
        }
    }

}
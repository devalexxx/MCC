// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Graphics/Mesh.h"

#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/epsilon.hpp>

#include <algorithm>
#include <unordered_map>

namespace Mcc
{

    inline float Quantize(const float x, const float step = 1e-3f)
    {
        return std::round(x / step) * step;
    }

    inline glm::vec3 QVec(const glm::vec3& v, const float step = 1e-3f)
    {
        return { Quantize(v.x, step), Quantize(v.y, step), Quantize(v.z, step) };
    }

    inline glm::vec2 QVec(const glm::vec2& v, const float step = 1e-3f)
    {
        return { Quantize(v.x, step), Quantize(v.y, step) };
    }

    inline size_t QHash(const float x, const float step = 1e-3f)
    {
        return std::hash<float>()(Quantize(x, step));
    }

    bool operator==(const PackedVertex& lhs, const PackedVertex& rhs)
    {
        return QVec(lhs.vertex) == QVec(rhs.vertex) && QVec(lhs.color) == QVec(rhs.color) &&
               QVec(lhs.uv) == QVec(rhs.uv) && QVec(lhs.normal) == QVec(rhs.normal);
    }

    bool operator!=(const PackedVertex& lhs, const PackedVertex& rhs)
    {
        return !(lhs == rhs);
    }

    std::size_t PackedVertexHasher::operator()(const PackedVertex& pv) const
    {
        const std::size_t h1 = QHash(pv.vertex.x) ^ QHash(pv.vertex.y) << 1 ^ QHash(pv.vertex.z) << 2;
        const std::size_t h2 = QHash(pv.color.x) ^ QHash(pv.color.y) << 1 ^ QHash(pv.color.z) << 2;
        const std::size_t h3 = QHash(pv.uv.x) ^ QHash(pv.uv.y) << 1;
        const std::size_t h4 = QHash(pv.normal.x) ^ QHash(pv.normal.y) << 1 ^ QHash(pv.normal.z) << 2;
        return h1 ^ h2 ^ h3 ^ h4;
    }


    Mesh Index(const PackedVertexArray& vertices)
    {
        Mesh                                                         mesh;
        std::unordered_map<PackedVertex, size_t, PackedVertexHasher> vertexMap;
        for (auto& vertex: vertices)
        {
            auto [it, inserted] = vertexMap.try_emplace(vertex, mesh.vertex.size());
            if (inserted)
            {
                mesh.vertex.push_back(vertex);
            }
            mesh.index.push_back(it->second);
        }

        return mesh;
    }

    Mesh Optimize(const Mesh& mesh, float epsilonScale)
    {
        auto& [inVertex, inIndex] = mesh;

        Mesh optimized;
        optimized.index.resize(inIndex.size());

        auto comp = [epsilonScale, &inVertex](unsigned int i) {
            return [i, epsilonScale, &inVertex](const auto& v) {
                return glm::all(
                           glm::epsilonEqual(
                               inVertex[i].vertex, v.vertex, std::numeric_limits<float>::epsilon() * epsilonScale
                           )
                       ) &&
                       inVertex[i] == v;
            };
        };

        for (size_t i = 0; i < inVertex.size(); ++i)
        {
            if (auto found = std::ranges::find_if(optimized.vertex, comp(i)); found != optimized.vertex.end())
            {
                for (size_t j = 0; j < inIndex.size(); ++j)
                {
                    if (inIndex[j] == i)
                    {
                        optimized.index[j] = std::distance(optimized.vertex.begin(), found);
                    }
                }
            }
            else
            {
                optimized.vertex.push_back(inVertex[i]);
                for (size_t j = 0; j < inIndex.size(); ++j)
                {
                    if (inIndex[j] == i)
                    {
                        optimized.index[j] = optimized.vertex.size() - 1;
                    }
                }
            }
        }

        return optimized;
    }

    namespace Helper
    {

        namespace _
        {
            Mesh GenerateSphereMeshImpl(
                const float radius, const unsigned int segments, const unsigned int rings, const bool half
            )
            {
                MCC_ASSERT(
                    rings > (half ? 2u : 3u), "{} must have at least {} rings", half ? "Half sphere" : "Sphere",
                    half ? 2 : 3
                );
                MCC_ASSERT(segments > 3, "Sphere must have at least 3 segments");

                Mesh mesh;

                constexpr auto pi     = glm::pi<float>();
                const auto     theta  = 2.f * pi / static_cast<float>(segments);
                const auto     rRings = half ? rings * 2 : rings + 1;
                const auto     phi    = pi / static_cast<float>(rRings);
                const auto     offset = half ? rRings / 2 : 1;

                for (unsigned int i = offset; i < rRings; ++i)
                {
                    const float bA = static_cast<float>(i) * phi;
                    for (unsigned int j = 0; j <= segments; ++j)
                    {
                        if (j < segments)
                        {
                            const float hA = static_cast<float>(j) * theta;
                            const float x  = radius * std::sin(bA) * std::cos(hA);
                            const float y  = radius * std::cos(bA);
                            const float z  = radius * std::sin(bA) * std::sin(hA);
                            mesh.vertex.push_back({.vertex={ x, y, z }, .color={}, .uv={}, .normal={}});
                        }

                        if (i > offset && j > 0)
                        {
                            // 4 seg, 4 rings -> 4 * 4 = 16
                            // i = 2; j = 2
                            // current index = i * segments + j
                            auto v0 = (i - offset) * segments + j - 1;             // previous top
                            auto v1 = (i - offset - 1) * segments + j - 1;         // previous bottom
                            auto v2 = (i - offset) * segments + (j % segments);    // current top
                            auto v3 = (i - offset - 1) * segments + (j % segments);// current bottom

                            mesh.index.push_back(v1);
                            mesh.index.push_back(v3);
                            mesh.index.push_back(v2);

                            mesh.index.push_back(v1);
                            mesh.index.push_back(v2);
                            mesh.index.push_back(v0);
                        }
                    }
                }

                // North pole
                if (!half)
                {
                    mesh.vertex.push_back({.vertex={ 0, radius, 0 }, .color={}, .uv={}, .normal={}});
                    const auto v0 = mesh.vertex.size() - 1;
                    for (unsigned int i = 1; i <= segments; ++i)
                    {
                        auto v1 = i - 1;         // previous top
                        auto v2 = (i % segments);// current top

                        mesh.index.push_back(v1);
                        mesh.index.push_back(v2);
                        mesh.index.push_back(v0);
                    }
                }

                // South pole
                mesh.vertex.push_back({.vertex={ 0, -radius, 0 }, .color={}, .uv={}, .normal={}});
                const auto v0 = mesh.vertex.size() - 1;
                for (unsigned int i = 1; i <= segments; ++i)
                {
                    auto v1 = (rRings - offset - 1) * segments + i - 1;         // previous top
                    auto v2 = (rRings - offset - 1) * segments + (i % segments);// current top

                    mesh.index.push_back(v1);
                    mesh.index.push_back(v2);
                    mesh.index.push_back(v0);
                }


                MCC_LOG_DEBUG(
                    "{} mesh created with {} vertices and {} indices", half ? "Half sphere" : "Sphere",
                    mesh.vertex.size(), mesh.index.size()
                );

                return mesh;
            }
        }

        Mesh GenerateSphereMesh(const float radius, const unsigned int segments, const unsigned int rings)
        {
            return _::GenerateSphereMeshImpl(radius, segments, rings, false);
        }

        Mesh GenerateHalfSphereMesh(const float radius, const unsigned int segments, const unsigned int rings)
        {
            return _::GenerateSphereMeshImpl(radius, segments, rings, true);
        }

        Mesh GenerateCapsuleMesh(const float radius, const unsigned int segments, const unsigned int rings)
        {
            Mesh mesh;

            constexpr auto pi      = glm::pi<float>();
            constexpr auto height  = 2.0f;
            const auto     cHeight = height - 2.0f * radius;
            const auto     cHalf   = cHeight / 2.0f;

            const auto theta = 2.f * pi / static_cast<float>(segments);

            for (unsigned int i = 0; i <= segments; ++i)
            {
                if (i < segments)
                {
                    const float a = static_cast<float>(i) * theta;
                    const float x = radius * std::cos(a);
                    const float z = radius * std::sin(a);
                    mesh.vertex.push_back({.vertex={ x,  cHalf, z }, .color={}, .uv={}, .normal={}});
                    mesh.vertex.push_back({.vertex={ x, -cHalf, z }, .color={}, .uv={}, .normal={}});
                }

                if (i > 0)
                {
                    // 4 seg -> 4 * 2 = 8
                    // i = 4; 4 * 2     % 8 == 0
                    // i = 4; 4 * 2 + 1 % 8 == 1
                    auto v0 = i * 2 - 2;                   // previous top
                    auto v1 = i * 2 - 1;                   // previous bottom
                    auto v2 = (i * 2) % (segments * 2);    // current top
                    auto v3 = (i * 2 + 1) % (segments * 2);// current bottom

                    mesh.index.push_back(v2);
                    mesh.index.push_back(v3);
                    mesh.index.push_back(v1);

                    mesh.index.push_back(v0);
                    mesh.index.push_back(v2);
                    mesh.index.push_back(v1);
                }
            }

            auto&& [hsVertex, hsIndex] = GenerateHalfSphereMesh(radius, segments, rings);

            const auto topTransform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, cHalf, 0.f)) *
                                      glm::rotate(glm::mat4(1.f), pi, glm::vec3(0, 0, 1));
            const auto botTransform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -cHalf, 0.f));

            // Top sphere merging
            auto offset = mesh.vertex.size();
            for (auto& [v, c, u, n]: hsVertex)
            {
                mesh.vertex.push_back({
                    .vertex=topTransform * glm::vec4(v, 1.f), .color=c, .uv=u, .normal=topTransform * glm::vec4(n, 1.f)
                });
            }

            for (const auto i: hsIndex) { mesh.index.push_back(i + offset); }

            // Bottom sphere merging
            offset = mesh.vertex.size();
            for (auto& [v, c, u, n]: hsVertex)
            {
                mesh.vertex.push_back({
                    .vertex=botTransform * glm::vec4(v, 1.f), .color=c, .uv=u, .normal=botTransform * glm::vec4(n, 1.f)
                });
            }

            for (const auto i: hsIndex) { mesh.index.push_back(i + offset); }

            MCC_LOG_DEBUG(
                "Capsule mesh created with {} vertices and {} indices", mesh.vertex.size(), mesh.index.size()
            );
            auto&& optimized = Optimize(mesh);
            MCC_LOG_DEBUG(
                "Capsule mesh optimized with {} vertices and {} indices", optimized.vertex.size(),
                optimized.index.size()
            );

            return optimized;
        }

    }

}
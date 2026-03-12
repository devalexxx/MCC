// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_MESH_H
#define MCC_CLIENT_GRAPHICS_MESH_H

#include <glm/glm.hpp>

#include <vector>

namespace Mcc
{

    struct PackedVertex
    {
        glm::vec3 vertex;
        glm::vec3 color;
        glm::vec3 uv;
        glm::vec3 normal;
    };

    bool operator==(const PackedVertex& lhs, const PackedVertex& rhs);
    bool operator!=(const PackedVertex& lhs, const PackedVertex& rhs);

    struct PackedVertexHasher
    {
        std::size_t operator()(const PackedVertex& pv) const;
    };

    using PackedVertexArray = std::vector<PackedVertex>;
    using IndexArray        = std::vector<unsigned int>;

    struct Mesh
    {
        PackedVertexArray vertex;
        IndexArray        index;
    };

    Mesh Index(const PackedVertexArray& vertices);
    Mesh Optimize(const Mesh& mesh, float epsilonScale = 1.f);

    namespace Helper
    {
        Mesh GenerateSphereMesh(float radius, unsigned int segments, unsigned int rings);
        Mesh GenerateHalfSphereMesh(float radius, unsigned int segments, unsigned int rings);
        Mesh GenerateCapsuleMesh(float radius, unsigned int segments, unsigned int rings);
    }

}

#endif

// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_TERRAIN_RENDERER_COMPONENT_H
#define MCC_CLIENT_MODULE_TERRAIN_RENDERER_COMPONENT_H

#include "Client/Graphics/Mesh.h"
#include "Client/Graphics/OpenGL/OpenGLBuffer.h"
#include "Client/Graphics/OpenGL/OpenGLVertexArray.h"

#include "Common/Utils/FlecsUtils.h"

#include "Hexis/Core/Task.h"

namespace Mcc
{

    struct TShouldBuildMesh   {};
    struct TCouldRenderChunk  {};
    struct TShouldRenderChunk {};

    struct CChunkMesh
    {
        OpenGLVertexArray vertexArray;
        OpenGLBuffer      vertexBuffer { GL_ARRAY_BUFFER };
        OpenGLBuffer      indexBuffer { GL_ELEMENT_ARRAY_BUFFER };
        size_t            indexCount;
    };

    namespace _ { struct TerrainRendererModuleTag {}; }
    using CChunkMeshGenTask = ComponentWrapper<Hx::UniqueTaskRef<Mesh>, _::TerrainRendererModuleTag>;

}

#endif

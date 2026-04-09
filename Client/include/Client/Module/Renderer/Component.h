// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_RENDERER_COMPONENT_H
#define MCC_CLIENT_MODULE_RENDERER_COMPONENT_H

#include "Client/Graphics/OpenGL/OpenGLBuffer.h"
#include "Client/Graphics/OpenGL/OpenGLProgram.h"
#include "Client/Graphics/OpenGL/OpenGLTexture.h"
#include "Client/Graphics/OpenGL/OpenGLVertexArray.h"

#include "Common/Utils/FlecsUtils.h"
#include "Common/World/Transform.h"

namespace Mcc
{

    struct CRendererSettings
    {
        bool wireframe;
    };

    struct COpenGLMesh
    {
        OpenGLVertexArray vertexArray;
        OpenGLBuffer      vertexBuffer { GL_ARRAY_BUFFER };
        OpenGLBuffer      indexBuffer  { GL_ELEMENT_ARRAY_BUFFER };
        size_t            indexCount   { 0 };
    };

    namespace _ { struct RendererModuleTag {}; }
    using CRenderTransform = ComponentWrapper<Transform<FloatCoord>         , _::RendererModuleTag>;
    using COpenGLProgram   = ComponentWrapper<std::shared_ptr<OpenGLProgram>, _::RendererModuleTag>;
    using COpenGLTexture   = ComponentWrapper<std::shared_ptr<OpenGLTexture>, _::RendererModuleTag>;
    using CRenderQueue     = ComponentWrapper<
        std::unordered_map<flecs::entity_t,
            std::unordered_map<flecs::entity_t,
                std::unordered_map<flecs::entity_t, std::unordered_set<flecs::entity_t>>
            >
        >,
        _::RendererModuleTag
    >;

    struct ROpenGLMesh    {};
    struct ROpenGLProgram {};
    struct ROpenGLTexture {};

}

#endif

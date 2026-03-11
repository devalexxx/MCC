// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_RENDERER_COMPONENT_H
#define MCC_CLIENT_MODULE_RENDERER_COMPONENT_H

#include "Client/Graphics/OpenGL/OpenGLBuffer.h"
#include "Client/Graphics/OpenGL/OpenGLProgram.h"
#include "Client/Graphics/OpenGL/OpenGLVertexArray.h"

namespace Mcc
{

    struct COpenGLMesh
    {
        OpenGLVertexArray vertexArray;
        OpenGLBuffer      vertexBuffer { GL_ARRAY_BUFFER };
        OpenGLBuffer      indexBuffer  { GL_ELEMENT_ARRAY_BUFFER };
        size_t            indexCount   { 0 };
    };

    struct COpenGLMaterial
    {
        std::shared_ptr<OpenGLProgram> program;
        std::shared_ptr<OpenGLObject>  textureObject;
    };

}

#endif

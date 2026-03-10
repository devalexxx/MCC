// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_VERTEX_ARRAY_H
#define MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_VERTEX_ARRAY_H

#include "glad/glad.h"

#include "OpenGLObject.h"

namespace Mcc
{

    class OpenGLVertexArray final : public OpenGLObject
    {
      public:
        static bool IsThereAnyBound();

        OpenGLVertexArray();
        ~OpenGLVertexArray() override;
        OpenGLVertexArray(const OpenGLVertexArray&)            = delete;
        OpenGLVertexArray& operator=(const OpenGLVertexArray&) = delete;
        OpenGLVertexArray(OpenGLVertexArray&&)            noexcept;
        OpenGLVertexArray& operator=(OpenGLVertexArray&&) noexcept;

        void Create() override;
        void Delete() override;

        void Bind   () const override;
        bool IsValid() const override;

      private:
        GLuint mObjectID;

        static GLuint sBoundVertexArray;
    };

}

#endif

// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_BUFFER_H
#define MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_BUFFER_H

#include <glad/glad.h>

#include <span>

#include "OpenGLObject.h"

namespace Mcc
{

    class OpenGLBuffer final : public OpenGLObject
    {
      public:
        explicit OpenGLBuffer(GLenum target);
        ~OpenGLBuffer() override;
        OpenGLBuffer(const OpenGLBuffer&)            = delete;
        OpenGLBuffer& operator=(const OpenGLBuffer&) = delete;
        OpenGLBuffer(OpenGLBuffer&&)            noexcept;
        OpenGLBuffer& operator=(OpenGLBuffer&&) noexcept;

        void Create() override;
        void Delete() override;

        bool IsValid() const override;
        void Bind   () const override;

        void SetData(GLsizeiptr size, const void* data, GLenum usage) const;
        template<typename T>
        void SetData(std::span<T> data, GLenum usage);

      private:
        GLuint mObjectID;
        GLenum mTarget;

        static GLuint sBoundBuffer;
    };

}

#include <Client/Graphics/OpenGL/OpenGLBuffer.inl>

#endif

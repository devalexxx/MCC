// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_BUFFER_H
#define MCC_CLIENT_GRAPHICS_BUFFER_H

#include <glad/glad.h>

#include <span>

namespace Mcc
{

    class Buffer
    {
      private:
        static GLuint sBoundBuffer;

      public:
        explicit Buffer(GLenum target);
        ~Buffer();
        Buffer(const Buffer&)            = delete;
        Buffer& operator=(const Buffer&) = delete;
        Buffer(Buffer&&) noexcept;
        Buffer& operator=(Buffer&&) noexcept;

        [[nodiscard]] bool IsValid() const;

        void Create();
        void Delete();
        void Bind() const;

        void SetData(GLsizeiptr size, const void* data, GLenum usage) const;
        template<typename T>
        void SetData(std::span<T> data, GLenum usage);

      private:
        GLuint mId;
        GLenum mTarget;
    };

}

#include <Client/Graphics/Buffer.inl>

#endif

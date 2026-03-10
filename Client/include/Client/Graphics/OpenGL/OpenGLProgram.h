// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_PROGRAM_H
#define MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_PROGRAM_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "OpenGLObject.h"

namespace Mcc
{

    class OpenGLShader;

    class OpenGLProgram final : public OpenGLObject
    {
      public:
        OpenGLProgram();
        ~OpenGLProgram() override;
        OpenGLProgram(const OpenGLProgram&)            = delete;
        OpenGLProgram& operator=(const OpenGLProgram&) = delete;
        OpenGLProgram(OpenGLProgram&&)            noexcept;
        OpenGLProgram& operator=(OpenGLProgram&&) noexcept;

        void Create() override;
        void Delete() override;

        bool IsValid() const override;
        void Bind   () const override;

        void Attach(const OpenGLShader& shader) const;
        void Detach(const OpenGLShader& shader) const;
        void Link() const;

        GLint GetAttributeLocation(const char* name) const;
        GLint GetUniformLocation(const char* name) const;

        template<glm::length_t R, glm::length_t C, glm::qualifier Q>
        void SetUniformMatrix(GLint location, const glm::mat<R, C, float, Q>& matrix);

        template<glm::length_t L, glm::qualifier Q>
        void SetUniformVector(GLint location, const glm::vec<L, float, Q>& vec);

        void SetVertexAttribPointer(GLuint location, GLint size, GLenum type, GLsizei stride, GLsizei offset) const;
        void SetVertexAttribPointer(const char* name, GLint size, GLenum type, GLsizei stride, GLsizei offset) const;
        void SetVertexAttribPointer(GLuint location, GLint size, GLenum type) const;
        void SetVertexAttribPointer(const char* name, GLint size, GLenum type) const;

      private:
        void HandleError() const;

        GLuint mObjectID;

        static GLuint sUsedProgram;
    };

}

#include <Client/Graphics/OpenGL/OpenGLProgram.inl>

#endif

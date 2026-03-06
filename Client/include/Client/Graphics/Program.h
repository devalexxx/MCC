// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_PROGRAM_H
#define MCC_CLIENT_GRAPHICS_PROGRAM_H

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Mcc
{

    class Shader;

    class Program
    {
      private:
        static GLuint sUsedProgram;

      public:
        Program();
        ~Program();
        Program(const Program&)            = delete;
        Program& operator=(const Program&) = delete;
        Program(Program&&) noexcept;
        Program& operator=(Program&&) noexcept;

        void Create();
        void Delete();

        void Attach(const Shader& shader) const;
        void Detach(const Shader& shader) const;
        void Link() const;
        void Use() const;

        [[nodiscard]] bool IsValid() const;

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

        GLuint mId;
    };

}

#include <Client/Graphics/Program.inl>

#endif

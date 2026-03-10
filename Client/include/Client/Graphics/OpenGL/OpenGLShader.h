// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_SHADER_H
#define MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_SHADER_H

#include <glad/glad.h>

#include "OpenGLObject.h"

namespace Mcc
{

    class OpenGLShader final : public OpenGLObject
    {
      public:
        OpenGLShader(GLenum kind, const char* code);
        ~OpenGLShader() override;
        OpenGLShader(const OpenGLShader&)            = delete;
        OpenGLShader& operator=(const OpenGLShader&) = delete;
        OpenGLShader(OpenGLShader&&)            noexcept;
        OpenGLShader& operator=(OpenGLShader&&) noexcept;

        void Create() override;
        void Delete() override;

        void Bind   () const override;
        bool IsValid() const override;

      private:
        void HandleError() const;

        GLuint mObjectID;
        GLenum mKind;
        const char* mCode;

        friend class OpenGLProgram;
    };

}

#endif

// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Graphics/Program.h"

#include "Client/Graphics/Shader.h"

#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

#include <string_view>
#include <vector>

namespace Mcc
{

    GLuint Program::sUsedProgram = 0;

    Program::Program() : mId(0) {}

    Program::~Program()
    {
        Delete();
    }

    Program::Program(Program&& other) noexcept : mId(other.mId)
    {
        other.mId = 0;
    }

    Program& Program::operator=(Program&& other) noexcept
    {
        if (this != &other)
        {
            if (IsValid())
            {
                this->~Program();
            }

            mId = other.mId;

            other.mId = 0;
        }
        return *this;
    }

    void Program::Create()
    {
        glCheck(mId = glCreateProgram());
    }

    void Program::Delete()
    {
        if (IsValid())
        {
            glCheck(glDeleteProgram(mId));
            mId = 0;
        }
    }

    void Program::Attach(const Shader& shader) const
    {
        glCheck(glAttachShader(mId, shader.mId));
    }

    void Program::Detach(const Shader& shader) const
    {
        glCheck(glDetachShader(mId, shader.mId));
    }

    void Program::Link() const
    {
        glCheck(glLinkProgram(mId));
        HandleError();
    }

    void Program::Use() const
    {
        if (sUsedProgram != mId)
        {
            glCheck(glUseProgram(mId));
            sUsedProgram = mId;
        }
    }
    bool Program::IsValid() const
    {
        bool result;
        glCheck(result = glIsProgram(mId) == GL_TRUE);
        return result;
    }

    GLint Program::GetAttributeLocation(const char* name) const
    {
        GLint location;
        glCheck(location = glGetAttribLocation(mId, name));
        return location;
    }

    GLint Program::GetUniformLocation(const char* name) const
    {
        GLint location;
        glCheck(location = glGetUniformLocation(mId, name));
        return location;
    }

    void Program::SetVertexAttribPointer(GLuint location, GLint size, GLenum type, GLsizei stride, GLsizei offset) const
    {
        Use();
        glCheck(glEnableVertexAttribArray(location));
        glCheck(glVertexAttribPointer(location, size, type, GL_FALSE, stride, (void*) ((intptr_t) offset)));
    }

    void
    Program::SetVertexAttribPointer(const char* name, GLint size, GLenum type, GLsizei stride, GLsizei offset) const
    {
        SetVertexAttribPointer(GetAttributeLocation(name), size, type, stride, offset);
    }

    void Program::SetVertexAttribPointer(GLuint location, GLint size, GLenum type) const
    {
        SetVertexAttribPointer(location, size, type, 0, 0);
    }

    void Program::SetVertexAttribPointer(const char* name, GLint size, GLenum type) const
    {
        SetVertexAttribPointer(GetAttributeLocation(name), size, type);
    }

    void Program::HandleError() const
    {
        GLint result = GL_FALSE;
        GLint length;
        glCheck(glGetProgramiv(mId, GL_LINK_STATUS, &result));
        if (result == GL_FALSE)
        {
            glCheck(glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &length));
            std::vector<char> message(length);
            glCheck(glGetProgramInfoLog(mId, length, &length, message.data()));
            MCC_LOG_ERROR("Failed to link program {}:\n\t{}", mId, std::string_view(message.begin(), message.end()));
        }
    }

}
// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Graphics/OpenGL/OpenGLProgram.h"

#include "Client/Graphics/OpenGL/OpenGLShader.h"

#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

#include <string_view>
#include <vector>

namespace Mcc
{

    GLuint OpenGLProgram::sUsedProgram = 0;

    OpenGLProgram::OpenGLProgram() : mObjectID(0) {}

    OpenGLProgram::~OpenGLProgram()
    {
        Delete();
    }

    OpenGLProgram::OpenGLProgram(OpenGLProgram&& other) noexcept : mObjectID(other.mObjectID)
    {
        other.mObjectID = 0;
    }

    OpenGLProgram& OpenGLProgram::operator=(OpenGLProgram&& other) noexcept
    {
        if (this != &other)
        {
            if (IsValid())
            {
                this->~OpenGLProgram();
            }

            mObjectID = other.mObjectID;

            other.mObjectID = 0;
        }
        return *this;
    }

    void OpenGLProgram::Create()
    {
        glCheck(mObjectID = glCreateProgram());
    }

    void OpenGLProgram::Delete()
    {
        if (IsValid())
        {
            glCheck(glDeleteProgram(mObjectID));
            mObjectID = 0;
        }
    }

    void OpenGLProgram::Attach(const OpenGLShader& shader) const
    {
        glCheck(glAttachShader(mObjectID, shader.mObjectID));
    }

    void OpenGLProgram::Detach(const OpenGLShader& shader) const
    {
        glCheck(glDetachShader(mObjectID, shader.mObjectID));
    }

    void OpenGLProgram::Link() const
    {
        glCheck(glLinkProgram(mObjectID));
        HandleError();
    }

    void OpenGLProgram::Bind() const
    {
        if (sUsedProgram != mObjectID)
        {
            glCheck(glUseProgram(mObjectID));
            sUsedProgram = mObjectID;
        }
    }
    bool OpenGLProgram::IsValid() const
    {
        bool result;
        glCheck(result = glIsProgram(mObjectID) == GL_TRUE);
        return result;
    }

    GLint OpenGLProgram::GetAttributeLocation(const char* name) const
    {
        GLint location;
        glCheck(location = glGetAttribLocation(mObjectID, name));
        return location;
    }

    GLint OpenGLProgram::GetUniformLocation(const char* name) const
    {
        GLint location;
        glCheck(location = glGetUniformLocation(mObjectID, name));
        return location;
    }

    void OpenGLProgram::SetUniform(const GLint location, const int value) const
    {
        Bind();
        glCheck(glUniform1i(location, value));
    }

    void OpenGLProgram::SetUniform(const GLint location, const float value) const
    {
        Bind();
        glCheck(glUniform1f(location, value));
    }

    void OpenGLProgram::SetVertexAttribPointer(GLuint location, GLint size, GLenum type, GLsizei stride, GLsizei offset) const
    {
        Bind();
        glCheck(glEnableVertexAttribArray(location));
        glCheck(glVertexAttribPointer(location, size, type, GL_FALSE, stride, (void*) ((intptr_t) offset)));
    }

    void
    OpenGLProgram::SetVertexAttribPointer(const char* name, GLint size, GLenum type, GLsizei stride, GLsizei offset) const
    {
        SetVertexAttribPointer(GetAttributeLocation(name), size, type, stride, offset);
    }

    void OpenGLProgram::SetVertexAttribPointer(GLuint location, GLint size, GLenum type) const
    {
        SetVertexAttribPointer(location, size, type, 0, 0);
    }

    void OpenGLProgram::SetVertexAttribPointer(const char* name, GLint size, GLenum type) const
    {
        SetVertexAttribPointer(GetAttributeLocation(name), size, type);
    }

    void OpenGLProgram::HandleError() const
    {
        GLint result = GL_FALSE;
        GLint length;
        glCheck(glGetProgramiv(mObjectID, GL_LINK_STATUS, &result));
        if (result == GL_FALSE)
        {
            glCheck(glGetProgramiv(mObjectID, GL_INFO_LOG_LENGTH, &length));
            std::vector<char> message(length);
            glCheck(glGetProgramInfoLog(mObjectID, length, &length, message.data()));
            MCC_LOG_ERROR("Failed to link program {}:\n\t{}", mObjectID, std::string_view(message.begin(), message.end()));
        }
    }

}
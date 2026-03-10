// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Graphics/OpenGL/OpenGLShader.h"

#include "Client/Graphics/OpenGL/OpenGLCommon.h"

#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

#include <fmt/core.h>

#include <vector>

namespace Mcc
{

    OpenGLShader::OpenGLShader(const GLenum kind, const char* code) : mObjectID(0), mKind(kind), mCode(code) {}

    OpenGLShader::~OpenGLShader()
    {
        Delete();
    }

    OpenGLShader::OpenGLShader(OpenGLShader&& other) noexcept : mObjectID(other.mObjectID), mKind(other.mKind), mCode(other.mCode)
    {
        other.mObjectID = 0;
    }

    OpenGLShader& OpenGLShader::operator=(OpenGLShader&& other) noexcept
    {
        if (this != &other)
        {
            if (IsValid())
            {
                this->~OpenGLShader();
            }

            mObjectID = other.mObjectID;
            mKind     = other.mKind;
            mCode     = other.mCode;

            other.mObjectID = 0;
        }

        return *this;
    }

    void OpenGLShader::Create()
    {
        glCheck(mObjectID = glCreateShader(mKind));
        MCC_ASSERT(mObjectID, "OpenGLShader creation failed");

        glCheck(glShaderSource(mObjectID, 1, &mCode, nullptr));
        glCheck(glCompileShader(mObjectID));
        HandleError();
    }

    void OpenGLShader::Delete()
    {
        if (IsValid())
        {
            glCheck(glDeleteShader(mObjectID));
            mObjectID = 0;
        }
    }

    void OpenGLShader::Bind() const
    {
        MCC_ASSERT(true, "OpenGLShader isn't bindable");
    }

    bool OpenGLShader::IsValid() const
    {
        bool result;
        glCheck(result = glIsShader(mObjectID) == GL_TRUE);
        return result;
    }

    void OpenGLShader::HandleError() const
    {
        GLint result = GL_FALSE;
        GLint length;
        glCheck(glGetShaderiv(mObjectID, GL_COMPILE_STATUS, &result));
        if (result == GL_FALSE)
        {
            glCheck(glGetShaderiv(mObjectID, GL_INFO_LOG_LENGTH, &length));
            std::vector<char> message(length);
            glCheck(glGetShaderInfoLog(mObjectID, length, &length, message.data()));
            MCC_LOG_ERROR("Failed to compile shader {}:\n\t{}", mObjectID, std::string_view(message.begin(), message.end()));
        }
    }

}
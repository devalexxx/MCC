// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Graphics/OpenGL/OpenGLTexture.h"

#include "Client/Graphics/OpenGL/OpenGLCommon.h"

#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

#include <Hexis/Core/EnumArray.h>

namespace Mcc
{

    void OpenGLTexture::SetActiveTexture(const uint32_t slot)
    {
        glCheck(glActiveTexture(GL_TEXTURE0 + slot));
    }

    OpenGLTexture::OpenGLTexture(const GLenum target) : mObjectID(0), mTarget(target) {}

    OpenGLTexture::~OpenGLTexture()
    {
        Delete();
    }

    OpenGLTexture::OpenGLTexture(OpenGLTexture&& other) noexcept : mObjectID(other.mObjectID), mTarget(other.mTarget)
    {
        other.mObjectID = 0;
    }

    OpenGLTexture& OpenGLTexture::operator=(OpenGLTexture&& other) noexcept
    {
        if (this != &other)
        {
            this->~OpenGLTexture();

            mObjectID = other.mObjectID;
            mTarget   = other.mTarget;

            other.mObjectID = 0;
        }

        return *this;
    }

    void OpenGLTexture::Create()
    {
        glCheck(glGenTextures(1, &mObjectID));
        glCheck(glBindTexture(mTarget, mObjectID));
        MCC_ASSERT(IsValid(), "OpenGLTexture creation failed");
    }

    void OpenGLTexture::Delete()
    {
        if (IsValid())
        {
            glCheck(glDeleteTextures(1, &mObjectID));
            mObjectID = 0;
        }
    }

    bool OpenGLTexture::IsValid() const
    {
        bool result = false;
        glCheck(result = glIsTexture(mObjectID) == GL_TRUE);
        return result;
    }

    void OpenGLTexture::Bind() const
    {
        MCC_ASSERT(IsValid(), "A Texture must be valid (eg. created) to be bound");
        if (auto& map = GetBoundingMap(); map[mTarget] != mObjectID)
        {
            glCheck(glBindTexture(mTarget, mObjectID));
            map[mTarget] = mObjectID;
        }
    }

    void OpenGLTexture::GenerateMipmap() const
    {
        Bind();
        glCheck(glGenerateMipmap(mTarget));
    }

    void OpenGLTexture::SetParameter(const GLenum pName, const GLint param) const
    {
        Bind();
        glCheck(glTexParameteri(mTarget, pName, param));
    }

    void OpenGLTexture::SetParameter(const GLenum pName, const GLfloat param) const
    {
        glCheck(glTexParameterf(mTarget, pName, param));
    }

    GLint OpenGLTexture::GetParameter(const GLenum pName, int) const
    {
        GLint result;
        glCheck(glGetTexParameteriv(mTarget, pName, &result));
        return result;
    }

    GLfloat OpenGLTexture::GetParameter(const GLenum pName, float) const
    {
        GLfloat result;
        glCheck(glGetTexParameterfv(mTarget, pName, &result));
        return result;
    }

    GLint OpenGLTexture::GetLevelParameter(const GLint level, const GLenum pName, int) const
    {
        GLint result;
        glCheck(glGetTexLevelParameteriv(mTarget, level, pName, &result));
        return result;
    }

    GLfloat OpenGLTexture::GetLevelParameter(const GLint level, const GLenum pName, float) const
    {
        GLfloat result;
        glCheck(glGetTexLevelParameterfv(mTarget, level, pName, &result));
        return result;
    }

    std::unordered_map<GLenum, GLuint>& OpenGLTexture::GetBoundingMap()
    {
        static std::unordered_map<GLenum, GLuint> boundMap = {};
        return boundMap;
    }

    const std::vector<std::pair<GLenum, GLint>>& OpenGLTexture::GetFormatInternalFormatMapping()
    {
        static std::vector<std::pair<GLenum, GLint>> formatMap = {
            { GL_RED , GL_RED  },
            { GL_RG  , GL_RG   },
            { GL_RGB , GL_RGB  },
            { GL_RGB, GL_INVALID_ENUM },
            { GL_RGB, GL_RGB4 },
            { GL_RGB, GL_RGB8 },
            { GL_RGB, GL_RGB12 },
            { GL_RGB, GL_RGB16 },
            { GL_RGBA, GL_RGBA },
            { GL_RGBA, GL_RGBA2 },
            { GL_RGBA, GL_RGBA4 },
            { GL_RGBA, GL_RGBA8 },
            { GL_RGBA, GL_RGBA12 },
            { GL_RGBA, GL_RGBA16 },
        };

        return formatMap;
    }

    GLenum OpenGLTexture::PixelFormatToFormat(const PixelFormat format)
    {
        switch (format)
        {
            case PixelFormat::GREY:
                return GL_RED;
            case PixelFormat::GREY_A:
                return GL_RG;
            case PixelFormat::RGB:
                return GL_RGB;
            case PixelFormat::RGBA:
                return GL_RGBA;
            case PixelFormat::Count:
                MCC_ASSERT(false, "PixelFormat::Count is not a valid PixelFormat");
        }
        return GL_INVALID_ENUM;
    }

    GLenum OpenGLTexture::InternalFormatToFormat(const GLint internalFormat)
    {
        static std::unordered_map<GLint, GLenum> formatMap = ([](const std::vector<std::pair<GLenum, GLint>>& v) {
            std::unordered_map<GLint, GLenum> map{};
            for (const auto& [fmt, iFmt] : v)
            {
                map.emplace(iFmt, fmt);
            }
            return map;
        })(GetFormatInternalFormatMapping());

        return formatMap[internalFormat];
    }

    GLint OpenGLTexture::FormatToInternalFormat(const GLenum format, const int componentSize)
    {
        static std::unordered_map<GLenum, std::vector<GLint>> formatMap = ([](const std::vector<std::pair<GLenum, GLint>>& v) {
            std::unordered_map<GLenum, std::vector<GLint>> map{};
            for (const auto& [fmt, iFmt] : v)
            {
                if (auto [it, inserted] = map.try_emplace(fmt, std::vector { iFmt }); !inserted)
                {
                    it->second.push_back(iFmt);
                }
            }
            return map;
        })(GetFormatInternalFormatMapping());

        static std::unordered_map<int, size_t> componentSizeMap = {
            {-1, 0}, { 2, 1 }, { 4, 2 }, { 8, 3 }, { 16, 4 }
        };

        if (const auto it = componentSizeMap.find(componentSize); it != componentSizeMap.end())
        {
            if (const auto it2 = formatMap.find(format); it2 != formatMap.end() && it2->second.size() > it->second)
            {
                return it2->second[it->second];
            }
        }

        MCC_LOG_WARN("Invalid format or componentSize for {} {}", format, componentSize);
        return GL_INVALID_ENUM;
    }

    size_t OpenGLTexture::GetComponentCount(const GLenum format)
    {
        switch (format)
        {
            case GL_RED:
                return 1;
            case GL_RG:
                return 2;
            case GL_RGB:
                return 3;
            case GL_RGBA:
                return 4;
            default:
                MCC_ASSERT(false, "Not implemented format");
        }
    }

    void OpenGLTexture::EnsureImageFormat(const Image& image)
    {
        MCC_ASSERT(image.GetFormat() != PixelFormat::GREY  , "PixelFormat::GREY isn't supported");
        MCC_ASSERT(image.GetFormat() != PixelFormat::GREY_A, "PixelFormat::GREY_A isn't supported");
    }
}

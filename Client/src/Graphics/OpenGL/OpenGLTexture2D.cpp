// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Graphics/OpenGL/OpenGLTexture2D.h"

#include "Client/Graphics/OpenGL/OpenGLCommon.h"

#include "Common/Utils/Assert.h"

namespace Mcc
{

    OpenGLTexture2D::OpenGLTexture2D() : OpenGLTexture(GL_TEXTURE_2D) {}

    void OpenGLTexture2D::SetData(const Image& image, const int level) const
    {
        EnsureImageFormat(image);

        const auto fmt  = PixelFormatToFormat(image.GetFormat());
        const auto iFmt = FormatToInternalFormat(fmt);
        SetData(level, iFmt, image.GetWidth(), image.GetHeight(),fmt, GL_UNSIGNED_BYTE, image.GetData().data());
    }

    void OpenGLTexture2D::SetData(
        const int level, const int internalFormat, const uint32_t width, const uint32_t height,
        const GLenum format, const GLenum type, const void* data
    ) const
    {
        Bind();
        glCheck(glTexImage2D(mTarget, level, internalFormat, width, height, 0, format, type, data));
    }

}

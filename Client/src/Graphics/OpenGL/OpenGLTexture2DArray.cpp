// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Graphics/OpenGL/OpenGLTexture2DArray.h"

#include "Client/Graphics/OpenGL/OpenGLCommon.h"

#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    OpenGLTexture2DArray::OpenGLTexture2DArray() : OpenGLTexture(GL_TEXTURE_2D_ARRAY) {}

    void OpenGLTexture2DArray::SetData(
        const int level, const int internalFormat, const uint32_t width, const uint32_t height, const uint32_t depth,
        const GLenum format, const GLenum type, const void* data
    ) const
    {
        Bind();
        glCheck(glTexImage3D(mTarget, level, internalFormat, width, height, depth, 0, format, type, data));
    }

    void OpenGLTexture2DArray::SetData(const std::span<const Image> images, const int level) const
    {
        MCC_ASSERT(!images.empty(), "Cannot set an empty array of images");

        Bind();
        const auto& first   = images[0];
        const auto  pFormat = PixelFormatToFormat(first.GetFormat());
        const auto  iFormat = FormatToInternalFormat(pFormat, 8);

        SetData(level, iFormat, first.GetWidth(), first.GetHeight(), images.size(), pFormat, GL_UNSIGNED_BYTE, nullptr);
        for (size_t i = 0; i < images.size(); ++i)
        {
            UpdateData(images[i], i, level);
        }
    }

    void OpenGLTexture2DArray::SetData(const Image& image, const int level) const
    {
        SetData(std::span(&image, 1), level);
    }

    void OpenGLTexture2DArray::UpdateData(
        const int level, const uint32_t xOffset, const uint32_t yOffset, const uint32_t zOffset, const uint32_t width,
        const uint32_t height, const uint32_t depth, const GLenum format, const GLenum type, const void* data
    ) const
    {
        Bind();
        glCheck(glTexSubImage3D(mTarget, level, xOffset, yOffset, zOffset, width, height, depth, format, type, data));
    }

    void OpenGLTexture2DArray::UpdateData(const Image& image, const uint32_t index, const int level) const
    {
        Bind();
        EnsureImageFormat(image);

        MCC_ASSERT(
            static_cast<uint32_t>(GetLevelParameter(level, GL_TEXTURE_WIDTH , 0)) == image.GetWidth() &&
            static_cast<uint32_t>(GetLevelParameter(level, GL_TEXTURE_HEIGHT, 0)) == image.GetHeight(),
            "All images must have the same size"
        );
        MCC_ASSERT(
            GetLevelParameter(level, GL_TEXTURE_DEPTH , 0) > static_cast<GLint>(index),
            "Array index must be less than the depth of the texture"
        );

        const auto fmt = PixelFormatToFormat(image.GetFormat());
        UpdateData(
            level, 0, 0, index, image.GetWidth(), image.GetHeight(),
            1, fmt, GL_UNSIGNED_BYTE, image.GetData().data()
        );
    }

    void OpenGLTexture2DArray::UpdateData(
        const std::span<const Image> images, const uint32_t offset, const int level
    ) const
    {
        for (size_t i = 0; i < images.size(); ++i)
        {
            UpdateData(images[i], offset + i, level);
        }
    }

    void OpenGLTexture2DArray::AddData(const std::span<const Image> images) const
    {
        Bind();

        if (GetLevelParameter(0, GL_TEXTURE_DEPTH, 0) == 0)
        {
            SetData(images);
        }

        const auto w = GetLevelParameter(0, GL_TEXTURE_WIDTH , 0);
        const auto h = GetLevelParameter(0, GL_TEXTURE_HEIGHT, 0);
        const auto d = GetLevelParameter(0, GL_TEXTURE_DEPTH , 0);

        const auto iFormat = GetLevelParameter(0, GL_TEXTURE_INTERNAL_FORMAT , 0);
        const auto pFormat = InternalFormatToFormat(iFormat);
        const auto cCount  = GetComponentCount(pFormat);

        std::vector<uint8_t> buffer(w * h * d * cCount, 0);
        glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());

        SetData(0, iFormat, w, h, d + images.size(), pFormat, GL_UNSIGNED_BYTE, nullptr);

        UpdateData(0, 0, 0, 0, w, h, d, pFormat, GL_UNSIGNED_BYTE, buffer.data());
        UpdateData(images, d, 0);
    }

    void OpenGLTexture2DArray::AddData(const Image& image) const
    {
        AddData(std::span(&image, 1));
    }

}

// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_TEXTURE2D_ARRAY_H
#define MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_TEXTURE2D_ARRAY_H

#include "Client/Graphics/OpenGL/OpenGLTexture.h"

namespace Mcc
{

    struct OpenGLTexture2DArray final : OpenGLTexture
    {
        OpenGLTexture2DArray();

        void SetData(
            int level, int internalFormat, uint32_t width, uint32_t height, uint32_t depth, GLenum format, GLenum type,
            const void* data
        ) const;
        void SetData(std::span<const Image> images, int level = 0) const;
        void SetData(const Image& image, int level = 0) const;

        void UpdateData(
            int level, uint32_t xOffset, uint32_t yOffset, uint32_t zOffset, uint32_t width, uint32_t height,
            uint32_t depth, GLenum format, GLenum type, const void* data
        ) const;

        void UpdateData(const Image& image, uint32_t index, int level = 0) const;
        void UpdateData(std::span<const Image> images, uint32_t offset, int level = 0) const;

        void AddData(std::span<const Image> images) const;
        void AddData(const Image& image) const;
    };

}

#endif

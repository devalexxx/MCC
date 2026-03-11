// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_TEXTURE2D_H
#define MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_TEXTURE2D_H

#include "Client/Graphics/OpenGL/OpenGLTexture.h"

namespace Mcc
{

    struct OpenGLTexture2D final : OpenGLTexture
    {
        OpenGLTexture2D();

        void SetData(const Image& image, int level = 0) const;
        void SetData(
            int level, int internalFormat, uint32_t width, uint32_t height, GLenum format, GLenum type, const void* data
        ) const;
    };

}

#endif

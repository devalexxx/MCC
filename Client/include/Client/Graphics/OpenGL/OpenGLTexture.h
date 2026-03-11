// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_TEXTURE_H
#define MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_TEXTURE_H

#include "Client/Graphics/Image.h"
#include "Client/Graphics/OpenGL/OpenGLObject.h"

#include <glad/glad.h>

namespace Mcc
{

    class OpenGLTexture : public OpenGLObject
    {
      public:
        static void SetActiveTexture(uint32_t slot);

        OpenGLTexture(GLenum target);
        ~OpenGLTexture() override;
        OpenGLTexture(const OpenGLTexture&)            = delete;
        OpenGLTexture& operator=(const OpenGLTexture&) = delete;
        OpenGLTexture(OpenGLTexture&&)            noexcept;
        OpenGLTexture& operator=(OpenGLTexture&&) noexcept;

        void Create() override;
        void Delete() final;

        bool IsValid() const override;
        void Bind()    const override;

        void GenerateMipmap() const;

        void SetParameter(GLenum pName, GLint   param) const;
        void SetParameter(GLenum pName, GLfloat param) const;

        GLint   GetParameter(GLenum pName, int  ) const;
        GLfloat GetParameter(GLenum pName, float) const;

        GLint   GetLevelParameter(GLint level, GLenum pName, int  ) const;
        GLfloat GetLevelParameter(GLint level, GLenum pName, float) const;

      protected:
        GLuint mObjectID;
        GLenum mTarget;

        static       std::unordered_map<GLenum, GLuint>&    GetBoundingMap();
        static const std::vector<std::pair<GLenum, GLint>>& GetFormatInternalFormatMapping();

        static GLenum PixelFormatToFormat   (PixelFormat format);
        static GLenum InternalFormatToFormat(GLint internalFormat);
        static GLint  FormatToInternalFormat(GLenum format, int componentSize = -1);
        static size_t GetComponentCount     (GLenum format);

        static void EnsureImageFormat(const Image& image);
    };

}

#endif

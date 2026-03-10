// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_OBJECT_H
#define MCC_CLIENT_GRAPHICS_OPENGL_OPENGL_OBJECT_H

namespace Mcc
{

    struct OpenGLObject
    {
        virtual ~OpenGLObject() = default;

        virtual void Create() = 0;
        virtual void Delete() = 0;

        virtual void Bind   () const = 0;
        virtual bool IsValid() const = 0;
    };

}

#endif

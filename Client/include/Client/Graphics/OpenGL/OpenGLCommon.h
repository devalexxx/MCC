// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_OPENGL_COMMON_H
#define MCC_CLIENT_GRAPHICS_OPENGL_COMMON_H

namespace Mcc
{
    void GlCheckError(const char* file, unsigned int line, const char* expression);
}

#ifdef MCC_DEBUG
#    define glCheck(expr)                                 \
        do {                                              \
            expr;                                         \
            Mcc::GlCheckError(__FILE__, __LINE__, #expr); \
        } while (false)
#else
#    define glCheck(expr) (expr)
#endif

#endif

// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Graphics/OpenGL/OpenGLCommon.h"

#include "Common/Utils/Logging.h"

#include <fmt/core.h>
#include <glad/glad.h>

namespace Mcc
{

    void GlCheckError(const char* file, unsigned int line, [[maybe_unused]] const char* expression)
    {
        const GLenum error_code = glGetError();

        if (error_code != GL_NO_ERROR)
        {
#ifdef MCC_LOG_FULL
            const char* error       = "Unknown error";
            const char* description = "No description";

            switch (error_code)
            {
                case GL_INVALID_ENUM:
                    error       = "GL_INVALID_ENUM";
                    description = "An unacceptable value has been specified for an enumerated argument.";
                    break;
                case GL_INVALID_VALUE:
                    error       = "GL_INVALID_VALUE";
                    description = "A numeric argument is out of range.";
                    break;
                case GL_INVALID_OPERATION:
                    error       = "GL_INVALID_OPERATION";
                    description = "The specified operation is not allowed in the current state.";
                    break;
                case GL_STACK_OVERFLOW:
                    error       = "GL_STACK_OVERFLOW";
                    description = "This command would cause a stack overflow.";
                    break;
                case GL_OUT_OF_MEMORY:
                    error       = "GL_OUT_OF_MEMORY";
                    description = "There is not enough memory left to execute the command.";
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    error       = "GL_INVALID_FRAMEBUFFER_OPERATION";
                    description = "The object bound to FRAMEBUFFER_BINDING is not \"framebuffer complete\".";
                    break;
                default:
                    break;
            }

            MCC_LOG_ERROR(
                "An internal OpenGL call failed in {}, ({}).\nExpression:\n\t{}\nError description:\n\t{}\n\t{}\n",
                file, line, expression, error, description
            );
#else
            MCC_LOG_ERROR("An internal OpenGL call failed in {}, ({}).", file, line);
#endif
        }
    }

}
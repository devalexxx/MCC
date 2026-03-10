// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Graphics/OpenGL/OpenGLCommon.h"
#include "Client/Graphics/OpenGL/OpenGLVertexArray.h"

#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    GLuint OpenGLVertexArray::sBoundVertexArray = 0;

    bool OpenGLVertexArray::IsThereAnyBound()
    {
        return sBoundVertexArray != 0;
    }

    OpenGLVertexArray::OpenGLVertexArray() : mObjectID(0)
    {}

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        Delete();
    }

    OpenGLVertexArray::OpenGLVertexArray(OpenGLVertexArray&& other) noexcept : mObjectID(other.mObjectID)
    {
        other.mObjectID = 0;
    }

    OpenGLVertexArray& OpenGLVertexArray::operator=(OpenGLVertexArray&& other) noexcept
    {
        if (this != &other)
        {
            if (IsValid())
            {
                this->~OpenGLVertexArray();
            }

            mObjectID       = other.mObjectID;
            other.mObjectID = 0;
        }
        return *this;
    }

    bool OpenGLVertexArray::IsValid() const
    {
        bool result;
        glCheck(result = glIsVertexArray(mObjectID) == GL_TRUE);
        return result;
    }

    void OpenGLVertexArray::Create()
    {
        glCheck(glGenVertexArrays(1, &mObjectID));
        glBindVertexArray(mObjectID);
        MCC_ASSERT(IsValid(), "VertexArray creation failed");
    }

    void OpenGLVertexArray::Delete()
    {
        if (IsValid())
        {
            glCheck(glDeleteVertexArrays(1, &mObjectID));
            mObjectID = 0;
        }
    }

    void OpenGLVertexArray::Bind() const
    {
        MCC_ASSERT(IsValid(), "A VertexArray must be valid (eg. created) to be bound");
        if (sBoundVertexArray != mObjectID)
        {
            glCheck(glBindVertexArray(mObjectID));
            sBoundVertexArray = mObjectID;
        }
    }

}
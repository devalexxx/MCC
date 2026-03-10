// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Graphics/OpenGL/OpenGLBuffer.h"

#include "Client/Graphics/OpenGL/OpenGLCommon.h"
#include "Client/Graphics/OpenGL/OpenGLVertexArray.h"

#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    GLuint OpenGLBuffer::sBoundBuffer = 0;

    OpenGLBuffer::OpenGLBuffer(const GLenum target) : mObjectID(0), mTarget(target)
    {}

    OpenGLBuffer::~OpenGLBuffer()
    {
        Delete();
    }

    OpenGLBuffer::OpenGLBuffer(OpenGLBuffer&& other) noexcept : mObjectID(other.mObjectID), mTarget(other.mTarget)
    {
        other.mObjectID = 0;
        other.mTarget   = 0;
    }

    OpenGLBuffer& OpenGLBuffer::operator=(OpenGLBuffer&& other) noexcept
    {
        if (this != &other)
        {
            if (IsValid())
            {
                this->~OpenGLBuffer();
            }

            mObjectID = other.mObjectID;
            mTarget   = other.mTarget;

            other.mObjectID = 0;
            other.mTarget   = 0;
        }
        return *this;
    }

    bool OpenGLBuffer::IsValid() const
    {
        bool result;
        glCheck(result = glIsBuffer(mObjectID) == GL_TRUE);
        return result;
    }

    void OpenGLBuffer::Create()
    {
        MCC_ASSERT(OpenGLVertexArray::IsThereAnyBound(), "A VertexArray must be bound to create a Buffer");
        glCheck(glGenBuffers(1, &mObjectID));
        glBindBuffer(mTarget, mObjectID);
        MCC_ASSERT(IsValid(), "OpenGLBuffer creation failed");
    }

    void OpenGLBuffer::Delete()
    {
        if (IsValid())
        {
            glCheck(glDeleteBuffers(1, &mObjectID));
            mObjectID = 0;
        }
    }

    void OpenGLBuffer::Bind() const
    {
        MCC_ASSERT(IsValid(), "A Buffer must be valid (eg. created) to be bound");
        if (sBoundBuffer != mObjectID)
        {
            glCheck(glBindBuffer(mTarget, mObjectID));
            sBoundBuffer = mObjectID;
        }
    }

    void OpenGLBuffer::SetData(const GLsizeiptr size, const void* data, const GLenum usage) const
    {
        Bind();
        glCheck(glBufferData(mTarget, size, data, usage));
    }

}
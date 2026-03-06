// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Graphics/Buffer.h"

#include "Client/Graphics/Common.h"
#include "Client/Graphics/VertexArray.h"

#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    GLuint Buffer::sBoundBuffer = 0;

    Buffer::Buffer(const GLenum target) : mId(0), mTarget(target)
    {}

    Buffer::~Buffer()
    {
        Delete();
    }

    Buffer::Buffer(Buffer&& other) noexcept : mId(other.mId), mTarget(other.mTarget)
    {
        other.mId     = 0;
        other.mTarget = 0;
    }

    Buffer& Buffer::operator=(Buffer&& other) noexcept
    {
        if (this != &other)
        {
            if (IsValid())
            {
                this->~Buffer();
            }

            mId     = other.mId;
            mTarget = other.mTarget;

            other.mId     = 0;
            other.mTarget = 0;
        }
        return *this;
    }

    bool Buffer::IsValid() const
    {
        bool result;
        glCheck(result = glIsBuffer(mId) == GL_TRUE);
        return result;
    }

    void Buffer::Create()
    {
        MCC_ASSERT(VertexArray::IsThereAnyBound(), "A VertexArray must be bound to create a Buffer");
        glCheck(glGenBuffers(1, &mId));
        glBindBuffer(mTarget, mId);
        MCC_ASSERT(IsValid(), "Buffer creation failed");
    }

    void Buffer::Delete()
    {
        if (IsValid())
        {
            glCheck(glDeleteBuffers(1, &mId));
            mId = 0;
        }
    }

    void Buffer::Bind() const
    {
        MCC_ASSERT(IsValid(), "A Buffer must be valid (eg. created) to be bound");
        if (sBoundBuffer != mId)
        {
            glCheck(glBindBuffer(mTarget, mId));
            sBoundBuffer = mId;
        }
    }

    void Buffer::SetData(const GLsizeiptr size, const void* data, const GLenum usage) const
    {
        Bind();
        glCheck(glBufferData(mTarget, size, data, usage));
    }

}
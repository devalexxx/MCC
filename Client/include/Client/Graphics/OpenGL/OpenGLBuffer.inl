// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

namespace Mcc
{

    template<typename T>
    void OpenGLBuffer::SetData(std::span<T> data, GLenum usage)
    {
        SetData(static_cast<GLsizeiptr>(sizeof(T) * data.size()), data.data(), usage);
    }

}
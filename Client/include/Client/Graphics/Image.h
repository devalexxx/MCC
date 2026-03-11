// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_IMAGE_H
#define MCC_CLIENT_GRAPHICS_IMAGE_H

#include <functional>
#include <span>


namespace Mcc
{

    enum class PixelFormat
    {
        GREY, GREY_A, RGB, RGBA,
        Count
    };

    class Image
    {
      public:
        using DataType = std::unique_ptr<uint8_t, std::function<void(uint8_t*)>>;

        Image(uint32_t width, uint32_t height, uint32_t channels, PixelFormat format, DataType data);
        Image(uint32_t width, uint32_t height, uint32_t channels, PixelFormat format, uint8_t  value);
        Image(uint32_t width, uint32_t height, uint32_t channels, PixelFormat format);

        size_t   GetSize    () const;
        uint32_t GetWidth   () const;
        uint32_t GetHeight  () const;
        uint32_t GetChannels() const;

        PixelFormat        GetFormat() const;
        std::span<uint8_t> GetData  () const;

      private:
        uint32_t    mWidth;
        uint32_t    mHeight;
        uint32_t    mChannels;
        PixelFormat mFormat;
        DataType    mData;
    };

    Image STBLoadImage(const char* path, int desiredChannels = 0);

}

#endif

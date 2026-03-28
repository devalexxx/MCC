// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Graphics/Image.h"

#include "Common/Utils/Logging.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#   define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"

namespace Mcc
{

    Image::Image(
        const uint32_t width, const uint32_t height, const uint32_t channels, const PixelFormat format, DataType data
    ) :
        mWidth(width), mHeight(height), mChannels(channels),
        mFormat(format), mData(std::move(data))
    {}

    Image::Image(
        const uint32_t width, const uint32_t height, const uint32_t channels, const PixelFormat format, const uint8_t value
    ) :
        mWidth(width), mHeight(height), mChannels(channels), mFormat(format),
        mData(DataType(new uint8_t[width * height * channels], std::default_delete<uint8_t[]>()))
    {
        std::fill_n(mData.get(), GetSize(), value);
    }

    Image::Image(const uint32_t width, const uint32_t height, const uint32_t channels, const PixelFormat format) :
        Image(width, height, channels, format, 0)
    {}

    size_t Image::GetSize() const
    {
        return mWidth * mHeight * mChannels;
    }

    uint32_t Image::GetWidth() const
    {
        return mWidth;
    }

    uint32_t Image::GetHeight() const
    {
        return mHeight;
    }

    uint32_t Image::GetChannels() const
    {
        return mChannels;
    }

    PixelFormat Image::GetFormat() const
    {
        return mFormat;
    }

    std::span<uint8_t> Image::GetData() const
    {
        return { mData.get(), GetSize() };
    }

    Image STBLoadImage(const char* path, const int desiredChannels)
    {
        static PixelFormat formatMapping[] = {
            PixelFormat::GREY, PixelFormat::GREY_A, PixelFormat::RGB, PixelFormat::RGBA
        };

        uint32_t x, y, n;
        stbi_set_flip_vertically_on_load(true);
        const auto data = stbi_load(
            path, reinterpret_cast<int*>(&x), reinterpret_cast<int*>(&y), reinterpret_cast<int*>(&n), desiredChannels
        );
        if (!data)
        {
            MCC_LOG_ERROR("Failed to load image {}\n\treason: {}", path, stbi_failure_reason());
            return { x, y, n, formatMapping[n - 1] };
        }
        return { x, y, n, formatMapping[n - 1], { data, stbi_image_free } };
    }

    std::shared_ptr<Image> AssetLoader<Image>::operator()(
        AssetRegistry& reg, const std::string_view path, const bool cache
    ) const
    {
        const auto _path = reg.Resolve(path);
        if (!_path)
        {
            MCC_LOG_ERROR("[AssetGetter] Failed to resolve asset path {}", path);
            return nullptr;
        }

        auto image = std::make_shared<Image>(STBLoadImage(_path->c_str()));
        return cache
            ? std::dynamic_pointer_cast<Image>(reg.Add(std::string(path), image))
            : image;
    }

}

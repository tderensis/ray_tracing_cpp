#pragma once

#include <cstddef>

template <typename PIXEL_TYPE, int WIDTH, int HEIGHT>
class Image
{
public:
    auto& operator[](std::size_t column)
    {
        return m_image[column];
    }

    const auto& operator[](std::size_t column) const
    {
        return m_image[column];
    }

    static constexpr int width = WIDTH;
    static constexpr int height = HEIGHT;
private:
    PIXEL_TYPE m_image[WIDTH][HEIGHT] = {};
};

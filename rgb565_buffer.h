// MIT license; see LICENSE. Cached CPU conversion for the verified RGB565 backend.
#pragma once
#include <array>
#include <cstdint>
#include <new>
#include <vector>
namespace pc2fix
{
class Rgb565Buffer
{
    std::vector<uint32_t> pixels;
    static const std::array<uint32_t, 65536> &Colors()
    {
        static const auto colors = [] {
            std::array<uint32_t, 65536> result{};
            for (unsigned c = 0; c < result.size(); ++c)
            {
                const unsigned r = (c >> 11) & 31, g = (c >> 5) & 63, b = c & 31;
                result[c] = (((r << 3) | (r >> 2)) << 16) |
                            (((g << 2) | (g >> 4)) << 8) | ((b << 3) | (b >> 2));
            }
            return result;
        }();
        return colors;
    }

  public:
    bool Convert(const void *source, int pitch, int width, int height)
    {
        if (!source || width <= 0 || height <= 0 || width > 4096 || height > 2160 ||
            pitch < width * 2 || pitch % 4 || reinterpret_cast<uintptr_t>(source) % 2)
            return false;
        const size_t count = static_cast<size_t>(width) * height;
        try
        {
            pixels.resize(count);
        }
        catch (const std::bad_alloc &)
        {
            return false;
        }
        const auto &colors = Colors();
        const auto input = static_cast<const unsigned char *>(source);
        for (int y = 0; y < height; ++y)
        {
            const auto row = reinterpret_cast<const uint16_t *>(input + static_cast<size_t>(y) * pitch);
            auto output = pixels.data() + static_cast<size_t>(y) * width;
            for (int x = 0; x < width; ++x)
                output[x] = colors[row[x]];
        }
        return true;
    }
    const uint32_t *Data() const { return pixels.data(); }
    void Reset() { std::vector<uint32_t>().swap(pixels); }
};
} // namespace pc2fix

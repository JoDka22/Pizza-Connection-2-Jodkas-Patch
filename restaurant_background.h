// MIT license; see LICENSE. Restore only dirty pixels outside the room bitmap.
#pragma once
#include <windows.h>
#include <cstddef>
#include <cstring>
namespace pc2fix
{
struct BackgroundMargins
{
    RECT rectangles[4]{};
    unsigned count = 0;
};
inline BackgroundMargins RestaurantBackgroundMargins(RECT dirty, RECT artwork, RECT buffer)
{
    BackgroundMargins result{};
    RECT clipped{}, inside{};
    if (!IntersectRect(&clipped, &dirty, &buffer))
        return result;
    if (!IntersectRect(&inside, &clipped, &artwork))
    {
        result.rectangles[result.count++] = clipped;
        return result;
    }
    auto add = [&](RECT rectangle) {
        if (rectangle.right > rectangle.left && rectangle.bottom > rectangle.top)
            result.rectangles[result.count++] = rectangle;
    };
    add({clipped.left, clipped.top, clipped.right, inside.top});
    add({clipped.left, inside.bottom, clipped.right, clipped.bottom});
    add({clipped.left, inside.top, inside.left, inside.bottom});
    add({inside.right, inside.top, clipped.right, inside.bottom});
    return result;
}
inline bool ClearRestaurantMargins(BYTE *pixels, size_t bytes, int pitch, int width, int height,
                                   const BackgroundMargins &margins, BYTE backgroundTag)
{
    // This intermediate engine buffer has a 16-bit color plus one tag byte per
    // pixel. Its stride is in bytes, unlike the DirectDraw bitmap's WORD stride.
    if (!pixels || width <= 0 || width > 4096 || height <= 0 || height > 2160 ||
        pitch < width * 3 || pitch > 16384 || bytes < size_t(pitch) * height || margins.count > 4)
        return false;
    for (unsigned i = 0; i < margins.count; ++i)
    {
        auto r = margins.rectangles[i];
        if (r.left < 0 || r.top < 0 || r.right > width || r.bottom > height ||
            r.left >= r.right || r.top >= r.bottom)
            return false;
    }
    for (unsigned i = 0; i < margins.count; ++i)
    {
        auto r = margins.rectangles[i];
        for (LONG y = r.top; y < r.bottom; ++y)
        {
            auto row = pixels + size_t(y) * pitch + size_t(r.left) * 3;
            if (!backgroundTag)
                std::memset(row, 0, size_t(r.right - r.left) * 3);
            else
                for (LONG x = r.left; x < r.right; ++x, row += 3)
                {
                    row[0] = row[1] = 0;
                    row[2] = backgroundTag;
                }
        }
    }
    return true;
}
} // namespace pc2fix

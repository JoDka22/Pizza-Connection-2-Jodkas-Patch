// MIT license; see LICENSE. Dirty restoration must preserve current actors.
#define WIN32_LEAN_AND_MEAN
#include "../restaurant_background.h"
#include <cstdio>
#include <vector>
int main()
{
    int failures = 0;
    const RECT buffer{0, 0, 24, 18};
    const RECT artwork{5, 4, 19, 13};
    const RECT cases[] = {{0, 0, 24, 18}, {6, 5, 18, 12}, {0, 0, 3, 3},
                          {-8, -4, 12, 9}, {18, 12, 40, 30}, {20, 5, 22, 11},
                          {0, 14, 24, 18}, {30, 30, 40, 40}, {4, 4, 4, 9}};
    for (auto dirty : cases)
    {
        auto margins = pc2fix::RestaurantBackgroundMargins(dirty, artwork, buffer);
        bool correct = margins.count <= 4;
        for (LONG y = 0; y < buffer.bottom; ++y)
            for (LONG x = 0; x < buffer.right; ++x)
            {
                POINT pixel{x, y}; unsigned touches = 0;
                for (unsigned i = 0; i < margins.count; ++i)
                    touches += PtInRect(&margins.rectangles[i], pixel) != FALSE;
                const bool expected = PtInRect(&dirty, pixel) && !PtInRect(&artwork, pixel);
                correct &= touches == unsigned(expected);
            }
        for (unsigned i = 0; i < margins.count; ++i)
        {
            RECT clipped{};
            correct &= IntersectRect(&clipped, &buffer, &margins.rectangles[i]) &&
                       EqualRect(&clipped, &margins.rectangles[i]);
        }
        printf("%s: exact non-overlapping dirty/background difference\n", correct ? "PASS" : "FAIL");
        failures += !correct;
    }
    // A moved actor leaves old pixels in the dirty margin; a stationary actor
    // elsewhere must survive, and the current actor is drawn after restoration.
    std::vector<unsigned> pixels(24 * 18, 0);
    pixels[15 * 24 + 8] = 1; pixels[2 * 24 + 2] = 2;
    auto margins = pc2fix::RestaurantBackgroundMargins({6, 14, 12, 18}, artwork, buffer);
    for (unsigned i = 0; i < margins.count; ++i)
        for (LONG y = margins.rectangles[i].top; y < margins.rectangles[i].bottom; ++y)
            for (LONG x = margins.rectangles[i].left; x < margins.rectangles[i].right; ++x)
                pixels[y * 24 + x] = 0;
    pixels[16 * 24 + 10] = 3;
    bool correct = pixels[15 * 24 + 8] == 0 && pixels[2 * 24 + 2] == 2 && pixels[16 * 24 + 10] == 3;
    printf("%s: old footprint removed, unchanged and current actors preserved\n", correct ? "PASS" : "FAIL");
    failures += !correct;
    for (BYTE tag : {BYTE{0}, BYTE{17}})
    {
        const int pitch = 24 * 3 + 6;
        std::vector<BYTE> packed(pitch * 18 + 16, 0xa5);
        auto all = pc2fix::RestaurantBackgroundMargins(buffer, artwork, buffer);
        correct = pc2fix::ClearRestaurantMargins(packed.data(), pitch * 18, pitch, 24, 18, all, tag);
        for (int y = 0; y < 18; ++y)
            for (int byte = 0; byte < pitch; ++byte)
            {
                POINT point{byte / 3, y};
                BYTE expected = 0xa5;
                if (byte < 24 * 3 && !PtInRect(&artwork, point)) expected = byte % 3 == 2 ? tag : 0;
                correct &= packed[y * pitch + byte] == expected;
            }
        for (size_t i = pitch * 18; i < packed.size(); ++i) correct &= packed[i] == 0xa5;
        printf("%s: packed color/tag pixels, row padding and end guard preserved\n", correct ? "PASS" : "FAIL");
        failures += !correct;
        auto before = packed;
        correct = !pc2fix::ClearRestaurantMargins(packed.data(), pitch * 18 - 1, pitch, 24, 18, all, tag) &&
                  !pc2fix::ClearRestaurantMargins(packed.data(), packed.size(), 24 * 2, 24, 18, all, tag);
        all.rectangles[all.count - 1].right = 25;
        correct &= !pc2fix::ClearRestaurantMargins(packed.data(), packed.size(), pitch, 24, 18, all, tag) && packed == before;
        printf("%s: invalid packed buffer or rectangle rejected before every write\n", correct ? "PASS" : "FAIL");
        failures += !correct;
    }
    return failures ? 1 : 0;
}

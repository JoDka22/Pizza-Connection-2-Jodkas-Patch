// MIT license; see LICENSE. Synthetic pixels only, no game data required.
#define WIN32_LEAN_AND_MEAN
#include "../presentation_pixels.h"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <vector>
int main(int argc, char **argv)
{
    const bool verifyOnly = argc == 2 && std::strcmp(argv[1], "--verify-only") == 0;
    pc2fix::DisplayLayout layout{};
    pc2fix::MakeLayout(1920, 1080, layout);
    pc2fix::RestaurantView room{};
    if (!pc2fix::MakeRestaurantView(layout, {26, 79, 826, 630}, room))
        return 1;
    std::vector<WORD> source(1920 * 1080);
    for (size_t i = 0; i < source.size(); ++i)
        source[i] = static_cast<WORD>((i * 37) ^ (i / 1920 * 91));
    for (SIZE size : {SIZE{1920, 1080}, SIZE{1467, 825}, SIZE{1000, 1000}})
    {
        HDC dc = CreateCompatibleDC(nullptr);
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = size.cx;
        info.bmiHeader.biHeight = -size.cy;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        void *pixels = nullptr;
        HBITMAP bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
        if (!dc || !bitmap || !pixels)
            return 2;
        HGDIOBJ previous = SelectObject(dc, bitmap);
        RECT client{0, 0, size.cx, size.cy};
        auto draw = [&](bool optimized) {
            bool base = optimized
                            ? pc2fix::DrawRestaurantBase(dc, source.data(), 3840, layout, 0xf800, 0x7e0,
                                                        0x1f, client, room)
                            : pc2fix::DrawRgb16Frame(dc, source.data(), 3840, 1920, 1080, 0xf800, 0x7e0,
                                                    0x1f, client);
            return base && pc2fix::DrawRestaurantView(dc, source.data(), 3840, layout, 0xf800, 0x7e0,
                                                     0x1f, client, room) && GdiFlush();
        };
        if (!draw(false))
            return 3;
        auto output = static_cast<DWORD *>(pixels);
        std::vector<DWORD> expected(output, output + static_cast<size_t>(size.cx) * size.cy);
        if (!draw(true))
            return 4;
        for (size_t i = 0; i < expected.size(); ++i)
            if ((expected[i] & 0xffffff) != (output[i] & 0xffffff))
            {
                printf("FAIL: RGB mismatch at pixel %zu in %ldx%ld\n", i, size.cx, size.cy);
                return 5;
            }
        printf("PASS: all RGB pixels match baseline at %ldx%ld\n", size.cx, size.cy);
        double timings[2][5]{};
        for (int round = 0; !verifyOnly && round < 5; ++round)
            for (int order = 0; order < 2; ++order)
            {
                int mode = (round + order) % 2;
                for (int warm = 0; warm < 10; ++warm)
                    if (!draw(mode != 0))
                        return 6;
                auto start = std::chrono::steady_clock::now();
                for (int frame = 0; frame < 100; ++frame)
                    if (!draw(mode != 0))
                        return 7;
                timings[mode][round] =
                    std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start)
                        .count() / 100;
            }
        std::sort(timings[0], timings[0] + 5);
        std::sort(timings[1], timings[1] + 5);
        if (!verifyOnly)
            printf("Median composition %ldx%ld: baseline %.3f ms, clipped %.3f ms (%+.1f%%)\n",
                   size.cx, size.cy, timings[0][2], timings[1][2],
                   (timings[1][2] / timings[0][2] - 1) * 100);
        SelectObject(dc, previous);
        DeleteObject(bitmap);
        DeleteDC(dc);
    }
    return 0;
}

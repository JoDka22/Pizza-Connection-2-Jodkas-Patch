// MIT license; see LICENSE. Exhaustive colors and synthetic frames, no game assets.
#define WIN32_LEAN_AND_MEAN
#include "../presentation_buffer.h"
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
    // Include source padding and all 65536 RGB565 color values.
    const LONG pitch = 3848;
    std::vector<WORD> source(static_cast<size_t>(pitch / 2) * 1080);
    for (size_t i = 0; i < source.size(); ++i)
        source[i] = static_cast<WORD>(i);
    pc2fix::Rgb565Buffer converter;
    if (converter.Convert(nullptr, pitch, 1920, 1080) ||
        converter.Convert(source.data(), 3836, 1920, 1080) ||
        converter.Convert(source.data(), pitch, 0, 1080) ||
        converter.Convert(source.data(), pitch, 1920, 2161))
        return 2;
    puts("PASS: conversion rejects invalid input and dimensions");
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
            return 3;
        HGDIOBJ previous = SelectObject(dc, bitmap);
        RECT client{0, 0, size.cx, size.cy};
        pc2fix::PresentationBuffer buffer;
        for (int scene = 0; scene < 4; ++scene)
        {
            bool native = scene != 0;
            bool restaurant = scene == 2;
            DWORD red = scene == 3 ? 0x7c00 : 0xf800;
            DWORD green = scene == 3 ? 0x3e0 : 0x7e0;
            auto draw = [&](bool optimized) {
                if (optimized)
                {
                    bool enlarged = false;
                    return buffer.Compose(source.data(), pitch, layout, native, red, green, 0x1f,
                                          client, restaurant ? &room : nullptr, enlarged) &&
                           enlarged == restaurant && buffer.Present(dc) && GdiFlush();
                }
                bool base = restaurant
                    ? pc2fix::DrawRestaurantBase(dc, source.data(), pitch, layout, red, green, 0x1f, client, room)
                    : pc2fix::DrawRgb16Frame(dc, source.data(), pitch, native ? 1920 : 800,
                                            native ? 1080 : 600, red, green, 0x1f, client);
                return base && (!restaurant || pc2fix::DrawRestaurantView(dc, source.data(), pitch,
                                                layout, red, green, 0x1f, client, room)) && GdiFlush();
            };
            if (!draw(false)) return 4;
            auto output = static_cast<DWORD *>(pixels);
            std::vector<DWORD> expected(output, output + static_cast<size_t>(size.cx) * size.cy);
            if (!draw(true)) return 5;
            for (size_t i = 0; i < expected.size(); ++i)
                if ((expected[i] & 0xffffff) != (output[i] & 0xffffff))
                {
                    printf("FAIL: RGB mismatch scene %d at %zu in %ldx%ld\n", scene, i, size.cx, size.cy);
                    return 6;
                }
            printf("PASS: complete RGB image scene %d at %ldx%ld\n", scene, size.cx, size.cy);
            double timings[2][3]{};
            for (int round = 0; !verifyOnly && scene != 3 && round < 3; ++round)
                for (int order = 0; order < 2; ++order)
                {
                    int mode = (round + order) % 2;
                    for (int warm = 0; warm < 5; ++warm) if (!draw(mode != 0)) return 7;
                    auto start = std::chrono::steady_clock::now();
                    for (int frame = 0; frame < 50; ++frame) if (!draw(mode != 0)) return 8;
                    timings[mode][round] = std::chrono::duration<double, std::milli>(
                        std::chrono::steady_clock::now() - start).count() / 50;
                }
            for (auto &timing : timings) std::sort(timing, timing + 3);
            if (!verifyOnly && scene != 3)
                printf("Median scene %d %ldx%ld: GDI16 %.3f ms, converted/published %.3f ms\n",
                       scene, size.cx, size.cy, timings[0][1], timings[1][1]);
        }
        buffer.Reset();
        SelectObject(dc, previous);
        DeleteObject(bitmap);
        DeleteDC(dc);
    }
    converter.Reset();
    return 0;
}

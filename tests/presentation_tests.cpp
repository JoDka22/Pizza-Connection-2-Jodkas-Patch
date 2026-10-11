#define WIN32_LEAN_AND_MEAN
#include "../presentation_pixels.h"
#include <cstdio>
#include <vector>
int main()
{
    int failures = 0;
    auto check = [&](bool pass, const char *label) {
        printf("%s: %s\n", pass ? "PASS" : "FAIL", label);
        if (!pass)
            ++failures;
    };
    auto wide = pc2fix::FitView(1920, 1080, 800, 600);
    check(wide.left == 240 && wide.right == 1680 && wide.top == 0 && wide.bottom == 1080,
          "4:3 views use full height without distortion");
    auto city = pc2fix::FitView(1280, 720, 1920, 1080);
    check(city.left == 0 && city.right == 1280 && city.bottom == 720,
          "city fits a smaller window proportionally");
    auto tall = pc2fix::FitView(1000, 1000, 1920, 1080);
    check(tall.top > 0 && tall.left == 0, "resized tall window letterboxes vertically");
    POINT point{};
    check(pc2fix::ViewPoint(wide, 800, 600, {960, 540}, point) && point.x == 400 && point.y == 300,
          "mouse uses inverse view transformation");
    check(!pc2fix::ViewPoint(wide, 800, 600, {239, 100}, point) &&
              !pc2fix::ViewPoint(wide, 800, 600, {1680, 100}, point),
          "letterbox bars and exclusive right boundary reject input");
    check(pc2fix::FitView(0, 0, 800, 600).right == 0 && !pc2fix::ViewPoint({}, 800, 600, {0, 0}, point),
          "minimized/empty views reject input safely");
    auto dc = CreateCompatibleDC(nullptr);
    BITMAPINFO bitmap{};
    bitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap.bmiHeader.biWidth = 1920;
    bitmap.bmiHeader.biHeight = -1080;
    bitmap.bmiHeader.biPlanes = 1;
    bitmap.bmiHeader.biBitCount = 32;
    bitmap.bmiHeader.biCompression = BI_RGB;
    void *output = nullptr;
    auto dib = CreateDIBSection(dc, &bitmap, DIB_RGB_COLORS, &output, nullptr, 0);
    if (!dc || !dib)
        return 2;
    auto previous = SelectObject(dc, dib);
    // Actual 1920-wide row stride, 600 authored rows and unused black rows below.
    std::vector<WORD> source(1920 * 1080, 0);
    for (int y = 0; y < 600; ++y)
        for (int x = 0; x < 800; ++x)
            source[y * 1920 + x] = y < 300 ? 0xf800 : 0x001f;
    check(pc2fix::DrawRgb16Frame(dc, source.data(), 3840, 800, 600, 0xf800, 0x7e0, 0x1f, {0, 0, 1920, 1080}),
          "actual GDI RGB565 conversion succeeds");
    check(GetPixel(dc, 960, 1) == RGB(255, 0, 0), "first authored row retained (DIB crop regression)");
    check(GetPixel(dc, 960, 1078) == RGB(0, 0, 255), "last authored row retained and correctly oriented");
    check(GetPixel(dc, 239, 500) == RGB(0, 0, 0) && GetPixel(dc, 1680, 500) == RGB(0, 0, 0),
          "unused desktop bars cleared");
    check(!pc2fix::DrawRgb16Frame(dc, source.data(), 1598, 800, 600, 0xf800, 0x7e0, 0x1f, {0, 0, 1920, 1080}),
          "undersized source pitch rejected");
    SelectObject(dc, previous);
    DeleteObject(dib);
    DeleteDC(dc);
    return failures ? 1 : 0;
}

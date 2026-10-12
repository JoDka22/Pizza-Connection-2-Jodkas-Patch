#define WIN32_LEAN_AND_MEAN
#include "../presentation_buffer.h"
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
    WINDOWPLACEMENT placement{sizeof(WINDOWPLACEMENT)};
    placement.rcNormalPosition = {100, 120, 1380, 840};
    placement.showCmd = SW_SHOWMINIMIZED;
    auto restored = pc2fix::FocusRestorePlacement(placement);
    check(restored.showCmd == SW_SHOWNORMAL &&
              EqualRect(&restored.rcNormalPosition, &placement.rcNormalPosition),
          "minimized focus snapshot restores normal window with unchanged geometry");
    placement.flags = WPF_RESTORETOMAXIMIZED;
    restored = pc2fix::FocusRestorePlacement(placement);
    check(restored.showCmd == SW_SHOWMAXIMIZED && !(restored.flags & WPF_RESTORETOMAXIMIZED),
          "minimized maximized window returns to maximized state");
    placement.showCmd = SW_MINIMIZE;
    auto alternate = pc2fix::FocusRestorePlacement(placement);
    placement.showCmd = SW_SHOWMINNOACTIVE;
    check(alternate.showCmd == SW_SHOWMAXIMIZED &&
              pc2fix::FocusRestorePlacement(placement).showCmd == SW_SHOWMAXIMIZED,
          "all minimized show commands normalize for activation");
    placement.showCmd = SW_SHOWMAXIMIZED;
    restored = pc2fix::FocusRestorePlacement(placement);
    check(restored.showCmd == placement.showCmd && restored.flags == placement.flags &&
              EqualRect(&restored.rcNormalPosition, &placement.rcNormalPosition),
          "ordinary focus snapshot remains unchanged");
    check(wide.left == 240 && wide.right == 1680 && wide.top == 0 && wide.bottom == 1080,
          "4:3 views use full height without distortion");
    auto city = pc2fix::FitView(1280, 720, 1920, 1080);
    check(city.left == 0 && city.right == 1280 && city.bottom == 720,
          "city fits a smaller window proportionally");
    auto tall = pc2fix::FitView(1000, 1000, 1920, 1080);
    check(tall.top > 0 && tall.left == 0, "resized tall window letterboxes vertically");
    POINT point{};
    pc2fix::DisplayLayout layout{};
    pc2fix::MakeLayout(1920, 1080, layout);
    pc2fix::RestaurantView restaurant{};
    check(pc2fix::MakeRestaurantView(layout, {26, 79, 826, 630}, restaurant) &&
              restaurant.destination.right - restaurant.destination.left > 800 &&
              abs(restaurant.destination.left - (restaurant.area.right - restaurant.destination.right)) <=
                  1 &&
              abs(restaurant.destination.top - (restaurant.area.bottom - restaurant.destination.bottom)) <= 1,
          "restaurant enlarged and centered in area clear of sidebar tabs");
    const auto &dst = restaurant.destination;
    check(!pc2fix::RestaurantFramePoint(restaurant, false, {dst.left, dst.top}, point),
          "changed room rejects picking until matching image is presented");
    check(pc2fix::RestaurantFramePoint(restaurant, true, {dst.left, dst.top}, point) &&
              point.x == restaurant.source.left && point.y == restaurant.source.top,
          "matching frame retains inverse room picking");
    check(pc2fix::RestaurantFramePoint(restaurant, false, {1900, 300}, point) && point.x == 1900 &&
              point.y == 300 && pc2fix::RestaurantFramePoint(restaurant, false, {500, 1050}, point) &&
              point.x == 500 && point.y == 1050,
          "stale room geometry does not suppress sidebar or status input");
    check(pc2fix::RestaurantPoint(restaurant, {dst.left, dst.top}, point) &&
              point.x == restaurant.source.left && point.y == restaurant.source.top &&
              pc2fix::RestaurantPoint(restaurant, {dst.right - 1, dst.bottom - 1}, point) &&
              point.x < restaurant.source.right && point.y < restaurant.source.bottom,
          "restaurant corners map inside the original artwork crop");
    check(!pc2fix::RestaurantPoint(restaurant, {0, 0}, point) &&
              !pc2fix::RestaurantPoint(restaurant, {dst.right, dst.top}, point),
          "black margins and exclusive crop boundary reject furniture placement");
    check(pc2fix::RestaurantPoint(restaurant, {1681, 300}, point) && point.x == 1681 && point.y == 300 &&
              pc2fix::RestaurantPoint(restaurant, {900, 1031}, point) && point.x == 900 && point.y == 1031,
          "protruding sidebar controls and status retain their input coordinates");
    auto interiorWindow = pc2fix::FitView(1467, 825, 1920, 1080);
    POINT screen{interiorWindow.left +
                     MulDiv((dst.left + dst.right) / 2, interiorWindow.right - interiorWindow.left, 1920),
                 interiorWindow.top +
                     MulDiv((dst.top + dst.bottom) / 2, interiorWindow.bottom - interiorWindow.top, 1080)};
    POINT native{};
    check(pc2fix::ViewPoint(interiorWindow, 1920, 1080, screen, native) &&
              pc2fix::RestaurantPoint(restaurant, native, point) && abs(point.x - 426) <= 1 &&
              abs(point.y - 354) <= 1,
          "window fit and restaurant inverse mapping compose accurately");
    pc2fix::RestaurantView invalid{};
    check(!pc2fix::MakeRestaurantView(layout, {-1, 0, 800, 600}, invalid) &&
              !pc2fix::MakeRestaurantView(layout, {0, 0, 1800, 1000}, invalid) &&
              !pc2fix::MakeRestaurantView(layout, {}, invalid),
          "partly visible oversized and empty artwork preserve normal camera view");
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
    std::fill(source.begin(), source.end(), WORD{0});
    for (int y = restaurant.source.top; y < restaurant.source.bottom; ++y)
        for (int x = restaurant.source.left; x < restaurant.source.right; ++x)
            source[y * 1920 + x] =
                y < (restaurant.source.top + restaurant.source.bottom) / 2 ? 0xf800 : 0x001f;
    for (int y = 0; y < 1080; ++y)
        for (int x = 1681; x < 1920; ++x)
            source[y * 1920 + x] = 0x07e0;
    check(pc2fix::DrawRgb16Frame(dc, source.data(), 3840, 1920, 1080, 0xf800, 0x7e0, 0x1f,
                                 {0, 0, 1920, 1080}) &&
              pc2fix::DrawRestaurantView(dc, source.data(), 3840, layout, 0xf800, 0x7e0, 0x1f,
                                         {0, 0, 1920, 1080}, restaurant),
          "actual GDI restaurant crop and enlargement succeeds");
    check(GetPixel(dc, (dst.left + dst.right) / 2, dst.top + 2) == RGB(255, 0, 0) &&
              GetPixel(dc, (dst.left + dst.right) / 2, dst.bottom - 3) == RGB(0, 0, 255),
          "nonzero source crop retains top and bottom row orientation");
    check(GetPixel(dc, 1681, 300) == RGB(0, 255, 0) && GetPixel(dc, 1900, 300) == RGB(0, 255, 0) &&
              GetPixel(dc, 1800, 1050) == RGB(0, 255, 0),
          "world redraw preserves original sidebar overhang and status pixels");
    check(GetPixel(dc, 0, 100) == RGB(0, 0, 0), "old restaurant origin erased from presentation");
    pc2fix::PresentationBuffer buffer;
    bool enlarged = false;
    SetPixel(dc, 500, 300, RGB(255, 255, 255));
    check(buffer.Compose(source.data(), 3840, layout, true, 0xf800, 0x7e0, 0x1f, {0, 0, 1920, 1080},
                         &restaurant, enlarged) &&
              enlarged && GetPixel(dc, 500, 300) == RGB(255, 255, 255),
          "offscreen composition leaves visible target untouched until publication");
    check(buffer.Present(dc) && GetPixel(dc, 500, 300) == RGB(255, 0, 0) &&
              GetPixel(dc, 1900, 300) == RGB(0, 255, 0) && GetPixel(dc, 0, 100) == RGB(0, 0, 0),
          "single publication transfers complete restaurant UI and black margins");
    auto handles = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    bool repeated = true;
    for (int i = 0; i < 20; ++i)
        repeated = repeated &&
                   buffer.Compose(source.data(), 3840, layout, true, 0xf800, 0x7e0, 0x1f, {0, 0, 1920, 1080},
                                  &restaurant, enlarged) &&
                   buffer.Present(dc);
    check(repeated && GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS) == handles,
          "repeated frames reuse GDI resources without accumulating handles");
    SetPixel(dc, 500, 300, RGB(255, 255, 255));
    check(!buffer.Compose(source.data(), 1598, layout, true, 0xf800, 0x7e0, 0x1f, {0, 0, 1920, 1080},
                          &restaurant, enlarged) &&
              !buffer.Present(dc) && GetPixel(dc, 500, 300) == RGB(255, 255, 255),
          "invalid source does not publish partial or stale buffered frame");
    auto broken = restaurant;
    broken.source.left = -1;
    check(buffer.Compose(source.data(), 3840, layout, true, 0xf800, 0x7e0, 0x1f, {0, 0, 1920, 1080}, &broken,
                         enlarged) &&
              !enlarged && buffer.Present(dc) && GetPixel(dc, 100, 100) == RGB(255, 0, 0),
          "failed enlargement composes complete original frame before publication");
    check(buffer.Compose(source.data(), 3840, layout, true, 0xf800, 0x7e0, 0x1f, {0, 0, 1000, 1000},
                         &restaurant, enlarged) &&
              buffer.Present(dc) && GetPixel(dc, 500, 900) == RGB(0, 0, 0),
          "resizing replaces composition buffer and clears new letterbox bars");
    check(!buffer.Compose(source.data(), 3840, layout, true, 0xf800, 0x7e0, 0x1f, {0, 0, 5000, 3000}, nullptr,
                          enlarged) &&
              !buffer.Present(dc),
          "unbounded output allocation rejected without publishing");
    buffer.Reset();
    check(GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS) == handles - 2 && !buffer.Present(dc),
          "explicit buffer teardown releases bitmap and memory DC");
    SelectObject(dc, previous);
    DeleteObject(dib);
    DeleteDC(dc);
    return failures ? 1 : 0;
}

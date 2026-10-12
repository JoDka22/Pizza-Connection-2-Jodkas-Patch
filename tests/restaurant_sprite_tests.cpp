// MIT license; see LICENSE. Synthetic sprites outside the previous artwork crop.
#define WIN32_LEAN_AND_MEAN
#include "../presentation_buffer.h"
#include <cstdio>
#include <vector>
int main()
{
    int failures = 0;
    auto check = [&](bool pass, const char *name) {
        printf("%s: %s\n", pass ? "PASS" : "FAIL", name);
        failures += !pass;
    };
    pc2fix::DisplayLayout layout{};
    pc2fix::MakeLayout(1920, 1080, layout);
    pc2fix::RestaurantView room{}, canvas{};
    pc2fix::MakeRestaurantView(layout, {26, 79, 826, 630}, room);
    check(pc2fix::RestaurantCanvasView(room, canvas) && EqualRect(&canvas.source, &room.area) &&
              canvas.destination.right > room.destination.right && canvas.destination.bottom > room.destination.bottom,
          "moving world sprites extend beyond background anchor at the same zoom");
    auto broken = room; broken.source.left = -1;
    pc2fix::RestaurantView invalid{};
    check(!pc2fix::RestaurantCanvasView(broken, invalid) && !pc2fix::RestaurantCanvasView({}, invalid),
          "invalid world extension rejected before geometry division");
    auto project = [&](POINT source) {
        return POINT{canvas.destination.left + MulDiv(source.x - canvas.source.left,
                         canvas.destination.right - canvas.destination.left, canvas.source.right - canvas.source.left),
                     canvas.destination.top + MulDiv(source.y - canvas.source.top,
                         canvas.destination.bottom - canvas.destination.top, canvas.source.bottom - canvas.source.top)};
    };
    auto anchor = project({room.source.left, room.source.top});
    check(abs(anchor.x - room.destination.left) <= 1 && abs(anchor.y - room.destination.top) <= 1,
          "room placement stays anchored instead of following moving characters");
    std::vector<WORD> source(1920 * 1080, 0);
    RECT actorRight{room.source.right + 2, 300, room.source.right + 22, 340};
    RECT actorBottom{100, room.source.bottom + 1, 130, room.source.bottom + 7};
    auto fill = [&](RECT rectangle, WORD color) {
        for (LONG y = rectangle.top; y < rectangle.bottom; ++y)
            for (LONG x = rectangle.left; x < rectangle.right; ++x) source[y * 1920 + x] = color;
    };
    fill({26, 79, 826, 630}, 0x7bef);
    fill(actorRight, 0xf800); fill(actorBottom, 0x001f);
    fill({room.area.right, 0, 1920, 1080}, 0x07e0);
    fill({0, 1031, 1920, 1080}, 0x07e0);
    for (SIZE size : {SIZE{1920, 1080}, SIZE{1467, 825}, SIZE{1000, 1000}})
    {
        HDC dc = CreateCompatibleDC(nullptr);
        BITMAPINFO info{}; info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = size.cx; info.bmiHeader.biHeight = -size.cy;
        info.bmiHeader.biPlanes = 1; info.bmiHeader.biBitCount = 32;
        void *pixels = nullptr;
        HBITMAP bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
        if (!dc || !bitmap || !pixels) return 2;
        auto old = SelectObject(dc, bitmap);
        RECT client{0, 0, size.cx, size.cy};
        pc2fix::PresentationBuffer buffer;
        bool enlarged = false;
        check(buffer.Compose(source.data(), 3840, layout, true, 0xf800, 0x7e0, 0x1f, client, &room, enlarged) &&
                  enlarged && buffer.Present(dc) && GdiFlush(), "extended world composition and publication succeed");
        auto toOutput = [&](POINT point) {
            auto r = pc2fix::RestaurantOutputRect(layout, client, {point.x, point.y, point.x + 1, point.y + 1});
            return POINT{r.left, r.top};
        };
        POINT rightCenter{(actorRight.left + actorRight.right) / 2, (actorRight.top + actorRight.bottom) / 2};
        auto right = toOutput(project(rightCenter));
        auto bottom = toOutput(project({115, room.source.bottom + 3}));
        check(GetPixel(dc, right.x, right.y) == RGB(255, 0, 0),
              "complete foreground sprite beyond old right crop remains visible");
        check(GetPixel(dc, bottom.x, bottom.y) == RGB(0, 0, 255),
              "sprite tail beyond old bottom crop is drawn up to world viewport");
        POINT frame{}, mapped{};
        auto fitted = pc2fix::FitView(size.cx, size.cy, 1920, 1080);
        check(pc2fix::ViewPoint(fitted, 1920, 1080, right, frame) &&
                  pc2fix::RestaurantPoint(room, frame, mapped) &&
                  abs(mapped.x - rightCenter.x) <= 2 && abs(mapped.y - rightCenter.y) <= 2,
              "newly visible sprite has matching inverse picking after window fit");
        auto sidebar = toOutput({1900, 300}), status = toOutput({900, 1050});
        check(GetPixel(dc, sidebar.x, sidebar.y) == RGB(0, 255, 0) &&
                  GetPixel(dc, status.x, status.y) == RGB(0, 255, 0),
              "world extension preserves authored sidebar and status pixels");
        auto empty = toOutput({0, 100}), tab = toOutput({room.area.right + 4, 100});
        check(GetPixel(dc, empty.x, empty.y) == RGB(0, 0, 0) && GetPixel(dc, tab.x, tab.y) == RGB(0, 255, 0),
              "extended sprites stay clipped out of sidebar overhang and canvas margin");
        buffer.Reset(); SelectObject(dc, old); DeleteObject(bitmap); DeleteDC(dc);
    }
    return failures ? 1 : 0;
}

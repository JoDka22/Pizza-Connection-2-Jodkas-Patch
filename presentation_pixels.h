// MIT license; see LICENSE. Shared by the actual presenter and native GDI tests.
#pragma once
#include "presentation.h"
#include "restaurant_view.h"
namespace pc2fix
{
inline RECT RestaurantOutputRect(const DisplayLayout &layout, RECT client, RECT rectangle)
{
    auto frame = FitView(client.right, client.bottom, layout.width, layout.height);
    return {frame.left + MulDiv(rectangle.left, frame.right - frame.left, layout.width),
            frame.top + MulDiv(rectangle.top, frame.bottom - frame.top, layout.height),
            frame.left + MulDiv(rectangle.right, frame.right - frame.left, layout.width),
            frame.top + MulDiv(rectangle.bottom, frame.bottom - frame.top, layout.height)};
}
inline bool DrawRestaurantView(HDC dc, const void *pixels, LONG pitch, const DisplayLayout &layout, DWORD red,
                               DWORD green, DWORD blue, RECT client, const RestaurantView &view,
                               unsigned bitCount = 16)
{
    if ((bitCount != 16 && bitCount != 32) || !dc || !pixels ||
        pitch < layout.width * static_cast<int>(bitCount / 8) || pitch % 4 || client.right <= 0 || client.bottom <= 0 ||
        view.source.left < 0 || view.source.top < 0 || view.source.right > layout.width ||
        view.source.bottom > layout.height || view.source.right <= view.source.left ||
        view.source.bottom <= view.source.top || view.destination.right <= view.destination.left ||
        view.destination.bottom <= view.destination.top)
        return false;
    struct Dib
    {
        BITMAPINFOHEADER header;
        DWORD masks[3];
    } dib{};
    dib.header.biSize = sizeof(BITMAPINFOHEADER);
    dib.header.biWidth = pitch / static_cast<int>(bitCount / 8);
    dib.header.biHeight = -layout.height;
    dib.header.biPlanes = 1;
    dib.header.biBitCount = static_cast<WORD>(bitCount);
    dib.header.biCompression = bitCount == 16 ? BI_BITFIELDS : BI_RGB;
    dib.masks[0] = red;
    dib.masks[1] = green;
    dib.masks[2] = blue;
    RestaurantView canvas{};
    if (!RestaurantCanvasView(view, canvas))
        return false;
    auto area = RestaurantOutputRect(layout, client, canvas.area);
    auto destination = RestaurantOutputRect(layout, client, canvas.destination);
    int saved = SaveDC(dc);
    if (!saved)
        return false;
    if (IntersectClipRect(dc, area.left, area.top, area.right, area.bottom) == ERROR)
    {
        RestoreDC(dc, saved);
        return false;
    }
    if (!FillRect(dc, &area, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH))) ||
        !SetStretchBltMode(dc, COLORONCOLOR))
    {
        RestoreDC(dc, saved);
        return false;
    }
    // GDI source Y is relative to the bottom even for a top-down DIB crop.
    int rows = StretchDIBits(dc, destination.left, destination.top, destination.right - destination.left,
                             destination.bottom - destination.top, canvas.source.left,
                             layout.height - canvas.source.bottom, canvas.source.right - canvas.source.left,
                             canvas.source.bottom - canvas.source.top, pixels,
                             reinterpret_cast<BITMAPINFO *>(&dib), DIB_RGB_COLORS, SRCCOPY);
    return RestoreDC(dc, saved) != FALSE && rows != 0 && rows != GDI_ERROR;
}
inline bool DrawRgbFrame(HDC dc, const void *pixels, LONG pitch, int width, int height, DWORD red,
                         DWORD green, DWORD blue, RECT client, unsigned bitCount)
{
    if ((bitCount != 16 && bitCount != 32) || !dc || !pixels || width <= 0 || height <= 0 ||
        pitch < width * static_cast<int>(bitCount / 8) || pitch % 4 || client.right <= 0 ||
        client.bottom <= 0)
        return false;
    struct Dib
    {
        BITMAPINFOHEADER header;
        DWORD masks[3];
    } dib{};
    dib.header.biSize = sizeof(BITMAPINFOHEADER);
    dib.header.biWidth = pitch / static_cast<int>(bitCount / 8);
    // Describe only the authored rows, never padding below an 800x600 view.
    dib.header.biHeight = -height;
    dib.header.biPlanes = 1;
    dib.header.biBitCount = static_cast<WORD>(bitCount);
    dib.header.biCompression = bitCount == 16 ? BI_BITFIELDS : BI_RGB;
    dib.masks[0] = red;
    dib.masks[1] = green;
    dib.masks[2] = blue;
    auto area = FitView(client.right, client.bottom, width, height);
    RECT bars[] = {{0, 0, client.right, area.top},
                   {0, area.bottom, client.right, client.bottom},
                   {0, area.top, area.left, area.bottom},
                   {area.right, area.top, client.right, area.bottom}};
    for (auto &bar : bars)
        if (bar.right > bar.left && bar.bottom > bar.top &&
            !FillRect(dc, &bar, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH))))
            return false;
    if (!SetStretchBltMode(dc, COLORONCOLOR))
        return false;
    int rows =
        StretchDIBits(dc, area.left, area.top, area.right - area.left, area.bottom - area.top, 0, 0, width,
                      height, pixels, reinterpret_cast<BITMAPINFO *>(&dib), DIB_RGB_COLORS, SRCCOPY);
    return rows != 0 && rows != GDI_ERROR;
}
inline bool DrawRestaurantBase(HDC dc, const void *pixels, LONG pitch, const DisplayLayout &layout,
                               DWORD red, DWORD green, DWORD blue, RECT client, const RestaurantView &view,
                               unsigned bitCount = 16)
{
    if (!dc || view.area.left < 0 || view.area.top < 0 || view.area.right > layout.width ||
        view.area.bottom > layout.height || view.area.right <= view.area.left ||
        view.area.bottom <= view.area.top)
        return false;
    auto area = RestaurantOutputRect(layout, client, view.area);
    const int saved = SaveDC(dc);
    if (!saved)
        return false;
    // The enlarged room replaces this entire area. Preserve sidebar/status/bars
    // without first converting and drawing pixels that will be erased again.
    bool ok = ExcludeClipRect(dc, area.left, area.top, area.right, area.bottom) != ERROR &&
              DrawRgbFrame(dc, pixels, pitch, layout.width, layout.height, red, green, blue, client, bitCount);
    return RestoreDC(dc, saved) != FALSE && ok;
}
inline bool DrawRgb16Frame(HDC dc, const void *pixels, LONG pitch, int width, int height, DWORD red,
                           DWORD green, DWORD blue, RECT client)
{
    return DrawRgbFrame(dc, pixels, pitch, width, height, red, green, blue, client, 16);
}
} // namespace pc2fix

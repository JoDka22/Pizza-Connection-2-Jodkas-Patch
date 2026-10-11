// MIT license; see LICENSE. Shared by the actual presenter and native GDI tests.
#pragma once
#include "presentation.h"
namespace pc2fix
{
inline bool DrawRgb16Frame(HDC dc, const void *pixels, LONG pitch, int width, int height, DWORD red,
                           DWORD green, DWORD blue, RECT client)
{
    if (!dc || !pixels || width <= 0 || height <= 0 || pitch < width * 2 || pitch % 4 || client.right <= 0 ||
        client.bottom <= 0)
        return false;
    struct Dib
    {
        BITMAPINFOHEADER header;
        DWORD masks[3];
    } dib{};
    dib.header.biSize = sizeof(BITMAPINFOHEADER);
    dib.header.biWidth = pitch / 2;
    // Describe only the authored rows, never padding below an 800x600 view.
    dib.header.biHeight = -height;
    dib.header.biPlanes = 1;
    dib.header.biBitCount = 16;
    dib.header.biCompression = BI_BITFIELDS;
    dib.masks[0] = red;
    dib.masks[1] = green;
    dib.masks[2] = blue;
    auto area = FitView(client.right, client.bottom, width, height);
    RECT bars[] = {{0, 0, client.right, area.top},
                   {0, area.bottom, client.right, client.bottom},
                   {0, area.top, area.left, area.bottom},
                   {area.right, area.top, client.right, area.bottom}};
    for (auto &bar : bars)
        FillRect(dc, &bar, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
    SetStretchBltMode(dc, COLORONCOLOR);
    int rows =
        StretchDIBits(dc, area.left, area.top, area.right - area.left, area.bottom - area.top, 0, 0, width,
                      height, pixels, reinterpret_cast<BITMAPINFO *>(&dib), DIB_RGB_COLORS, SRCCOPY);
    return rows != 0 && rows != GDI_ERROR;
}
} // namespace pc2fix

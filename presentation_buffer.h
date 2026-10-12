// MIT license; see LICENSE. Compose offscreen, publish only completed images.
#pragma once
#include "presentation_pixels.h"
#include "rgb565_buffer.h"
namespace pc2fix
{
class PresentationBuffer
{
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ previous = nullptr;
    int width = 0, height = 0;
    bool ready = false;
    Rgb565Buffer rgb565;
    bool Ensure(int w, int h)
    {
        if (w <= 0 || h <= 0 || w > 4096 || h > 2160)
            return false;
        if (dc && width == w && height == h)
            return true;
        auto nextDC = CreateCompatibleDC(nullptr);
        if (!nextDC)
            return false;
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = w;
        info.bmiHeader.biHeight = -h;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void *pixels = nullptr;
        auto nextBitmap = CreateDIBSection(nextDC, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
        auto nextPrevious = nextBitmap ? SelectObject(nextDC, nextBitmap) : nullptr;
        if (!nextBitmap || !nextPrevious || nextPrevious == HGDI_ERROR)
        {
            if (nextBitmap)
                DeleteObject(nextBitmap);
            DeleteDC(nextDC);
            return false;
        }
        Reset();
        dc = nextDC;
        bitmap = nextBitmap;
        previous = nextPrevious;
        width = w;
        height = h;
        return true;
    }

  public:
    PresentationBuffer() = default;
    PresentationBuffer(const PresentationBuffer &) = delete;
    PresentationBuffer &operator=(const PresentationBuffer &) = delete;
    // Explicit teardown avoids GDI calls from DLL-loader static destruction.
    void Reset()
    {
        rgb565.Reset();
        if (dc && previous)
            SelectObject(dc, previous);
        if (bitmap)
            DeleteObject(bitmap);
        if (dc)
            DeleteDC(dc);
        dc = nullptr;
        bitmap = nullptr;
        previous = nullptr;
        width = height = 0;
        ready = false;
    }
    bool Compose(const void *pixels, LONG pitch, const DisplayLayout &layout, bool native, DWORD red,
                 DWORD green, DWORD blue, RECT client, const RestaurantView *interior, bool &enlarged)
    {
        ready = false;
        enlarged = false;
        if (!Ensure(client.right, client.bottom))
            return false;
        const int sourceWidth = native ? layout.width : 800;
        const int sourceHeight = native ? layout.height : 600;
        const void *image = pixels;
        LONG imagePitch = pitch;
        unsigned bitCount = 16;
        // Other formats and allocation failures retain the original GDI path.
        // Convert only authored rows; menu frames use an 800x600 subrectangle.
        if (red == 0xf800 && green == 0x7e0 && blue == 0x1f &&
            rgb565.Convert(pixels, pitch, sourceWidth, sourceHeight))
        {
            image = rgb565.Data();
            imagePitch = sourceWidth * 4;
            bitCount = 32;
        }
        if (native && interior)
        {
            enlarged = DrawRestaurantBase(dc, image, imagePitch, layout, red, green, blue, client, *interior, bitCount) &&
                       DrawRestaurantView(dc, image, imagePitch, layout, red, green, blue, client, *interior, bitCount);
        }
        if (!enlarged && !DrawRgbFrame(dc, image, imagePitch, sourceWidth, sourceHeight, red, green, blue, client, bitCount))
        {
            if (bitCount != 32 || !DrawRgb16Frame(dc, pixels, pitch, sourceWidth, sourceHeight, red, green, blue, client))
                return false;
        }
        ready = true;
        return true;
    }
    bool Present(HDC target) const
    {
        return ready && target && BitBlt(target, 0, 0, width, height, dc, 0, 0, SRCCOPY) != FALSE;
    }
};
} // namespace pc2fix

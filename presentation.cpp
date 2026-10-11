// MIT license; see LICENSE. Optional output backend for the verified GOG image.
#define WIN32_LEAN_AND_MEAN
#include "presentation.h"
#include "main.h"
#include "presentation_pixels.h"
#include <MinHook.h>
#include <cstdio>
namespace
{
pc2fix::DisplayLayout layout{};
bool enabled = false, framed = false;
HWND gameWindow = nullptr;
IDirectDrawSurface *front = nullptr;
IDirectDrawSurface *back = nullptr;
using AttachedFn = HRESULT(WINAPI *)(IDirectDrawSurface *, DDSCAPS *, IDirectDrawSurface **);
AttachedFn attachedOriginal = nullptr;
using FlipFn = void(__thiscall *)(void *);
FlipFn flipOriginal = nullptr;
using MessageFn = LRESULT(__thiscall *)(void *, HWND, UINT, WPARAM, LPARAM);
MessageFn messageOriginal = nullptr;
using ClipFn = BOOL(WINAPI *)(const RECT *);
ClipFn clipOriginal = nullptr;
bool reported = false;
WINDOWPLACEMENT focusPlacement{sizeof(WINDOWPLACEMENT)};
bool haveFocusPlacement = false;
RECT View(HWND window)
{
    RECT client{};
    GetClientRect(window, &client);
    return pc2fix::FitView(client.right, client.bottom, IsNativeWorldFrame() ? layout.width : 800,
                           IsNativeWorldFrame() ? layout.height : 600);
}
void Paint()
{
    if (!front || !IsWindow(gameWindow) || IsIconic(gameWindow))
        return;
    DDSURFACEDESC data{};
    data.dwSize = sizeof(data);
    auto result = front->Lock(nullptr, &data, DDLOCK_WAIT | DDLOCK_READONLY, nullptr);
    if (FAILED(result))
    {
        if (!reported)
        {
            Log("present: front lock failed; frame skipped");
            reported = true;
        }
        return;
    }
    if (data.lPitch >= layout.width * 2 && data.lPitch % 4 == 0 && data.ddpfPixelFormat.dwRGBBitCount == 16)
    {
        auto dc = GetDC(gameWindow);
        if (dc)
        {
            RECT client{};
            GetClientRect(gameWindow, &client);
            if (!pc2fix::DrawRgb16Frame(
                    dc, data.lpSurface, data.lPitch, IsNativeWorldFrame() ? layout.width : 800,
                    IsNativeWorldFrame() ? layout.height : 600, data.ddpfPixelFormat.dwRBitMask,
                    data.ddpfPixelFormat.dwGBitMask, data.ddpfPixelFormat.dwBBitMask, client) &&
                !reported)
            {
                Log("present: GDI output failed");
                reported = true;
            }
            ReleaseDC(gameWindow, dc);
        }
    }
    else if (!reported)
    {
        Log("present: unexpected RGB16 format/pitch; frame skipped");
        reported = true;
    }
    front->Unlock(data.lpSurface);
}
HRESULT WINAPI Attached(IDirectDrawSurface *surface, DDSCAPS *caps, IDirectDrawSurface **output)
{
    if (surface == front && back && caps && (caps->dwCaps & DDSCAPS_BACKBUFFER))
    {
        if (!output)
            return E_POINTER;
        back->AddRef();
        *output = back;
        return DD_OK;
    }
    return attachedOriginal(surface, caps, output);
}
void __fastcall Flip(void *object, void *)
{
    if (!enabled || !front || !back)
    {
        flipOriginal(object);
        return;
    }
    // The engine continues copying dirty regions front->back after this call.
    // Neither buffer is resized or used as a desktop-format primary surface.
    auto result = front->Blt(nullptr, back, nullptr, DDBLT_WAIT, nullptr);
    if (result == DDERR_SURFACELOST && SUCCEEDED(front->Restore()) && SUCCEEDED(back->Restore()))
        result = front->Blt(nullptr, back, nullptr, DDBLT_WAIT, nullptr);
    if (SUCCEEDED(result))
        Paint();
    else if (!reported)
    {
        char line[96]{};
        sprintf_s(line, "present: back-to-front copy failed HRESULT=%08lx; frame skipped",
                  static_cast<DWORD>(result));
        Log(line);
        reported = true;
    }
}
BOOL WINAPI Clip(const RECT *area)
{
    if (!enabled || !gameWindow || !area || GetForegroundWindow() != gameWindow)
        return clipOriginal(area);
    if (framed)
        return clipOriginal(nullptr); // Permit leaving/moving/resizing a normal window.
    RECT client{};
    GetClientRect(gameWindow, &client);
    POINT origin{};
    ClientToScreen(gameWindow, &origin);
    OffsetRect(&client, origin.x, origin.y);
    return clipOriginal(&client);
}
LRESULT __fastcall Message(void *object, void *, HWND window, UINT msg, WPARAM wp, LPARAM lp)
{
    if (enabled && framed && window == gameWindow && msg == WM_ACTIVATEAPP && !wp)
        haveFocusPlacement = GetWindowPlacement(window, &focusPlacement) != FALSE;
    if (enabled && window == gameWindow && msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST)
    {
        POINT point{static_cast<short>(LOWORD(lp)), static_cast<short>(HIWORD(lp))};
        bool screen = msg == WM_MOUSEWHEEL || msg == WM_MOUSEHWHEEL;
        if (screen)
            ScreenToClient(window, &point);
        POINT mapped{};
        if (!pc2fix::ViewPoint(View(window), IsNativeWorldFrame() ? layout.width : 800,
                               IsNativeWorldFrame() ? layout.height : 600, point, mapped))
            mapped = {-1, -1};
        if (screen)
            ClientToScreen(window, &mapped);
        lp = MAKELPARAM(static_cast<WORD>(mapped.x), static_cast<WORD>(mapped.y));
    }
    auto result = messageOriginal(object, window, msg, wp, lp);
    if (enabled && framed && window == gameWindow && msg == WM_ACTIVATEAPP && wp && haveFocusPlacement)
    {
        SetWindowPlacement(window, &focusPlacement);
        haveFocusPlacement = false;
    }
    if (enabled && window == gameWindow && msg == WM_PAINT)
        Paint();
    return result;
}
} // namespace
void ConfigurePresentation(const pc2fix::DisplayLayout &target, bool requested, bool windowed)
{
    layout = target;
    enabled = requested;
    framed = windowed;
}
bool PresentationEnabled()
{
    return enabled;
}
bool InstallPresentationHooks()
{
    if (!enabled)
        return true;
    return MH_CreateHook(reinterpret_cast<void *>(0x539860), &Flip,
                         reinterpret_cast<void **>(&flipOriginal)) == MH_OK &&
           MH_CreateHook(reinterpret_cast<void *>(0x5252e0), &Message,
                         reinterpret_cast<void **>(&messageOriginal)) == MH_OK &&
           MH_CreateHook(reinterpret_cast<void *>(&ClipCursor), &Clip,
                         reinterpret_cast<void **>(&clipOriginal)) == MH_OK;
}
HRESULT PresentationCooperativeLevel(IDirectDraw *draw, HWND window)
{
    auto result = draw->SetCooperativeLevel(window, DDSCL_NORMAL);
    if (FAILED(result) || gameWindow)
        return result;
    gameWindow = window;
    MONITORINFO monitor{sizeof(monitor)};
    if (!GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor))
        return E_FAIL;
    DWORD style = framed ? WS_OVERLAPPEDWINDOW : WS_POPUP;
    SetWindowLongW(window, GWL_STYLE, static_cast<LONG>(style));
    SetWindowLongW(window, GWL_EXSTYLE, 0);
    RECT bounds = monitor.rcMonitor;
    if (framed)
    {
        auto work = monitor.rcWork;
        auto area =
            pc2fix::FitView((work.right - work.left) * 9 / 10, (work.bottom - work.top) * 8 / 10, 1920, 1080);
        bounds = {0, 0, area.right - area.left, area.bottom - area.top};
        AdjustWindowRectEx(&bounds, style, FALSE, 0);
        int w = bounds.right - bounds.left, h = bounds.bottom - bounds.top;
        bounds = {work.left + (work.right - work.left - w) / 2, work.top + (work.bottom - work.top - h) / 2,
                  work.left + (work.right - work.left + w) / 2, work.top + (work.bottom - work.top + h) / 2};
    }
    if (!SetWindowPos(window, HWND_NOTOPMOST, bounds.left, bounds.top, bounds.right - bounds.left,
                      bounds.bottom - bounds.top, SWP_FRAMECHANGED | SWP_SHOWWINDOW))
        return E_FAIL;
    ClipCursor(nullptr);
    Log(framed ? "present: framed window; desktop mode retained"
               : "present: borderless fullscreen; desktop mode retained");
    return result;
}
HRESULT CreatePresentationSurface(IDirectDraw *draw, IDirectDrawSurface **output)
{
    if (!output)
        return E_POINTER;
    *output = nullptr;
    if (!draw || front || back)
        return E_INVALIDARG;
    DDSURFACEDESC desc{};
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
    desc.dwWidth = layout.width;
    desc.dwHeight = layout.height;
    desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    desc.ddpfPixelFormat.dwFlags = DDPF_RGB;
    desc.ddpfPixelFormat.dwRGBBitCount = 16;
    desc.ddpfPixelFormat.dwRBitMask = 0xf800;
    desc.ddpfPixelFormat.dwGBitMask = 0x7e0;
    desc.ddpfPixelFormat.dwBBitMask = 0x1f;
    auto result = draw->CreateSurface(&desc, &front, nullptr);
    if (SUCCEEDED(result))
        result = draw->CreateSurface(&desc, &back, nullptr);
    if (SUCCEEDED(result))
    {
        auto table = *reinterpret_cast<void ***>(front);
        auto target = table[12]; // IDirectDrawSurface::GetAttachedSurface, vtable +0x30.
        if (MH_CreateHook(target, &Attached, reinterpret_cast<void **>(&attachedOriginal)) != MH_OK ||
            MH_EnableHook(target) != MH_OK)
            result = E_FAIL;
    }
    if (FAILED(result))
    {
        if (front)
            front->Release();
        if (back)
            back->Release();
        front = nullptr;
        back = nullptr;
        return result;
    }
    *output = front; // Game owns front; presenter holds back until DirectDraw teardown.
    Log("present: two 1920x1080 RGB565 system-memory buffers allocated");
    return DD_OK;
}
void ReleasePresentation()
{
    if (back)
        back->Release();
    back = nullptr;
    front = nullptr;
    gameWindow = nullptr;
}

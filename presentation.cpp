// MIT license; see LICENSE. Optional output backend for the verified GOG image.
#define WIN32_LEAN_AND_MEAN
#include "presentation.h"
#include "main.h"
#include "presentation_buffer.h"
#include "performance_probe.h"
#include "presentation_surfaces.h"
#include "frame_pipeline.h"
#include <MinHook.h>
#include <cstdio>
namespace
{
pc2fix::DisplayLayout layout{};
bool enabled = false, framed = false;
HWND gameWindow = nullptr;
pc2fix::PresentationSurfaces<IDirectDrawSurface> surfaces;
auto &front = surfaces.front;
auto &back = surfaces.back;
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
pc2fix::FramePipeline framePipeline;
pc2fix::PresentationBuffer outputBuffer;
bool painting = false;
bool performanceProbe = false;
LONGLONG performanceFrequency = 0, performanceWindow = 0, previousFlip = 0;
pc2fix::FrameStats performanceStats{};
LONGLONG Stamp()
{
    LARGE_INTEGER counter{};
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
}
double Elapsed(LONGLONG start, LONGLONG end)
{
    return pc2fix::TickMilliseconds(start, end, performanceFrequency);
}
void RecordPerformance(pc2fix::FrameSample sample, LONGLONG start)
{
    auto end = Stamp();
    if (previousFlip)
        performanceStats.gap.Add(Elapsed(previousFlip, start));
    previousFlip = start;
    sample.total = Elapsed(start, end);
    performanceStats.Add(sample);
    if (Elapsed(performanceWindow, end) < 5000)
        return;
    auto &s = performanceStats;
    char line[512]{};
    sprintf_s(line,
              "perf: frames=%u completed=%u over50ms=%u avg/max(ms) gap=%.2f/%.2f copy=%.2f/%.2f "
              "lock=%.2f/%.2f compose=%.2f/%.2f publish=%.2f/%.2f total=%.2f/%.2f",
              s.frames, s.completed, s.over50ms, s.gap.Average(), s.gap.peak, s.copy.Average(), s.copy.peak,
              s.lock.Average(), s.lock.peak, s.compose.Average(), s.compose.peak,
              s.publish.Average(), s.publish.peak, s.total.Average(), s.total.peak);
    Log(line);
    performanceStats = {};
    performanceWindow = end;
}
void Paint(HDC paintDC = nullptr, pc2fix::FrameSample *sample = nullptr)
{
    const auto image = framePipeline.Front();
    if (painting || !front || !image.valid || !IsWindow(gameWindow) || IsIconic(gameWindow))
        return;
    painting = true;
    DDSURFACEDESC data{};
    data.dwSize = sizeof(data);
    auto phaseStart = sample ? Stamp() : 0;
    auto result = front->Lock(nullptr, &data, DDLOCK_WAIT | DDLOCK_READONLY, nullptr);
    if (sample)
        sample->lock = Elapsed(phaseStart, Stamp());
    if (SUCCEEDED(result))
    {
        if (data.lPitch >= layout.width * 2 && data.lPitch % 4 == 0 &&
            data.ddpfPixelFormat.dwRGBBitCount == 16)
        {
            RECT client{};
            GetClientRect(gameWindow, &client);
            phaseStart = sample ? Stamp() : 0;
            bool enlarged = false;
            const bool composed = outputBuffer.Compose(data.lpSurface, data.lPitch, layout, image.native,
                                                       data.ddpfPixelFormat.dwRBitMask, data.ddpfPixelFormat.dwGBitMask,
                                                       data.ddpfPixelFormat.dwBBitMask, client,
                                                       image.interior ? &image.room : nullptr, enlarged);
            if (sample)
                sample->compose = Elapsed(phaseStart, Stamp());
            if (composed)
            {
                phaseStart = sample ? Stamp() : 0;
                auto dc = paintDC ? paintDC : GetDC(gameWindow);
                bool presented = dc && outputBuffer.Present(dc);
                if (dc && !paintDC)
                    ReleaseDC(gameWindow, dc);
                if (sample)
                {
                    sample->publish = Elapsed(phaseStart, Stamp());
                    sample->completed = presented;
                }
                if (presented)
                {
                    framePipeline.Published(image, {client.right, client.bottom}, enlarged);
                    static bool logged = false;
                    if (!logged)
                    {
                        Log("present: composed frame published with one BitBlt");
                        logged = true;
                    }
                }
                else if (!reported)
                {
                    Log("present: completed frame transfer failed; input mapping retained");
                    reported = true;
                }
            }
            else if (!reported)
            {
                Log("present: offscreen composition failed; no partial frame published");
                reported = true;
            }
        }
        else if (!reported)
        {
            Log("present: unexpected RGB16 format/pitch; frame skipped");
            reported = true;
        }
        front->Unlock(data.lpSurface);
    }
    else if (!reported)
    {
        Log("present: front lock failed; frame skipped");
        reported = true;
    }
    painting = false;
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
    // Window/GDI calls may dispatch messages while Paint holds the front lock.
    // Defer a nested flip rather than blitting into that locked surface.
    if (painting)
        return;
    auto performanceStart = performanceProbe ? Stamp() : 0;
    pc2fix::FrameSample sample{};
    const auto image = framePipeline.Back();
    auto result = front->Blt(nullptr, back, nullptr, DDBLT_WAIT, nullptr);
    if (result == DDERR_SURFACELOST && SUCCEEDED(front->Restore()) && SUCCEEDED(back->Restore()))
        result = front->Blt(nullptr, back, nullptr, DDBLT_WAIT, nullptr);
    if (performanceProbe)
        sample.copy = Elapsed(performanceStart, Stamp());
    if (SUCCEEDED(result))
    {
        framePipeline.Copied(image);
        Paint(nullptr, performanceProbe ? &sample : nullptr);
    }
    else if (!reported)
    {
        char line[96]{};
        sprintf_s(line, "present: back-to-front copy failed HRESULT=%08lx; frame skipped",
                  static_cast<DWORD>(result));
        Log(line);
        reported = true;
    }
    if (performanceProbe)
        RecordPerformance(sample, performanceStart);
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
    // The original WM_PAINT path copies the uncomposed legacy image (0x5246e0).
    // Validate the update region ourselves and publish only the finished frame.
    if (enabled && front && window == gameWindow && msg == WM_PAINT)
    {
        PAINTSTRUCT paint{};
        auto dc = BeginPaint(window, &paint);
        if (dc)
            Paint(dc);
        EndPaint(window, &paint);
        return 0;
    }
    if (enabled && front && window == gameWindow && msg == WM_ERASEBKGND)
        return 1; // Composition covers the client, including all letterbox bars.
    if (enabled && framed && window == gameWindow && msg == WM_ACTIVATEAPP && !wp)
        haveFocusPlacement = GetWindowPlacement(window, &focusPlacement) != FALSE;
    if (enabled && window == gameWindow && msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST)
    {
        POINT point{static_cast<short>(LOWORD(lp)), static_cast<short>(HIWORD(lp))};
        bool screen = msg == WM_MOUSEWHEEL || msg == WM_MOUSEHWHEEL;
        if (screen)
            ScreenToClient(window, &point);
        POINT mapped{};
        RECT client{}, current{};
        GetClientRect(window, &client);
        const bool geometryMatches = !framePipeline.Enlarged() ||
            (RestaurantArtwork(current) && framePipeline.ArtworkMatches(current));
        if (!framePipeline.Point(layout, {client.right, client.bottom}, geometryMatches, point, mapped))
            mapped = {-1, -1};
        if (screen)
            ClientToScreen(window, &mapped);
        lp = MAKELPARAM(static_cast<WORD>(mapped.x), static_cast<WORD>(mapped.y));
    }
    auto result = messageOriginal(object, window, msg, wp, lp);
    if (enabled && framed && window == gameWindow && msg == WM_ACTIVATEAPP && wp && haveFocusPlacement)
    {
        // Deactivation may arrive after SC_MINIMIZE has changed showCmd.
        // Reapplying that snapshot would immediately minimize an activated game.
        auto restore = pc2fix::FocusRestorePlacement(focusPlacement);
        SetWindowPlacement(window, &restore);
        haveFocusPlacement = false;
    }
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
void BeginPresentationScene()
{
    if (enabled)
        framePipeline.BeginScene();
}
uint64_t PresentationSceneGeneration()
{
    return framePipeline.Generation();
}
void CapturePresentationFrame(uint64_t generation, bool native)
{
    if (!enabled)
        return;
    RECT artwork{};
    const bool interior = native && RestaurantArtwork(artwork);
    framePipeline.Rendered(generation, native, layout, interior ? &artwork : nullptr);
}
void ConfigurePerformanceProbe(bool requested)
{
    LARGE_INTEGER frequency{};
    performanceProbe = requested && QueryPerformanceFrequency(&frequency) && frequency.QuadPart > 0;
    performanceFrequency = performanceProbe ? frequency.QuadPart : 0;
    performanceStats = {};
    previousFlip = 0;
    performanceWindow = performanceProbe ? Stamp() : 0;
    if (performanceProbe)
        Log("perf: optional frontend timing probe active; reports every five seconds");
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
        if (MH_CreateHook(target, &Attached, reinterpret_cast<void **>(&attachedOriginal)) != MH_OK)
            result = E_FAIL;
        else if (MH_EnableHook(target) != MH_OK)
        {
            MH_RemoveHook(target); // Do not retain a half-installed vtable detour.
            attachedOriginal = nullptr;
            result = E_FAIL;
        }
    }
    if (FAILED(result))
    {
        surfaces.Reset();
        return result;
    }
    *output = surfaces.FrontForGame(); // Game and presenter hold independent COM references.
    Log("present: two 1920x1080 RGB565 system-memory buffers allocated");
    return DD_OK;
}
void ReleasePresentation()
{
    ConfigurePerformanceProbe(false);
    framePipeline.Reset();
    outputBuffer.Reset();
    gameWindow = nullptr;
    surfaces.Reset();
}

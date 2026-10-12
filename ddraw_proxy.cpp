// MIT license; see LICENSE. This proxy targets the verified PC2 import set only.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ddraw.h>
#include <cstdio>
#include <new>
#include "main.h"
#include "display_layout.h"
#include "widescreen.h"
#include "presentation.h"
namespace {
using CreateFn = HRESULT (WINAPI*)(GUID*, LPDIRECTDRAW*, IUnknown*);
INIT_ONCE init = INIT_ONCE_STATIC_INIT;
CreateFn systemCreate = nullptr;
bool known = false;
bool probe = false;
bool widescreen = false;
volatile LONG probeClaimed = 0;
bool dimensionsApplied = false;
void RestoreDimensions()
{
    StopWidescreen();
    ConfigurePresentation({}, false, false);
    ConfigureFrameLimiter(0);
    *reinterpret_cast<DWORD*>(0x574734) = 800;
    *reinterpret_cast<DWORD*>(0x574738) = 600;
    *reinterpret_cast<RECT*>(0x573d38) = {0,0,800,600};
    dimensionsApplied = false;
}
pc2fix::DisplayLayout layout{};
BOOL CALLBACK Initialise(PINIT_ONCE, PVOID, PVOID*)
{
    // Lazy initialisation runs in DirectDrawCreate, outside the loader lock.
    wchar_t path[32768]{};
    auto length = GetSystemDirectoryW(path, _countof(path));
    if (!length || length >= _countof(path) ||
        wcscat_s(path, L"\\ddraw.dll")) return TRUE;
    auto module = LoadLibraryExW(path, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (module) systemCreate = reinterpret_cast<CreateFn>(GetProcAddress(module, "DirectDrawCreate"));
    if (!systemCreate) { Log("ddraw: system DirectDrawCreate unavailable"); return TRUE; }
    known = IsSupportedGame();
    if (!known) { Log("ddraw: unknown host; unmodified system forwarding"); return TRUE; }
    length = GetModuleFileNameW(nullptr, path, _countof(path));
    auto slash = length && length < _countof(path) ? wcsrchr(path, L'\\') : nullptr;
    if (!slash || wcscpy_s(slash + 1, _countof(path) - (slash + 1 - path), L"pc2fix.ini")) return TRUE;
    probe = GetPrivateProfileIntW(L"display", L"RenderProbe", 0, path) == 1;
    widescreen = GetPrivateProfileIntW(L"display", L"Widescreen", 0, path) == 1;
    probe = probe || widescreen;
    auto width = GetPrivateProfileIntW(L"display", L"Width", 1920, path);
    auto height = GetPrivateProfileIntW(L"display", L"Height", 1080, path);
    if (widescreen && !pc2fix::SupportedWidescreenSize(static_cast<int>(width), static_cast<int>(height))) {
        probe = false; Log("ddraw: unsupported widescreen dimensions; original mode retained");
    }
    if (!pc2fix::MakeLayout(static_cast<int>(width), static_cast<int>(height), layout)) {
        probe = false; Log("ddraw: invalid dimensions; original mode retained");
    }
    bool windowed = GetPrivateProfileIntW(L"display", L"Windowed", 0, path) == 1;
    bool presentation = windowed || GetPrivateProfileIntW(L"display", L"Presentation", 0, path) == 1;
    // This backend is deliberately restricted to the requested/tested Full-HD mode.
    ConfigurePresentation(layout, presentation && widescreen && probe && width == 1920 && height == 1080, windowed);
    ConfigurePerformanceProbe(PresentationEnabled() && GetPrivateProfileIntW(L"display", L"PerformanceProbe", 0, path) == 1);
    ConfigureFrameLimiter(static_cast<int>(GetPrivateProfileIntW(L"display", L"TargetFPS", 60, path)));
    Log(probe ? (widescreen ? "ddraw: opt-in native widescreen layout" :
        "ddraw: opt-in render probe; camera/UI unchanged") : "ddraw: diagnostics only; original resolution retained");
    return TRUE;
}
class Draw final : public IDirectDraw {
    IDirectDraw* original;
    bool firstMode = true;
    bool allowFallback;
    bool ownsPresentation;
    volatile LONG references = 1;
    bool UsesPresentation() const { return ownsPresentation && PresentationEnabled() && dimensionsApplied; }
public:
    Draw(IDirectDraw* object, bool initialProbe) : original(object), allowFallback(initialProbe), ownsPresentation(initialProbe) {}
    HRESULT WINAPI QueryInterface(REFIID id, void** output) override {
        // The game does not query other interfaces. Preserve COM identity for IUnknown/IDirectDraw.
        static const GUID drawId = {0x6c14db80,0xa733,0x11ce,{0xa5,0x21,0,0x20,0xaf,0x0b,0xe5,0x60}};
        static const GUID unknownId = {0,0,0,{0xc0,0,0,0,0,0,0,0x46}};
        if (!output) return E_POINTER;
        if (IsEqualGUID(id, drawId) || IsEqualGUID(id, unknownId)) {
            *output = static_cast<IDirectDraw*>(this); AddRef(); return S_OK;
        }
        // Returning a different underlying interface could bypass the probe's diagnostics.
        *output = nullptr; return E_NOINTERFACE;
    }
    ULONG WINAPI AddRef() override { return static_cast<ULONG>(InterlockedIncrement(&references)); }
    ULONG WINAPI Release() override {
        auto count = InterlockedDecrement(&references);
        if (!count) { if (ownsPresentation) ReleasePresentation(); original->Release(); delete this; }
        return static_cast<ULONG>(count);
    }
    HRESULT WINAPI Compact() override { return original->Compact(); }
    HRESULT WINAPI CreateClipper(DWORD f, LPDIRECTDRAWCLIPPER* p, IUnknown* u) override { return original->CreateClipper(f,p,u); }
    HRESULT WINAPI CreatePalette(DWORD f, LPPALETTEENTRY e, LPDIRECTDRAWPALETTE* p, IUnknown* u) override { return original->CreatePalette(f,e,p,u); }
    HRESULT WINAPI CreateSurface(LPDDSURFACEDESC d, LPDIRECTDRAWSURFACE* p, IUnknown* u) override {
        auto result = UsesPresentation() && d && (d->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
            ? CreatePresentationSurface(original,p) : original->CreateSurface(d,p,u);
        if (SUCCEEDED(result)) firstMode = false; // Never resize existing surfaces via fallback.
        if (SUCCEEDED(result) && p && *p && d && (d->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)) {
            DDSURFACEDESC actual{}; actual.dwSize = sizeof(actual);
            if (SUCCEEDED((*p)->GetSurfaceDesc(&actual))) {
                char message[160]{};
                sprintf_s(message,"ddraw: primary surface %lux%lu pitch=%ld caps=%08lx",
                    actual.dwWidth,actual.dwHeight,actual.lPitch,actual.ddsCaps.dwCaps); Log(message);
            }
            DDSCAPS caps{}; caps.dwCaps = DDSCAPS_BACKBUFFER;
            IDirectDrawSurface* back = nullptr;
            if (SUCCEEDED((*p)->GetAttachedSurface(&caps, &back)) && back) {
                actual = {}; actual.dwSize = sizeof(actual);
                if (SUCCEEDED(back->GetSurfaceDesc(&actual))) {
                    char message[128]{};
                    sprintf_s(message,"ddraw: attached backbuffer %lux%lu pitch=%ld",
                        actual.dwWidth,actual.dwHeight,actual.lPitch); Log(message);
                }
                back->Release();
            }
        }
        return result;
    }
    HRESULT WINAPI DuplicateSurface(LPDIRECTDRAWSURFACE s, LPDIRECTDRAWSURFACE* p) override { return original->DuplicateSurface(s,p); }
    HRESULT WINAPI EnumDisplayModes(DWORD f, LPDDSURFACEDESC d, LPVOID c, LPDDENUMMODESCALLBACK cb) override { return original->EnumDisplayModes(f,d,c,cb); }
    HRESULT WINAPI EnumSurfaces(DWORD f, LPDDSURFACEDESC d, LPVOID c, LPDDENUMSURFACESCALLBACK cb) override { return original->EnumSurfaces(f,d,c,cb); }
    HRESULT WINAPI FlipToGDISurface() override { return original->FlipToGDISurface(); }
    HRESULT WINAPI GetCaps(LPDDCAPS d, LPDDCAPS h) override { return original->GetCaps(d,h); }
    HRESULT WINAPI GetDisplayMode(LPDDSURFACEDESC d) override { return original->GetDisplayMode(d); }
    HRESULT WINAPI GetFourCCCodes(LPDWORD n, LPDWORD c) override { return original->GetFourCCCodes(n,c); }
    HRESULT WINAPI GetGDISurface(LPDIRECTDRAWSURFACE* p) override { return original->GetGDISurface(p); }
    HRESULT WINAPI GetMonitorFrequency(LPDWORD f) override { return original->GetMonitorFrequency(f); }
    HRESULT WINAPI GetScanLine(LPDWORD l) override { return original->GetScanLine(l); }
    HRESULT WINAPI GetVerticalBlankStatus(LPBOOL b) override { return original->GetVerticalBlankStatus(b); }
    HRESULT WINAPI Initialize(GUID* g) override { return original->Initialize(g); }
    HRESULT WINAPI RestoreDisplayMode() override { return original->RestoreDisplayMode(); }
    HRESULT WINAPI SetCooperativeLevel(HWND window, DWORD flags) override {
        return UsesPresentation() ? PresentationCooperativeLevel(original,window) :
            original->SetCooperativeLevel(window,flags);
    }
    HRESULT WINAPI SetDisplayMode(DWORD w, DWORD h, DWORD bits) override {
        auto result = UsesPresentation() ? DD_OK : original->SetDisplayMode(w,h,bits);
        char message[160]{};
        sprintf_s(message,"ddraw: %s %lux%lux%lu HRESULT=%08lx",
            UsesPresentation() ? "virtual render mode (desktop retained)" : "SetDisplayMode",
            w,h,bits,static_cast<DWORD>(result));
        Log(message);
        // Fallback is safe only for the FIRST mode request, before any surface.
        if (firstMode && allowFallback && dimensionsApplied && w == static_cast<DWORD>(layout.width) &&
            h == static_cast<DWORD>(layout.height) && FAILED(result)) {
            RestoreDimensions();
            Log("ddraw: initial probe mode rejected; retrying original 800x600");
            result = original->SetDisplayMode(800,600,bits);
            sprintf_s(message,"ddraw: fallback HRESULT=%08lx",static_cast<DWORD>(result)); Log(message);
        }
        firstMode = false;
        return result;
    }
    HRESULT WINAPI WaitForVerticalBlank(DWORD flags, HANDLE event) override { return original->WaitForVerticalBlank(flags,event); }
};
bool ApplyProbe()
{
    // Data and optional verified in-memory detours are installed before surfaces
    // exist. Original files remain untouched; unknown images never reach here.
    auto width = reinterpret_cast<DWORD*>(0x574734);
    auto height = reinterpret_cast<DWORD*>(0x574738);
    auto rect = reinterpret_cast<RECT*>(0x573d38);
    constexpr DWORD addresses[] = {0x574734u, 0x574738u, 0x573d38u, 0x563860u, 0x57c5c8u};
    for (auto address : addresses) {
        MEMORY_BASIC_INFORMATION region{};
        if (!VirtualQuery(reinterpret_cast<void*>(address), &region, sizeof(region)) ||
            region.State != MEM_COMMIT || (region.Protect & (PAGE_GUARD | PAGE_NOACCESS)) ||
            ((region.Protect & 0xff) != PAGE_READWRITE && (region.Protect & 0xff) != PAGE_WRITECOPY) ||
            reinterpret_cast<ULONG_PTR>(region.BaseAddress) + region.RegionSize - address < sizeof(RECT)) {
            Log("ddraw: game data page unavailable or not writable; render probe refused"); return false;
        }
    }
    if (*reinterpret_cast<DWORD*>(0x563860) != 1 ||
        *reinterpret_cast<DWORD*>(0x57c5c8) != 0 ||
        *width != 800 || *height != 600 || rect->left || rect->top ||
        rect->right != 800 || rect->bottom != 600) {
        Log("ddraw: unexpected startup geometry; render probe refused"); return false;
    }
    *width = layout.width; *height = layout.height;
    *rect = {0,0,layout.width,layout.height};
    dimensionsApplied = true;
    if (widescreen && !InstallWidescreen(layout)) { RestoreDimensions(); return false; }
    Log(widescreen ? "ddraw: render/layout dimensions installed before initial display mode" :
        "ddraw: render dimensions set before initial display mode; camera/UI unchanged");
    return true;
}
}
extern "C" HRESULT WINAPI DirectDrawCreate(GUID* guid, LPDIRECTDRAW* output, IUnknown* outer)
{
    if (!InitOnceExecuteOnce(&init, Initialise, nullptr, nullptr) || !systemCreate) return E_FAIL;
    const bool appliedNow = known && probe && output && !outer &&
        InterlockedCompareExchange(&probeClaimed, 1, 0) == 0 && ApplyProbe();
    auto result = systemCreate(guid,output,outer);
    if (known && SUCCEEDED(result) && output && *output && !outer) {
        auto wrapper = new (std::nothrow) Draw(*output, appliedNow);
        if (wrapper) *output = wrapper;
        else {
            if (appliedNow) RestoreDimensions();
            Log("ddraw: wrapper allocation failed; original dimensions and system object retained");
        }
    }
    if (FAILED(result) && appliedNow) RestoreDimensions();
    return result;
}

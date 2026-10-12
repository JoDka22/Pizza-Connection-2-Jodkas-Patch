// MIT license; see LICENSE. Exercise the actual proxy with synthetic COM objects.
#include "../ddraw_proxy.cpp"
#include "../presentation_surfaces.h"
namespace
{
bool testEnabled = true;
int presentationCalls = 0, teardownCalls = 0;
class FakeDraw final : public IDirectDraw
{
  public:
    int references = 1, modeCalls = 0, cooperativeCalls = 0, surfaceCalls = 0;
    HRESULT WINAPI QueryInterface(REFIID, void **p) override { if (p) *p = nullptr; return E_NOINTERFACE; }
    ULONG WINAPI AddRef() override { return ++references; }
    ULONG WINAPI Release() override { return --references; }
    HRESULT WINAPI SetCooperativeLevel(HWND, DWORD) override { ++cooperativeCalls; return DD_OK; }
    HRESULT WINAPI SetDisplayMode(DWORD, DWORD, DWORD) override { ++modeCalls; return DD_OK; }
    HRESULT WINAPI CreateSurface(LPDDSURFACEDESC, LPDIRECTDRAWSURFACE *p, IUnknown *) override
    { ++surfaceCalls; if (p) *p = nullptr; return E_FAIL; }
    HRESULT WINAPI Compact() override { return E_NOTIMPL; }
    HRESULT WINAPI CreateClipper(DWORD, LPDIRECTDRAWCLIPPER *, IUnknown *) override { return E_NOTIMPL; }
    HRESULT WINAPI CreatePalette(DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown *) override { return E_NOTIMPL; }
    HRESULT WINAPI DuplicateSurface(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE *) override { return E_NOTIMPL; }
    HRESULT WINAPI EnumDisplayModes(DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK) override { return E_NOTIMPL; }
    HRESULT WINAPI EnumSurfaces(DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMSURFACESCALLBACK) override { return E_NOTIMPL; }
    HRESULT WINAPI FlipToGDISurface() override { return E_NOTIMPL; }
    HRESULT WINAPI GetCaps(LPDDCAPS, LPDDCAPS) override { return E_NOTIMPL; }
    HRESULT WINAPI GetDisplayMode(LPDDSURFACEDESC) override { return E_NOTIMPL; }
    HRESULT WINAPI GetFourCCCodes(LPDWORD, LPDWORD) override { return E_NOTIMPL; }
    HRESULT WINAPI GetGDISurface(LPDIRECTDRAWSURFACE *) override { return E_NOTIMPL; }
    HRESULT WINAPI GetMonitorFrequency(LPDWORD) override { return E_NOTIMPL; }
    HRESULT WINAPI GetScanLine(LPDWORD) override { return E_NOTIMPL; }
    HRESULT WINAPI GetVerticalBlankStatus(LPBOOL) override { return E_NOTIMPL; }
    HRESULT WINAPI Initialize(GUID *) override { return E_NOTIMPL; }
    HRESULT WINAPI RestoreDisplayMode() override { return E_NOTIMPL; }
    HRESULT WINAPI WaitForVerticalBlank(DWORD, HANDLE) override { return E_NOTIMPL; }
};
struct FakeSurface
{
    int references = 1;
    bool releasedWhileVisible = false;
    pc2fix::PresentationSurfaces<FakeSurface> *pair = nullptr;
    void AddRef() { ++references; }
    void Release()
    {
        if (pair && (pair->front || pair->back)) releasedWhileVisible = true;
        --references;
    }
};
}
void Log(const char *) {}
bool IsSupportedGame() { return false; }
bool InstallWidescreen(const pc2fix::DisplayLayout &) { return false; }
void StopWidescreen() {}
void ConfigurePresentation(const pc2fix::DisplayLayout &, bool, bool) {}
bool PresentationEnabled() { return testEnabled; }
void ConfigurePerformanceProbe(bool) {}
void ConfigureFrameLimiter(int) {}
HRESULT CreatePresentationSurface(IDirectDraw *, IDirectDrawSurface **p)
{ ++presentationCalls; if (p) *p = nullptr; return E_FAIL; }
HRESULT PresentationCooperativeLevel(IDirectDraw *, HWND) { ++presentationCalls; return DD_OK; }
void ReleasePresentation() { ++teardownCalls; }
int main()
{
    int failures = 0;
    auto check = [&](bool pass, const char *name) {
        printf("%s: %s\n", pass ? "PASS" : "FAIL", name);
        failures += !pass;
    };
    pc2fix::PresentationSurfaces<FakeSurface> pair;
    FakeSurface frontSurface, backSurface;
    pair.front = &frontSurface; pair.back = &backSurface;
    auto game = pair.FrontForGame();
    check(game == &frontSurface && frontSurface.references == 2 && backSurface.references == 1,
          "game front reference is independent of presenter creation references");
    game->Release();
    check(frontSurface.references == 1 && pair.front == &frontSurface,
          "game-first front release leaves presenter surface alive");
    frontSurface.pair = backSurface.pair = &pair;
    pair.Reset();
    check(frontSurface.references == 0 && backSurface.references == 0 &&
              !frontSurface.releasedWhileVisible && !backSurface.releasedWhileVisible,
          "teardown detaches both pointers before reentrant COM release");
    pair.Reset();
    check(frontSurface.references == 0 && backSurface.references == 0 && !pair.FrontForGame(),
          "repeated teardown does not double-release surfaces");
    FakeSurface lateFront, lateBack;
    pair.front = &lateFront; pair.back = &lateBack; game = pair.FrontForGame();
    pair.Reset();
    check(lateFront.references == 1 && lateBack.references == 0,
          "presenter-first teardown preserves outstanding game reference");
    game->Release();
    check(lateFront.references == 0, "last game reference releases front after presenter teardown");
    FakeSurface partial;
    pair.front = &partial; pair.Reset();
    check(partial.references == 0 && !pair.front && !pair.back,
          "partial surface construction releases its creation reference");
    dimensionsApplied = true;
    DDSURFACEDESC desc{}; desc.dwSize = sizeof(desc); desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    IDirectDrawSurface *surface = nullptr;
    FakeDraw secondary;
    auto wrapper = new Draw(&secondary, false);
    wrapper->SetCooperativeLevel(nullptr, DDSCL_NORMAL);
    wrapper->SetDisplayMode(800, 600, 16);
    wrapper->CreateSurface(&desc, &surface, nullptr);
    check(secondary.cooperativeCalls == 1 && secondary.modeCalls == 1 && secondary.surfaceCalls == 1 &&
              presentationCalls == 0,
          "secondary DirectDraw object forwards all display calls despite global presenter enable");
    wrapper->Release();
    check(secondary.references == 0 && teardownCalls == 0,
          "secondary DirectDraw release cannot tear down game presenter");
    FakeDraw owner;
    wrapper = new Draw(&owner, true);
    wrapper->SetCooperativeLevel(nullptr, DDSCL_NORMAL);
    wrapper->SetDisplayMode(1920, 1080, 16);
    wrapper->CreateSurface(&desc, &surface, nullptr);
    check(owner.cooperativeCalls == 0 && owner.modeCalls == 0 && owner.surfaceCalls == 0 &&
              presentationCalls == 2,
          "claimed game DirectDraw object retains virtual mode and presentation surfaces");
    wrapper->Release();
    check(owner.references == 0 && teardownCalls == 1, "only owner releases presentation exactly once");
    return failures ? 1 : 0;
}

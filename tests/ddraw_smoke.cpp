#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ddraw.h>
#include <cstdio>
int main()
{
    // Unknown host: exercise unmodified system forwarding without changing display mode.
    HMODULE module = LoadLibraryW(L"ddraw.dll");
    using CreateFn = HRESULT (WINAPI*)(GUID*,LPDIRECTDRAW*,IUnknown*);
    auto create = module ? reinterpret_cast<CreateFn>(GetProcAddress(module,"DirectDrawCreate")) : nullptr;
    IDirectDraw* draw = nullptr;
    HRESULT result = create ? create(nullptr,&draw,nullptr) : E_FAIL;
    if (FAILED(result) || !draw) { printf("FAIL: DirectDrawCreate %08lx\n",static_cast<DWORD>(result)); return 1; }
    DDSURFACEDESC mode{}; mode.dwSize = sizeof(mode);
    result = draw->GetDisplayMode(&mode);
    bool ok = SUCCEEDED(result) && mode.dwWidth && mode.dwHeight;
    printf("%s: unknown-host forwarding; current mode %lux%lu\n",ok ? "PASS" : "FAIL",mode.dwWidth,mode.dwHeight);
    draw->Release();
    // DLLs stay loaded until process exit, matching the supported game lifetime.
    return ok ? 0 : 1;
}

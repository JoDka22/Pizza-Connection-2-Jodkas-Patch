#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <cwchar>
#include "main.h"
namespace {
INIT_ONCE once = INIT_ONCE_STATIC_INIT;
using SetSoundSystemFn = int (WINAPI*)(void*, unsigned);
SetSoundSystemFn originalSetSoundSystem = nullptr;
HINSTANCE proxyModule = nullptr;
BOOL CALLBACK Initialise(PINIT_ONCE, PVOID, PVOID*)
{
    // A caller may resolve this wrapper before any PE forwarder has loaded the original.
    // Resolve the sibling explicitly in that case, outside DllMain and without CWD search.
    auto original = GetModuleHandleW(L"binkw32o.dll");
    if (!original) {
        wchar_t path[32768]{};
        DWORD length = GetModuleFileNameW(proxyModule, path, ARRAYSIZE(path));
        auto slash = length && length < ARRAYSIZE(path) ? std::wcsrchr(path, L'\\') : nullptr;
        if (slash && !wcscpy_s(slash + 1, ARRAYSIZE(path) - (slash + 1 - path), L"binkw32o.dll")) {
            original = LoadLibraryExW(path, nullptr,
                LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
        }
        if (!original) {
            char message[128]{};
            sprintf_s(message, "original sibling Bink DLL could not be loaded (Windows error %lu)", GetLastError());
            Log(message);
        }
    }
    if (original) originalSetSoundSystem = reinterpret_cast<SetSoundSystemFn>(
        GetProcAddress(original, "_BinkSetSoundSystem@8"));
    if (!originalSetSoundSystem) {
        Log("original BinkSetSoundSystem export missing; audio initialisation rejected");
        return TRUE;
    }
    Main();
    return TRUE;
}
}
extern "C" int WINAPI BinkSetSoundSystem(void* openSound, unsigned parameter)
{
    if (!InitOnceExecuteOnce(&once, Initialise, nullptr, nullptr) || !originalSetSoundSystem) return 0;
    return originalSetSoundSystem(openSound, parameter);
}
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) proxyModule = instance;
    return TRUE;
}

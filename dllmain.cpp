#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "main.h"
namespace {
INIT_ONCE once = INIT_ONCE_STATIC_INIT;
using SetSoundSystemFn = int (WINAPI*)(void*, unsigned);
SetSoundSystemFn originalSetSoundSystem = nullptr;
BOOL CALLBACK Initialise(PINIT_ONCE, PVOID, PVOID*)
{
    // Forwarders resolve the original before game entry. This runs outside our DllMain.
    auto original = GetModuleHandleW(L"binkw32o.dll");
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
BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
int main() {
    auto proxy = LoadLibraryW(L"binkw32.dll");
    if (!proxy) { printf("LoadLibrary failed: %lu\n", GetLastError()); return 1; }
    auto getError = reinterpret_cast<const char* (WINAPI*)()>(GetProcAddress(proxy, "_BinkGetError@0"));
    if (!getError) return 2;
    const char* message = getError();
    if (!message) return 3;
    printf("PASS: original BinkGetError forwarding returned a string\n");
    auto setSound = reinterpret_cast<int (WINAPI*)(void*, unsigned)>(GetProcAddress(proxy, "_BinkSetSoundSystem@8"));
    if (!setSound) return 4;
    // Null selects no sound callback; this tests bootstrap rejection in an unknown host.
    setSound(nullptr, 0);
    setSound(nullptr, 0);
    printf("PASS: repeated BinkSetSoundSystem calls completed in unknown host\n");
    FreeLibrary(proxy);
    return 0;
}

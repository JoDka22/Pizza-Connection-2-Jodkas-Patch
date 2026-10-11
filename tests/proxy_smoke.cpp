#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <cstring>
int main(int argc, char** argv) {
    auto proxy = LoadLibraryW(L"binkw32.dll");
    if (!proxy) { printf("LoadLibrary failed: %lu\n", GetLastError()); return 1; }
    auto setSound = reinterpret_cast<int (WINAPI*)(void*, unsigned)>(GetProcAddress(proxy, "_BinkSetSoundSystem@8"));
    if (!setSound) return 4;
    // Exercise the local wrapper BEFORE resolving any forwarded export.
    const auto result = setSound(nullptr, 0);
    if (argc == 2 && std::strcmp(argv[1], "--missing-original") == 0) {
        if (result != 0 || GetModuleHandleW(L"binkw32o.dll")) return 6;
        printf("PASS: missing original Bink rejected without crashing\n");
        FreeLibrary(proxy);
        return 0;
    }
    if (!GetModuleHandleW(L"binkw32o.dll")) {
        printf("FAIL: first wrapper call did not load original Bink\n");
        return 5;
    }
    printf("PASS: first wrapper call loaded sibling Bink before resolving forwarders\n");
    auto getError = reinterpret_cast<const char* (WINAPI*)()>(GetProcAddress(proxy, "_BinkGetError@0"));
    if (!getError) return 2;
    const char* message = getError();
    if (!message) return 3;
    printf("PASS: original BinkGetError forwarding returned a string\n");
    // Null selects no sound callback; this tests bootstrap rejection in an unknown host.
    setSound(nullptr, 0);
    setSound(nullptr, 0);
    printf("PASS: repeated BinkSetSoundSystem calls completed in unknown host\n");
    FreeLibrary(proxy);
    return 0;
}

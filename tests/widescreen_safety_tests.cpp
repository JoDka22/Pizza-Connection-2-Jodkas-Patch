#define WIN32_LEAN_AND_MEAN
#include "../main.cpp"
#include "../widescreen.cpp"
#include "../presentation.cpp"
#include <cstdio>
#include <windows.h>
int main()
{
    pc2fix::DisplayLayout testLayout{};
    if (!pc2fix::MakeLayout(1920, 1080, testLayout))
        return 1;
    ConfigurePresentation(testLayout, true, true);
    // This executable is deliberately not the game. No fixed address may be read,
    // written or detoured, even when the display feature is explicitly requested.
    if (InstallWidescreen(testLayout))
        return 2;
    if (MH_Uninitialize() != MH_ERROR_NOT_INITIALIZED)
        return 3;
    StopWidescreen();
    printf("PASS: unknown host rejects widescreen before fixed-address reads or MinHook init\n");
    return 0;
}

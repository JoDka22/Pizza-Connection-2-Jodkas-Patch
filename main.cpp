#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include <string.h>
#include <shellapi.h>
#include <stdio.h>
#include <cstdint>
#include <MinHook.h>

#include "types.h"

using Wndproc_t = LRESULT(__fastcall*)(uint32_t a1, uint32_t a2, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
Wndproc_t WndprocOrig = nullptr;

using GameWndProc_t = LRESULT(__thiscall*)(uint32_t a1, HWND hWnd, uint32_t Msg, WPARAM wParam, LPARAM lParam);
GameWndProc_t GameWndProcOrig = (GameWndProc_t)0x5252E0;

using CreateCallbackTimer_t = uint32_t(__thiscall*)(uint32_t* This, void* func, uint32_t maxMs, uint32_t a4);
CreateCallbackTimer_t CreateCallbackTimerOrig = 0;

using ProcessGameTick_t = uint32_t(__thiscall*)(MainGame* game, uint32_t bFpsCounter);
ProcessGameTick_t ProcessGameTickOrig = 0;

using CreateWorldClock_t = uint32_t(__cdecl*)(WorldClock* clock);
CreateWorldClock_t CreateWorldClockOrig = 0;

using EnterCriticalSectionFunc_t = uint32_t(__thiscall*)(void* This);
EnterCriticalSectionFunc_t EnterCriticalSectionFunc = (EnterCriticalSectionFunc_t)0x401320;
EnterCriticalSectionFunc_t LeaveCriticalSectionFunc = (EnterCriticalSectionFunc_t)0x401340;

double* gameSpeed = (double*)0x55C4C8;

bool bEnableConsole = 0;
uint32_t* bIsFocus = (uint32_t*)0x584504;
uint32_t* bFullscreen = (uint32_t*)0x563860;

uint32_t* hObject = (uint32_t*)0x57C0D8;

uint32_t currentSpeed = 1;
uint8_t* maxFrameMs = (uint8_t*)0x481713;
uint32_t* gameState = (uint32_t*)0x56386C;
uint32_t* maxEntityTickMs = nullptr;
uint32_t* currentEntityTickMs = nullptr;

uint8_t* waitForMultipleObjects = (uint8_t*)0x404E48;

uint32_t* Window_Height = (uint32_t*)0x404824;
uint32_t* Window_Width = (uint32_t*)0x404829;

RECT* ClientRect = (RECT*)0x573D38;
WorldClock* CurrentClock = nullptr;

void ChangeGameSpeed(uint32_t speed)
{
    static uint32_t previousSpeed = 1;

    if (!maxEntityTickMs)
        return;

    *gameSpeed = 1000 / speed;
    *maxEntityTickMs = 200 / speed;
    CurrentClock->msToTickClock = 3125 / speed;

    // The current coroutine timer's accumulator, special care has to be taken so we dont constantly reset it.
    // Or else the game will never actually execute the function.
    if (previousSpeed != speed)
    {
        *currentEntityTickMs = 200 / speed;
        previousSpeed = speed;
    }
}

LRESULT __stdcall Wndproc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LONG WindowLongA; // eax

    if ( Msg == WM_DESTROY )
    {
        SetEvent(hObject);
        PostQuitMessage(0);
    }
    else if ( Msg == WM_ACTIVATEAPP )
    {
        *bIsFocus = true;
    }

    if (*gameState != IN_GAME)
        maxEntityTickMs = nullptr;

    if (Msg == WM_KEYUP && maxEntityTickMs != nullptr)
    {
        switch (wParam)
        {
        case '1':
            currentSpeed = 1;
            break;
        case '2':
            currentSpeed = 2;
            break;
        case '3':
            currentSpeed = 4;
            break;
        case '4':
            currentSpeed = 8;
            break;
        default:
            break;
        }
    }

    WindowLongA = GetWindowLongA(hWnd, GWL_USERDATA);
    if ( WindowLongA )
        return GameWndProcOrig(WindowLongA, hWnd, Msg, wParam, lParam);
    else
        return DefWindowProcA(hWnd, Msg, wParam, lParam);
}

int __fastcall CreateCallbackTimer(uint32_t* This, void* EDX, void* func, int maxMs, int a4)
{
  if ((uint32_t)func == 0x481190)
  {
      int result = CreateCallbackTimerOrig(This, func, maxMs, a4);
      
      CallbackTimer* timer = *(CallbackTimer**)((char*)This + 0xC);
      maxEntityTickMs = &timer->maxMs;
      currentEntityTickMs = &timer->curMs;
      *currentEntityTickMs = 200 / currentSpeed;

      return result;
  }
  return CreateCallbackTimerOrig(This, func, maxMs, a4);
}

void __cdecl CreateWorldClock(WorldClock* clock)
{
    CurrentClock = clock;
    CreateWorldClockOrig(clock);
}

int __fastcall ProcessGameTick(MainGame* game, void* EDX, uint32_t bFpsCounter)
{
    ChangeGameSpeed(currentSpeed);
    return ProcessGameTickOrig(game, bFpsCounter);
}

void Main()
{
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc == 2 && lstrcmpW(argv[1], L"-windowed") == 0)
        *bFullscreen = false;

    if (bEnableConsole)
    {
        AllocConsole();

        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	    freopen_s((FILE**)stdout, "CONOUT$", "w", stderr);
	    freopen_s((FILE**)stdout, "CONOUT$", "r", stdin);
    }

    auto mhStatus = MH_Initialize();

    MH_EnableHook(&CreateWindowExA);

    if (MH_CreateHook((void*)0x525270, &Wndproc, reinterpret_cast<LPVOID*>(&WndprocOrig)) != MH_OK) throw;
	if (MH_EnableHook((void*)0x525270) != MH_OK) throw;

    if (MH_CreateHook((void*)0x403AC0, &CreateCallbackTimer, reinterpret_cast<LPVOID*>(&CreateCallbackTimerOrig)) != MH_OK) throw;
	if (MH_EnableHook((void*)0x403AC0) != MH_OK) throw;

    if (MH_CreateHook((void*)0x4DD2D0, &CreateWorldClock, reinterpret_cast<LPVOID*>(&CreateWorldClockOrig)) != MH_OK) throw;
	if (MH_EnableHook((void*)0x4DD2D0) != MH_OK) throw;

    if (MH_CreateHook((void*)0x525A10, &ProcessGameTick, reinterpret_cast<LPVOID*>(&ProcessGameTickOrig)) != MH_OK) throw;
    if (MH_EnableHook((void*)0x525A10) != MH_OK) throw;

    uint32_t processStartAddr = reinterpret_cast<uint32_t>(GetModuleHandle(nullptr));

    DWORD oldProtect = NULL;
    VirtualProtect((void*)(0x401000), sizeof(uint8_t) * 0x15b000, PAGE_EXECUTE_READWRITE, &oldProtect);

    // stop intro
    memset((void*)0x404196, 0x90, sizeof(uint8_t) * 5);
    memset((void*)0x4041B1, 0x90, sizeof(uint8_t) * 5);

    // center window
    memset((void*)0x404420, 0xC3, sizeof(uint8_t) * 1);

    // ret on srand
    // memset((void*)0x54F0D5, 0xC3, sizeof(char) * 1);
    
    uint32_t screenW = GetSystemMetrics(SM_CXSCREEN);
    uint32_t screenH = GetSystemMetrics(SM_CYSCREEN);

    uint32_t centerX = screenW / 2;
    uint32_t centerY = screenH / 2;
    
    ClientRect->left = centerY;
    ClientRect->top = centerX - *(uint32_t*)Window_Width;

    *maxFrameMs = 10;

    *Window_Width = *(uint32_t*)Window_Width + 4;
    *Window_Height = *(uint32_t*)Window_Height + 29;

    *waitForMultipleObjects = 1;

    VirtualProtect((void*)(0x401000), sizeof(uint8_t) * 0x15b000, oldProtect, &oldProtect);
    VirtualProtect((void*)gameSpeed, sizeof(uint8_t) * 8, PAGE_EXECUTE_READWRITE, &oldProtect);
}

void Quit()
{
    auto mhStatus = MH_DisableHook(MH_ALL_HOOKS);
}
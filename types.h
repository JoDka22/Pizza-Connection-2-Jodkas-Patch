#pragma once
#include <cstdint>
#include <ddraw.h>

struct WorldClock
{
	void* Func;
	uint16_t Year;
	uint8_t TotalHours;
	uint8_t byte7;
	uint8_t byte8;
	uint16_t byte9;
	uint32_t dwordC;
	uint32_t Hour;
	int32_t msToTickClock;
	uint8_t tmp2_1;
	uint8_t tmp2_2;
	uint8_t tmp2_3;
	uint8_t tmp2_4;
	uint32_t bIsRunning;
	uint32_t bQuitGame;
	uint32_t bStopTicking;
	char tmp[4];
};

struct CallbackTimer
{
	void* func;
	uint32_t dword4;
	uint32_t maxMs;
	uint32_t curMs;
	uint32_t dword10;
	uint32_t dword14;
};

struct ddraw_window_child
{
	LPDIRECTDRAW pp;
	LPDIRECTDRAWSURFACE7 ddrawSurface;
	LPDIRECTDRAWSURFACE7 backBuffer;
	LPDIRECTDRAWSURFACE7 backBuffer2;
	LPDIRECTDRAWSURFACE7 ddrawSurface2;
};

struct ddraw_window
{
	void* vtable;
	char tmp2[60];
	ddraw_window_child* ddraw;
	HWND hwnd;
	uint32_t _tmp1;
	uint32_t bFullscreen;
	char tmp3[36];
	uint32_t hasMMX;
	char tmp5[12];
	uint32_t xLeft;
	uint32_t xTop;
	char tmp6[8];
	uint32_t screenMode;
	char tmp4[4];
};

struct MainGame
{
	char tmp8[12];
	ddraw_window* window;
	char tmpbb[420];
	uint32_t bFullscreen;
	char tmp0[140];
	uint32_t* tmp13;
	char tmp4[12392];
	uint32_t tmp9;
	uint32_t ignoreInput2;
	uint32_t ignoreInput;
	char tmp5[20512];
	uint32_t tmp10;
	uint32_t tmp11;
	char tmp6[12];
	uint32_t tmp12;
	char tmp2[44];
	uint32_t hwnd;
	char tmp3[140];
	uint32_t bEnableHighFreq;
	char tmp[52];
};


#define IN_GAME 29
#define IN_FREEPLAY 23
#define GAME_SPEED 1000
#define MAX_ENTITY_TICK_MS 200
#define CURRENT_ENTITY_TICK_MS 200
#define TICK_CLOCK_MS 3125
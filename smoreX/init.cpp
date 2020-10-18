#include <Windows.h>
#include "functions.h"
#include "offsets.h"
#include "helper.h"
#include "config.h"
#include "init.h"
#include "hook.h"
#include "menu.h"


menu menu_smoreX;
init::init() {
	offsets::base = (uintptr_t)GetModuleHandle(L"ac_client.exe");
	offsets::localPlayer = (Entity*)*(uintptr_t*)(offsets::base + 0x10f4f4);
	offsets::GetCrosshairEnt = (tGetCrosshairEnt)(offsets::base + offsets::crosshair);
	offsets::showCursorMod = GetModuleHandle(L"SDL.dll");
	offsets::showCursorAddr = (uintptr_t)GetProcAddress(offsets::showCursorMod, "SDL_ShowCursor");

	menu_smoreX.create();
	menu_smoreX.init();
}

void init::main() {
	Meds();
	Ammo();
	AntiRecoil();
	Damage();
	Sprint();
	Fly();
	PaperWalls();
	Esp();
	Triggerbot();
	Aimbot();
	RapidFire();
	SuperJump();
	updateThemes();
}
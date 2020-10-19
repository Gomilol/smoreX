#include <Windows.h>
#include "helper.h"
#include "config.h"
#include "init.h"
#include "hook.h"
#include "menu.h"
#include "offsets.h"

typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);
twglSwapBuffers wglSwapBuffersGateway;

menu smoreX; init Init;
BOOL __stdcall hkwglSwapBuffers(HDC hDc) {
    offsets::entList = *(EntList**)offsets::entityListAddr;

    if (GetAsyncKeyState(VK_INSERT) & 1) {
        cfg::showMenu = !cfg::showMenu;
    }

    smoreX.render();
    Init.main();
    return wglSwapBuffersGateway(hDc);
}

DWORD __stdcall Thread(LPVOID param) {
    hook SwapBuffersHook("wglSwapBuffers", "opengl32.dll", (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateway, 5);
    SwapBuffersHook.enable();

    while (true) {
        if (cfg::closeMenu) {
            break;
        }
    }

    SwapBuffersHook.disable();

    Sleep(300); 
    smoreX.stop();

    Sleep(600); 
    FreeLibraryAndExitThread((HMODULE)param, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule,DWORD ul_reason_for_call, LPVOID lpReserved) {
    
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        CreateThread(NULL, 0, Thread, hModule, 0, NULL);
        DisableThreadLibraryCalls(hModule);
    }

    return TRUE;
}


// special thanks to https://github.com/Kix48/AssaultCube-Internal was helpful for helpers, drawing, offsets & menu theme
// thanks to guidedhacking for helping me understand c++ more clearly

// smoreX https://www.unknowncheats.me/forum/members/576479.html
// Kix48-Theme https://guidedhacking.com/members/kix.87156/
// Orange https://github.com/CookiePLMonster
// Dark https://github.com/Derydoca
// Grey https://github.com/malamanteau
#include "DllMain.h"

CheatEngine* ce = nullptr;


void CheatEngineInit(HINSTANCE hinstance)
{
    ce = new CheatEngine(hinstance);
    ce->Rendering();
    return delete ce;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CheatEngineInit, (LPVOID)hModule, NULL, NULL);
        break;
    }
    case DLL_THREAD_ATTACH: break;
    case DLL_PROCESS_DETACH:break;
    case DLL_THREAD_DETACH: break;
    }
    return TRUE;
}



#include "pch.h"
#include "adamreborn.h"
#include "rebornconfig.h"

DWORD WINAPI MainThread(LPVOID) {
    adamreborn::rebornconfig::LoadRebornConfig();
    adamreborn::ar::InitAdamReborn();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            MessageBoxA(NULL, "Hello, World!", "DLL Message", MB_OK | MB_ICONINFORMATION);
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

// Exported function to show your name
__declspec(dllexport) void ShowMyName() {
    MessageBoxA(NULL, "My name is Yasmin !", "Name Export", MB_OK | MB_ICONINFORMATION);
}

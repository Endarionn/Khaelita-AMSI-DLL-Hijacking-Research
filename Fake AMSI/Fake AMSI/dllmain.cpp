// dllmain.cpp : DLL uygulamasının giriş noktasını tanımlar.
#include "pch.h"
#include <windows.h> // Gerekli Windows tipleri için

// Bu fonksiyonu zaten diğer tarafta tanımladığını varsayıyorum. 
// Eğer aynı dosya içinde değilse 'extern void WriteFakeAmsiLog();' diyebilirsin.
void WriteFakeAmsiLog();

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // DLL hafızaya yüklendiği AN burası çalışır.
        WriteFakeAmsiLog();
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
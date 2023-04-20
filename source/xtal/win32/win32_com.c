#define COM_CO_CREATE_INSTANCE(name) HRESULT WINAPI name(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv)
typedef COM_CO_CREATE_INSTANCE(COM_CoCreateInstance_);
COM_CO_CREATE_INSTANCE(COM_CoCreateInstanceStub) {
    Unreferenced(rclsid);
    Unreferenced(pUnkOuter);
    Unreferenced(dwClsContext);
    Unreferenced(riid);
    Unreferenced(ppv);
    return 1;
}

global COM_CoCreateInstance_* COM_CoCreateInstance = COM_CoCreateInstanceStub;

#define COM_CO_TASK_MEM_FREE(name) void WINAPI name(LPVOID pv)
typedef COM_CO_TASK_MEM_FREE(COM_CoTaskMemFree_);
COM_CO_TASK_MEM_FREE(COM_CoTaskMemFreeStub) {
    Unreferenced(pv);
}
global COM_CoTaskMemFree_* COM_CoTaskMemFree = COM_CoTaskMemFreeStub;

#define COM_CO_INITIALIZE_EX(name) HRESULT WINAPI name(LPVOID pvReserved, DWORD dwCoInit)
typedef COM_CO_INITIALIZE_EX(COM_CoInitializeEx_);
COM_CO_INITIALIZE_EX(COM_CoInitializeExStub) {
    Unreferenced(pvReserved);
    Unreferenced(dwCoInit);
    return 1;
}
global COM_CoInitializeEx_* COM_CoInitializeEx = COM_CoInitializeExStub;

internal void InitCOM(void) {
    HMODULE lib = LoadLibraryW(L"ole32.dll");
    if (lib) {
        COM_CoCreateInstance = (COM_CoCreateInstance_*) GetProcAddress(lib, "CoCreateInstance");
        COM_CoInitializeEx   = (COM_CoInitializeEx_*) GetProcAddress(lib, "CoInitializeEx");
        COM_CoTaskMemFree    = (COM_CoTaskMemFree_*) GetProcAddress(lib, "CoTaskMemFree");
    }
    COM_CoInitializeEx(0, COINIT_MULTITHREADED);
}

// TODO(geni): Properly watch DLL using the directory change notification API

#if XTAL_HOT_RELOAD
global f64 last_reload_check_time;
#endif

typedef struct {
    HMODULE dll;
#if XTAL_HOT_RELOAD
    FILETIME last_dll_write_time;
#endif

    Xtal_ApplicationPermanentLoadCallback* PermanentLoad;
    Xtal_ApplicationHotLoadCallback*       HotLoad;
    Xtal_ApplicationHotUnloadCallback*     HotUnload;
    Xtal_ApplicationUpdateCallback*        Update;
    Xtal_ApplicationTickCallback*          Tick;
    Xtal_ApplicationDrawCallback*          Draw;
} AppCode;

internal b32 AppCodeLoad(AppCode* app_code) {
    b32 result = 1;

#if XTAL_HOT_RELOAD
    app_code->last_dll_write_time = GetLastWriteTime(global_app_dll_path);

    // NOTE(geni): CopyFile may fail while cl.exe is writing to the file, as it doesn't share reading
    //             To word around this, we try to copy until it works
    while (!CopyFileS16(global_app_dll_path, global_temp_app_dll_path)) {
        Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
        LogError("Error copying DLL: %S", FormatErrorCode(scratch.arena, GetLastError()));
        Xtal_MArenaTempEnd(scratch);
        Sleep(100);
    }

    app_code->dll = LoadLibraryW(global_temp_app_dll_path.s);
#else
    app_code->dll = LoadLibraryW(global_app_dll_path.s);
#endif

    if (!app_code->dll) {
        result = 0;
        goto end;
    }

    app_code->PermanentLoad = (Xtal_ApplicationPermanentLoadCallback*) GetProcAddress(app_code->dll, "Xtal_PermanentLoad");
    app_code->HotLoad       = (Xtal_ApplicationHotLoadCallback*) GetProcAddress(app_code->dll, "Xtal_HotLoad");
    app_code->HotUnload     = (Xtal_ApplicationHotUnloadCallback*) GetProcAddress(app_code->dll, "Xtal_HotUnload");
    app_code->Update        = (Xtal_ApplicationUpdateCallback*) GetProcAddress(app_code->dll, "Xtal_Update");
    app_code->Tick          = (Xtal_ApplicationTickCallback*) GetProcAddress(app_code->dll, "Xtal_Tick");
    app_code->Draw          = (Xtal_ApplicationDrawCallback*) GetProcAddress(app_code->dll, "Xtal_Draw");

    if (!app_code->PermanentLoad || !app_code->HotLoad || !app_code->HotUnload || !app_code->Update) {
        // NOTE(geni): App code is missing entry points
        app_code->PermanentLoad = Xtal_ApplicationPermanentLoadStub;
        app_code->HotLoad       = Xtal_ApplicationHotLoadStub;
        app_code->HotUnload     = Xtal_ApplicationHotUnloadStub;
        app_code->Update        = Xtal_ApplicationUpdateStub;
        app_code->Draw          = Xtal_ApplicationDrawStub;

        result = 0;
        goto end;
    }

#if XTAL_HOT_RELOAD
    last_reload_check_time = 0;
#endif

end:
    return result;
}

internal void AppCodeUnload(AppCode* app_code) {
    if (app_code->dll) {
        FreeLibrary(app_code->dll);
        // NOTE(geni): Pray to god the file gets freed by this point
        Sleep(300);
    }
    app_code->dll           = 0;
    app_code->PermanentLoad = Xtal_ApplicationPermanentLoadStub;
    app_code->HotLoad       = Xtal_ApplicationHotLoadStub;
    app_code->HotUnload     = Xtal_ApplicationHotUnloadStub;
    app_code->Update        = Xtal_ApplicationUpdateStub;
    app_code->Draw          = Xtal_ApplicationDrawStub;
}

#if XTAL_HOT_RELOAD
internal void AppCodeUpdate(AppCode* app_code) {
    // TODO(geni): Fix double hot reloading after cl makes changes. Possibly caused by file being written to, then deleted, then recreated?
    //             2023 update: Nevermind, it fixed itself at some point?! Maybe a VS update fixed it..
    if (last_reload_check_time == 0) {
        last_reload_check_time = GetTime(&global_win32_timer);
    }

    f64 time = GetTime(&global_win32_timer);
    if (time - last_reload_check_time >= 1.0) {
        FILETIME last_write_time = GetLastWriteTime(global_app_dll_path);
        if (CompareFileTime(&last_write_time, &app_code->last_dll_write_time)) {
            Log("Hot reloading");
            app_code->HotUnload();
            AppCodeUnload(app_code);
            AppCodeLoad(app_code);
            app_code->HotLoad(&global_os, 1);
        }
        last_reload_check_time = time;
    }
}
#endif

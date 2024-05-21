//~ Message output

internal String16 _FormatErrorCode(DWORD error) {
    thread_local local_persist WCHAR buffer[4096];

    DWORD size = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, buffer, ArrayCount(buffer), NULL);
    return (String16){buffer, size - 2};
}
internal String8 FormatErrorCode(Xtal_MArena* arena, DWORD error) {
    return S8_PushFromS16(arena, _FormatErrorCode(error));
}
internal String16 FormatErrorCodeS16(Xtal_MArena* arena, DWORD error) {
    return S16_PushCopy(arena, _FormatErrorCode(error));
}

internal void OutputGenericV(const char* title, const char* format, UINT type, va_list args) {
    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
    String8         text    = S8_PushFV(scratch.arena, format, args);
    MessageBoxA(0, text.cstr, title, type);
    Xtal_MArenaTempEnd(scratch);
}
internal void OutputGenericS16V(const u16* title, const u16* format, UINT type, va_list args) {
    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
    String16        text    = S16_PushFV(scratch.arena, format, args);
    MessageBoxW(0, text.s, title, type);
    Xtal_MArenaTempEnd(scratch);
}
internal void OutputErrorMessage(const char* title, const char* format, ...) {
    va_list args;
    va_start(args, format);
    OutputGenericV(title, format, MB_OK | MB_ICONERROR, args);
    va_end(args);
}
internal void OutputErrorMessageS16(const u16* title, const u16* format, ...) {
    va_list args;
    va_start(args, format);
    OutputGenericS16V(title, format, MB_OK | MB_ICONERROR, args);
    va_end(args);
}

internal void WriteToConsole(String8 data) {
    DWORD bytes_written;
    WriteFile(global_conout_handle, data.cstr, (DWORD) data.size, &bytes_written, NULL);
    HardAssert(bytes_written == data.size);
    OutputDebugStringA(data.cstr);
}

//~ Other

// TODO(geni): Should probably move
internal void ToggleFullscreen(HWND hwnd) {
    local_persist WINDOWPLACEMENT last_window_placement = {
        .length = sizeof last_window_placement,
    };

    LONG window_style = GetWindowLong(hwnd, GWL_STYLE);
    if (window_style & WS_OVERLAPPEDWINDOW) {
        MONITORINFO monitor_info = {
            .cbSize = sizeof monitor_info,
        };
        if (GetWindowPlacement(hwnd, &last_window_placement) &&
            GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY),
                           &monitor_info)) {
            SetWindowLong(hwnd, GWL_STYLE,
                          window_style & ~(WS_OVERLAPPEDWINDOW | WS_POPUP));

            SetWindowPos(hwnd, HWND_TOP,
                         monitor_info.rcMonitor.left,
                         monitor_info.rcMonitor.top,
                         monitor_info.rcMonitor.right -
                             monitor_info.rcMonitor.left,
                         monitor_info.rcMonitor.bottom -
                             monitor_info.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } else {
        SetWindowLong(hwnd, GWL_STYLE,
                      window_style | WS_OVERLAPPEDWINDOW | WS_POPUP);
        SetWindowPlacement(hwnd, &last_window_placement);
        SetWindowPos(hwnd, 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

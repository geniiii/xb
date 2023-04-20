global b32 global_raw_mouse_registered;

internal void UpdateClipRect(void) {
    RECT rect;
    GetWindowRect(global_window_handle, &rect);
    u32 cx = (rect.left + rect.right) / 2;
    u32 cy = (rect.top + rect.bottom) / 2;
    rect   = (RECT){
          .left   = cx - 1,
          .right  = cx + 1,
          .top    = cy - 1,
          .bottom = cy + 1};
    if (!ClipCursor(&rect)) {
        LogError("Failed to clip cursor");
    }
}

internal void RawMouseDeviceChangeState(b32 enable) {
    String8 verb = enable ? S8Lit("register") : S8Lit("unregister");

    if (enable == global_raw_mouse_registered) {
        LogWarning("Tried to pointlessly %S raw mouse input device", verb);
        return;
    }

    RAWINPUTDEVICE id = {
        .usUsagePage = HID_USAGE_PAGE_GENERIC,
        .usUsage     = HID_USAGE_GENERIC_MOUSE,
        .dwFlags     = enable ? RIDEV_NOLEGACY : RIDEV_REMOVE,
    };
    if (RegisterRawInputDevices(&id, 1, sizeof id) == FALSE) {
        Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
        OutputErrorMessage("Raw Input Error", "Could not %S raw mouse input device; error: %S", verb, FormatErrorCodeS16(scratch.arena, GetLastError()));
        Xtal_MArenaTempEnd(scratch);
        return;
    }

    global_raw_mouse_registered = enable;
}

internal void ToggleCursor(b32 show) {
    if (global_os.raw_mouse_input && global_raw_mouse_registered == show) {
        RawMouseDeviceChangeState(!show);
    }

    global_os.cursor_locked = !show;
    // NOTE(geni): I hate Win32
    //             ShowCursor returns an internal cursor state counter... which can be over 1 or under 0. Passing 1 increments it, 0 decrements it. Why?!
    //             We need to do these stupid while loops to get around the neverending stream of Microsoft insanity :((
    if (show) {
        while (ShowCursor(show) <= 0) {}
        ClipCursor(NULL);
    } else {
        while (ShowCursor(show) >= 0) {}
        UpdateClipRect();
    }
}

internal void ToggleRawMouse(b32 enable) {
    global_os.raw_mouse_input = enable;
    RawMouseDeviceChangeState(enable);
}

internal void HandleRawInputMessage(LPARAM l_param) {
    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);

    u32 size;
    GetRawInputData((HRAWINPUT) l_param, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
    u8* buf = Xtal_MArenaPush(scratch.arena, size);
    if (GetRawInputData((HRAWINPUT) l_param, RID_INPUT, buf, &size, sizeof(RAWINPUTHEADER)) != size) {
        LogWarning("Failed to get raw input data; error: %S", FormatErrorCodeS16(scratch.arena, GetLastError()));
        goto end;
    }
    RAWINPUT* input = (RAWINPUT*) buf;

    switch (input->header.dwType) {
        case RIM_TYPEMOUSE: {
            RAWMOUSE mouse = input->data.mouse;
            if (mouse.lLastX != 0 || mouse.lLastY != 0) {
                Xtal_OSPushEvent(Xtal_OSMouseMoveEvent((ivec2s){mouse.lLastX, mouse.lLastY}));
            }

            switch (mouse.usButtonFlags) {
                case RI_MOUSE_WHEEL: {
                    Xtal_OSPushEvent(Xtal_OSMouseScrollEvent((ivec2s){0, mouse.usButtonData / WHEEL_DELTA}));
                } break;
                case RI_MOUSE_HWHEEL: {
                    Xtal_OSPushEvent(Xtal_OSMouseScrollEvent((ivec2s){mouse.usButtonData / WHEEL_DELTA, 0}));
                } break;

                case RI_MOUSE_LEFT_BUTTON_DOWN: {
                    Xtal_OSPushEvent(Xtal_OSMousePressEvent(MouseButton_Left));
                } break;
                case RI_MOUSE_LEFT_BUTTON_UP: {
                    Xtal_OSPushEvent(Xtal_OSMouseReleaseEvent(MouseButton_Left));
                } break;
                case RI_MOUSE_MIDDLE_BUTTON_DOWN: {
                    Xtal_OSPushEvent(Xtal_OSMousePressEvent(MouseButton_Middle));
                } break;
                case RI_MOUSE_MIDDLE_BUTTON_UP: {
                    Xtal_OSPushEvent(Xtal_OSMouseReleaseEvent(MouseButton_Middle));
                } break;
                case RI_MOUSE_RIGHT_BUTTON_DOWN: {
                    Xtal_OSPushEvent(Xtal_OSMousePressEvent(MouseButton_Right));
                } break;
                case RI_MOUSE_RIGHT_BUTTON_UP: {
                    Xtal_OSPushEvent(Xtal_OSMouseReleaseEvent(MouseButton_Right));
                } break;
            }
        } break;

        default: {
            LogWarning("Received event from unimplemented raw input device of type %d", input->header.dwType);
        } break;
    }

end:
    Xtal_MArenaTempEnd(scratch);
}

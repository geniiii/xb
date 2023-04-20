//~ Events

internal inline Xtal_OSEvent Xtal_OSKeyPressEvent(Key key, KeyModifiers modifiers) {
    return (Xtal_OSEvent){
        .type      = Xtal_OSEventType_KeyPress,
        .key       = key,
        .modifiers = modifiers,
    };
}

internal inline Xtal_OSEvent Xtal_OSKeyReleaseEvent(Key key, KeyModifiers modifiers) {
    return (Xtal_OSEvent){
        .type      = Xtal_OSEventType_KeyRelease,
        .key       = key,
        .modifiers = modifiers,
    };
}

internal inline Xtal_OSEvent Xtal_OSMouseMoveEvent(ivec2s delta) {
    return (Xtal_OSEvent){
        .type  = Xtal_OSEventType_MouseMove,
        .delta = delta,
    };
}

internal inline Xtal_OSEvent Xtal_OSMousePressEvent(MouseButton button) {
    return (Xtal_OSEvent){
        .type         = Xtal_OSEventType_MousePress,
        .mouse_button = button,
    };
}

internal inline Xtal_OSEvent Xtal_OSMouseReleaseEvent(MouseButton mouse_button) {
    return (Xtal_OSEvent){
        .type         = Xtal_OSEventType_MouseRelease,
        .mouse_button = mouse_button,
    };
}

internal inline Xtal_OSEvent Xtal_OSMouseLeaveEvent(void) {
    return (Xtal_OSEvent){
        .type = Xtal_OSEventType_MouseLeave,
    };
}

internal inline Xtal_OSEvent Xtal_OSMouseScrollEvent(ivec2s delta) {
    return (Xtal_OSEvent){
        .type  = Xtal_OSEventType_MouseScroll,
        .delta = delta,
    };
}

internal inline Xtal_OSEvent Xtal_OSFileDroppedEvent(String8 path) {
    return (Xtal_OSEvent){
        .type = Xtal_OSEventType_FileDropped,
        .path = path,
    };
}

internal inline Xtal_OSEvent Xtal_OSWindowResizedEvent(void) {
    return (Xtal_OSEvent){
        .type = Xtal_OSEventType_WindowResized,
    };
}

internal inline Xtal_OSEvent Xtal_OSWindowFocusLostEvent(void) {
    return (Xtal_OSEvent){
        .type = Xtal_OSEventType_WindowFocusLost,
    };
}

internal inline Xtal_OSEvent Xtal_OSWindowFocusGainedEvent(void) {
    return (Xtal_OSEvent){
        .type = Xtal_OSEventType_WindowFocusGained,
    };
}

internal inline void Xtal_OSPushEvent(Xtal_OSEvent event) {
    HardAssert(xtal_os != NULL);
    if (xtal_os->event_count < ArrayCount(xtal_os->events)) {
        xtal_os->events[xtal_os->event_count++] = event;
    } else {
        LogWarning("No space in event buffer");
    }
}

internal Xtal_MArena* Xtal_GetScratchArena(Xtal_MArena* conflicts, u64 count) {
    local_persist thread_local Xtal_MArena scratch_pool[2];

    // NOTE(geni): Initialize scratch pool if necessary
    if (scratch_pool[0].base == NULL) {
        Xtal_MArena* arena_ptr = scratch_pool;
        for (u32 i = 0; i < ArrayCount(scratch_pool); ++i, ++arena_ptr) {
            *arena_ptr = Xtal_MArenaNew();
        }
    }
    Xtal_MArena* result = NULL;
    for (u32 i = 0; i < ArrayCount(scratch_pool); ++i) {
        Xtal_MArena* arena = &scratch_pool[i];
        result             = arena;
        for (u32 j = 0; j < count; ++j) {
            if (arena == &conflicts[j]) {
                result = NULL;
                break;
            }
        }
        if (result != NULL) {
            break;
        }
    }

    return result;
}
internal Xtal_MArenaTemp Xtal_GetScratch(Xtal_MArena* conflicts, u64 count) {
    return Xtal_MArenaTempBegin(Xtal_GetScratchArena(conflicts, count));
}

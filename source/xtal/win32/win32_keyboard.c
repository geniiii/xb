internal void HandleKeyPressMessage(WPARAM w_param, LPARAM l_param) {
    KeyModifiers modifiers = 0;
    if (GetKeyState(VK_CONTROL) & 0x8000) {
        modifiers |= KeyModifier_Ctrl;
    }
    if (GetKeyState(VK_SHIFT) & 0x8000) {
        modifiers |= KeyModifier_Shift;
    }
    if (GetKeyState(VK_MENU) & 0x8000) {
        modifiers |= KeyModifier_Alt;
    }

    u32 vkey_code = (u32) w_param;
    u32 key_input = 0;

    if (vkey_code >= 'A' && vkey_code <= 'Z') {
        key_input = Key_A + (vkey_code - 'A');
    } else if (CharIsNumeric(vkey_code)) {
        key_input = Key_0 + (vkey_code - '0');
    } else if (vkey_code >= VK_F1 && vkey_code <= VK_F12) {
        key_input = Key_F1 + vkey_code - VK_F1;
    } else {
        switch (vkey_code) {
            case VK_ESCAPE: {
                key_input = Key_Esc;
            } break;

            case VK_OEM_3: {
                key_input = Key_GraveAccent;
            } break;

            case VK_OEM_MINUS: {
                key_input = Key_Minus;
            } break;
            case VK_OEM_PLUS: {
                key_input = Key_Equal;
            } break;

            case VK_BACK: {
                key_input = Key_Backspace;
            } break;
            case VK_TAB: {
                key_input = Key_Tab;
            } break;
            case VK_SPACE: {
                key_input = Key_Space;
            } break;
            case VK_RETURN: {
                key_input = Key_Enter;
            } break;

            case VK_CONTROL: {
                key_input = Key_Ctrl;
                modifiers &= ~KeyModifier_Ctrl;
            } break;
            case VK_SHIFT: {
                key_input = Key_Shift;
                modifiers &= ~KeyModifier_Shift;
            } break;
            case VK_MENU: {
                key_input = Key_Alt;
                modifiers &= ~KeyModifier_Alt;
            } break;

            case VK_LEFT:
            case VK_UP:
            case VK_RIGHT:
            case VK_DOWN: {
                key_input = Key_Left + vkey_code - VK_LEFT;
            } break;

            case VK_DELETE: {
                key_input = Key_Delete;
            } break;

            case VK_PRIOR:
            case VK_NEXT: {
                key_input = Key_PageUp + vkey_code - VK_PRIOR;
            } break;

            case VK_HOME: {
                key_input = Key_Home;
            } break;
            case VK_END: {
                key_input = Key_End;
            } break;

            case VK_OEM_2: {
                key_input = Key_ForwardSlash;
            } break;

            case VK_OEM_PERIOD: {
                key_input = Key_Period;
            } break;
            case VK_OEM_COMMA: {
                key_input = Key_Comma;
            } break;
            case VK_OEM_7: {
                key_input = Key_Quote;
            } break;

            case VK_OEM_4: {
                key_input = Key_LeftBracket;
            } break;
            case VK_OEM_6: {
                key_input = Key_RightBracket;
            } break;
        }
    }

    b32 is_down = !(l_param & (1 << 31));
    Xtal_OSPushEvent(is_down ? Xtal_OSKeyPressEvent(key_input, modifiers) : Xtal_OSKeyReleaseEvent(key_input, modifiers));
}

internal void HandleCharInputMessage(WPARAM w_param) {
    // TODO(geni): This isn't very battle-tested, as I only tested Cyrillic while writing all the Unicode handling stuff

    local_persist u16 low_surrogate;

    u16 c = (u16) w_param;
    u32 codepoint;

    if (Xtal_Unicode_IsLowSurrogate(c)) {
        if (low_surrogate != 0) {
            LogWarning("Received two low surrogates in WM_CHAR, ignoring first");
        }
        low_surrogate = c;
        return;
    } else if (Xtal_Unicode_IsHighSurrogate(c)) {
        if (low_surrogate == 0) {
            LogWarning("Received high surrogate without low surrogate in WM_CHAR, ignoring");
            return;
        }
        codepoint     = ((c & 0x3FF) << 10) | (low_surrogate & 0x3FF);
        low_surrogate = 0;
    } else {
        codepoint = c;
    }

    Xtal_OSEvent event = {
        .type      = Xtal_OSEventType_CharacterInput,
        .character = {.size = Xtal_Unicode_UTF8SizeFromCodepoint(codepoint)},
    };
    Xtal_Unicode_UTF8FromCodepoint(codepoint, &event.character.data[0], sizeof event.character.data);
    Xtal_OSPushEvent(event);
}

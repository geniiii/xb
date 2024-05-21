//~ String creation functions

internal String16 S16_FromWString(u16* wstr) {
    return (String16){
        .s    = wstr,
        .size = wcslen(wstr),
    };
}

internal String16 S16_Push(Xtal_MArena* arena, u64 size) {
    String16 result = {
        .s    = Xtal_MArenaPushTypeN(arena, u16, size + 1),
        .size = size,
    };
    result.s[size] = L'\0';
    return result;
}

internal String16 S16_PushCopy(Xtal_MArena* arena, String16 str) {
    String16 result = {
        .s    = Xtal_MArenaPushTypeN(arena, u16, str.size + 1),
        .size = str.size,
    };
    Xtal_MemoryCopy(result.s, str.s, str.size * sizeof(u16));
    result.s[result.size] = '\0';
    return result;
}

// NOTE(geni): Always null terminates
internal String16 S16_PushFromS8(Xtal_MArena* arena, String8 str) {
    String16 result;

    u8* str_end = str.s;

    u64 needed_code_units = 0;
    u8  size;
    while (str_end != str.s + str.size) {
        size = Xtal_Unicode_UTF8Size(*str_end);
        if (size == 0) {
            // TODO(geni): This won't handle non-UTF-8 correctly
            LogWarning("Got unexpected continuation byte");
            ++str_end;
            continue;
        }

        needed_code_units += size <= 3 ? 1 : 2;
        str_end += size;
    }

    result = (String16){
        .s    = Xtal_MArenaPushTypeN(arena, u16, needed_code_units + 1),
        .size = needed_code_units,
    };
    result.s[needed_code_units] = L'\0';

    u16* write_ptr = result.s;

    Xtal_Unicode_Codepoint codepoint;
    do {
        codepoint = Xtal_Unicode_CodepointFromUTF8(str.s, str_end - str.s);
        // TODO(geni): This writes even if there's no space left
        write_ptr += Xtal_Unicode_UTF16FromCodepoint(codepoint.codepoint, write_ptr);
        str.s += codepoint.size;
    } while (str.s <= str_end && codepoint.size != 0);

    return result;
}

internal String16 S16_PushFV(Xtal_MArena* arena, const u16* fmt, va_list args) {
    // TODO(geni): Modify stbsp_sprintf to allow for wide chars
    // TODO(geni): _vsnwprintf is MSVC-specific

    va_list args2;
    va_copy(args2, args);
    u64      needed_bytes = _vsnwprintf(0, 0, fmt, args) + 1;
    String16 result;
    result.s              = Xtal_MArenaPushTypeN(arena, u16, needed_bytes);
    result.size           = _vsnwprintf(result.s, needed_bytes, fmt, args2);
    result.s[result.size] = L'\0';
    return result;
}

internal String16 S16_PushF(Xtal_MArena* arena, const u16* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    String16 result = S16_PushFV(arena, fmt, args);
    va_end(args);
    return result;
}

//~ Copy functions

internal void S16_CopyToFixedSizeBuffer(String16 str, void* buf) {
    Xtal_MemoryCopy(buf, str.s, str.size * sizeof(u16));
}

//~ Find occurrence functions

internal String16 S16_SubstrOnLastOccurrenceOf(String16 str, u16 c) {
    for (u16* i = str.s + str.size; i >= str.s; --i) {
        if (*i == c) {
            return (String16){
                .s    = i,
                .size = i - str.s + 1,
            };
        }
    }

    return str;
}

internal String16 S16_SubstrAfterLastOccurrenceOf(String16 str, u16 c) {
    String16 on_last = S16_SubstrOnLastOccurrenceOf(str, c);
    if (on_last.size - 1 == 0 || on_last.s == str.s) {
        return str;
    }
    return (String16){
        .s    = on_last.s + 1,
        .size = on_last.size - 1,
    };
}

internal String16 S16_SubstrUntilLastOccurrenceOf(String16 str, u16 c) {
    for (u16* i = str.s + str.size; i >= str.s; --i) {
        if (*i == c) {
            return (String16){
                .s    = str.s,
                .size = i - str.s + 1,
            };
        }
    }

    return str;
}

internal inline String16 S16_SubstrOnIndex(String16 str, u64 index) {
    return (String16){
        .s    = str.s + index,
        .size = str.size - index,
    };
}

internal inline String16 S16_SubstrAfterIndex(String16 str, u64 index) {
    return (String16){
        .s    = str.s + index + 1,
        .size = str.size - index - 1,
    };
}

//~ String creation functions

internal inline String8 S8_FromCString(char* str) {
    return (String8){
        .cstr = str,
        .size = CalculateCStringLength(str),
    };
}

internal inline String8 S8_Push(Xtal_MArena* arena, u64 size) {
    String8 result = {
        .s    = Xtal_MArenaPush(arena, size + 1),
        .size = size,
    };
    result.s[size] = '\0';
    return result;
}

internal inline String8 S8_PushCopy(Xtal_MArena* arena, String8 str) {
    String8 result = {
        .s    = Xtal_MArenaPush(arena, str.size + 1),
        .size = str.size,
    };
    Xtal_MemoryCopy(result.s, str.s, str.size);
    result.s[str.size] = '\0';
    return result;
}

internal String8 S8_PushFromS16(Xtal_MArena* arena, String16 str) {
    u16* str_end = str.s + str.size;

    // NOTE(geni): Calculate needed bytes
    u64 needed_bytes = 0;
    for (u16* i = str.s; i < str_end;) {
        Xtal_Unicode_Codepoint codepoint = Xtal_Unicode_CodepointFromUTF16(i, str_end - i);
        needed_bytes += Xtal_Unicode_UTF8SizeFromCodepoint(codepoint.codepoint);
        i += codepoint.size;
    }

    String8 result = {
        .s    = Xtal_MArenaPush(arena, needed_bytes + 1),
        .size = needed_bytes,
    };
    result.s[needed_bytes] = '\0';
    u8* write_ptr          = result.s;

    Xtal_Unicode_Codepoint codepoint;
    do {
        codepoint = Xtal_Unicode_CodepointFromUTF16(str.s, str_end - str.s);
        // TODO(geni): This may not work correctly
        write_ptr += Xtal_Unicode_UTF8FromCodepoint(codepoint.codepoint, write_ptr, result.size);
        str.s += codepoint.size;
    } while (str.s < str_end && codepoint.size != 0);

    return result;
}

internal String8 S8_PushFV(Xtal_MArena* arena, const char* format, va_list args) {
    va_list args2;
    va_copy(args2, args);
    i32     needed_bytes = stbsp_vsnprintf(0, 0, format, args) + 1;
    String8 result;
    {
        result.s              = Xtal_MArenaPush(arena, needed_bytes);
        result.size           = stbsp_vsnprintf(result.cstr, needed_bytes, format, args2);
        result.s[result.size] = '\0';
    }
    return result;
}

internal String8 S8_PushF(Xtal_MArena* arena, const char* format, ...) {
    va_list args;
    va_start(args, format);
    String8 result = S8_PushFV(arena, format, args);
    va_end(args);
    return result;
}

//~ String matching functions

internal inline b32 S8_IsEmpty(String8 str) {
    return str.s == NULL || str.size == 0 || str.s[0] == '\0';
}

internal b32 S8_MatchGeneric(String8 a, String8 b, StringMatchFlags flags) {
    b32 result = 0;

    if (a.size == b.size) {
        result = 1;
        for (u64 i = 0; i < a.size; ++i) {
            b32 match = 1;

            if (a.s[i] != b.s[i]) {
                match = 0;
            } else if (flags & StringMatchFlag_CaseInsensitive) {
                if (CharToLower(a.cstr[i]) != CharToLower(b.cstr[i])) {
                    match = 0;
                }
            }

            if (!match) {
                result = 0;
                break;
            }
        }
    }

    return result;
}

internal b32 S8_Match(String8 a, String8 b) {
    return S8_MatchGeneric(a, b, 0);
}

internal b32 S8_MatchCaseInsensitive(String8 a, String8 b) {
    return S8_MatchGeneric(a, b, StringMatchFlag_CaseInsensitive);
}

//~ Copy functions

internal inline void S8_CopyToFixedSizeBuffer(String8 str, void* buf) {
    Xtal_MemoryCopy(buf, str.s, str.size);
}

//~ Conversion functions

internal inline void S8_ConvertToLowercase(String8 str) {
    for (u64 i = 0; i < str.size; ++i) {
        str.s[i] = CharToLower(str.cstr[i]);
    }
}

internal inline void S8_ConvertToUppercase(String8 str) {
    for (u64 i = 0; i < str.size; ++i) {
        str.s[i] = CharToUpper(str.cstr[i]);
    }
}

internal inline void S8_ConvertToLowercaseWithUnderscores(String8 str) {
    for (u64 i = 0; i < str.size; ++i) {
        if (str.s[i] == ' ') {
            str.s[i] = '_';
        } else {
            str.s[i] = CharToLower(str.cstr[i]);
        }
    }
}

internal String8 S8_PushFormattedBytes(Xtal_MArena* arena, u64 bytes) {
    // TODO(geni): The 'g' format may be a bad choice
    if (bytes >= Gigabytes(1)) {
        return S8_PushF(arena, "%.03gGB", (f64) bytes / Gigabytes(1));
    } else if (bytes >= Megabytes(1)) {
        return S8_PushF(arena, "%.03gMB", (f64) bytes / Megabytes(1));
    } else if (bytes >= Kilobytes(1)) {
        return S8_PushF(arena, "%.03gKB", (f64) bytes / Kilobytes(1));
    }
    return S8_PushF(arena, "%lluB", bytes);
}

//~ Find occurrence functions

internal String8 S8_SubstrOnLastOccurenceOf(String8 str, char c) {
    for (u8* i = str.s + str.size - 1; i >= str.s; --i) {
        if (*i == c) {
            return (String8){
                .s    = i,
                .size = str.size - (u64) (i - str.s),
            };
        }
    }

    return str;
}

internal String8 S8_SubstrAfterLastOccurrenceOf(String8 str, char c) {
    String8 on_last = S8_SubstrOnLastOccurenceOf(str, c);
    // TODO(geni): Not sure if this behaves correctly when it can't find an occurrence
    if (on_last.size - 1 == 0 || on_last.s == str.s) {
        return str;
    }
    return (String8){
        .s    = on_last.s + 1,
        .size = on_last.size - 1,
    };
}

internal String8 S8_SubstrOnFirstOccurrenceOf(String8 str, char c) {
    for (u8* i = str.s; i < str.s + str.size - 1; ++i) {
        if (*i == c) {
            return (String8){
                .s    = i,
                .size = i - str.s,
            };
        }
    }

    return str;
}

internal String8 S8_SubstrAfterFirstOccurrenceOf(String8 str, char c) {
    String8 on_first = S8_SubstrOnFirstOccurrenceOf(str, c);
    if (on_first.size - 1 == 0 || on_first.s == str.s) {
        return str;
    }
    return (String8){
        .s    = on_first.s + 1,
        .size = on_first.size - 1};
}

internal inline String8 S8_SubstrOnIndex(String8 str, u64 index) {
    return (String8){
        .s    = str.s + index,
        .size = str.size - index,
    };
}

internal inline String8 S8_SubstrAfterIndex(String8 str, u64 index) {
    return (String8){
        .s    = str.s + index + 1,
        .size = str.size - index - 1,
    };
}

// TODO(geni): Make our own CString/String to I32/U32/F32/whatever functions
internal i64 S8_GetFirstI64(String8 string) {
    b32 found_first_digit     = 0;
    u64 integer_str_write_pos = 0;
    u8  integer_str[64]       = {0};

    for (u64 read_pos = 0; read_pos < string.size; ++read_pos) {
        if (found_first_digit) {
            // NOTE(geni): Can't write any more characters to buffer
            if (integer_str_write_pos == sizeof(integer_str)) {
                integer_str[sizeof(integer_str) - 1] = '\0';
                break;
            }
            if (CharIsDigit(string.s[read_pos]) || string.s[read_pos] == '-') {
                integer_str[integer_str_write_pos++] = string.s[read_pos];
            } else {
                // NOTE(geni): Integer is over
                integer_str[integer_str_write_pos] = '\0';
                break;
            }
        } else {
            if (CharIsDigit(string.s[read_pos]) || string.s[read_pos] == '-') {
                integer_str[integer_str_write_pos++] = string.s[read_pos];
                found_first_digit                    = 1;
            }
        }
    }

    return CStringToI64((const char*) integer_str);
}

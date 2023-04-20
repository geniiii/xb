//~ String matching functions

#define CalculateCStringLength(s)  ((u32) strlen(s))
#define CalculateWCStringLength(s) ((u32) wcslen(s))

// NOTE(geni): Returns 1 if both strings are NULL
internal b32 CStringMatchCaseInsensitiveN(const char* a, const char* b, u64 n) {
    if (a && b) {
        for (u64 i = 0; i < n; ++i) {
            if (CharToLower(a[i]) != CharToLower(b[i])) {
                return 0;
            } else if (a[i] == '\0' && b[i] == '\0') {
                return 1;
            }
        }
    } else if (a || b) {
        return 0;
    }

    return 1;
}

//~ Copy functions

internal void CopyCStringToFixedSizeBuffer(char* destination, u64 destination_max, char* source) {
    for (u64 i = 0; i < destination_max && source[i] != '\0'; ++i) {
        destination[i] = source[i];
    }
    destination[destination_max - 1] = '\0';
}

internal void CopyCStringToFixedSizeBufferN(char* destination, u64 destination_max, char* source, u64 source_max) {
    for (u64 i = 0; i < destination_max && i < source_max && source[i] != '\0'; ++i) {
        destination[i] = source[i];
    }
    destination[destination_max - 1] = '\0';
}

//~ Conversion functions

internal inline void ConvertCStringToLowercase(char* str) {
    for (u64 i = 0; str[i]; ++i) {
        str[i] = CharToLower(str[i]);
    }
}

internal inline void ConvertCStringToUppercase(char* str) {
    for (u64 i = 0; str[i]; ++i) {
        str[i] = CharToUpper(str[i]);
    }
}

internal inline void ConvertCStringToLowercaseWithUnderscores(char* str) {
    for (u64 i = 0; str[i]; ++i) {
        if (str[i] == ' ') {
            str[i] = '_';
        } else {
            str[i] = CharToLower(str[i]);
        }
    }
}

//~ Append functions

internal void AppendCStringToFixedSizeCString(char* destination, u64 destination_max, char* str) {
    u64 append_index = CalculateCStringLength(destination);
    if (destination_max > append_index) {
        CopyCStringToFixedSizeBuffer(destination + append_index, destination_max - append_index, str);
    } else {
        LogWarning("Could not append \"%s\" to \"%.*s\"", str, destination, append_index);
    }
}

//~ Conversion functions

#define CStringToI32(s) ((i32) atoi(s))
#define CStringToU32(s) ((u32) atoi(s))
#define CStringToF32(s) ((f32) atof(s))
#define CStringToU64(s) ((u64) strtoull(s, NULL, 10))
#define CStringToI64(s) ((u64) strtoll(s, NULL, 10))

//~ Find occurrence functions

internal f32 GetFirstF32FromCString(const char* str) {
    b32 found_first_digit   = 0;
    u64 float_str_write_pos = 0;
    u8  float_str[64]       = {0};

    for (u64 read_pos = 0; str[read_pos]; ++read_pos) {
        if (found_first_digit) {
            if (float_str_write_pos == sizeof(float_str)) {
                // NOTE(geni): Can't write any more characters to buffer
                float_str[sizeof(float_str) - 1] = '\0';
                break;
            }

            if (CharIsDigit(str[read_pos]) || str[read_pos] == '.' || str[read_pos] == '-') {
                float_str[float_str_write_pos++] = str[read_pos];
            } else {
                // NOTE(geni): Float is over
                float_str[float_str_write_pos] = '\0';
                break;
            }
        } else {
            if (CharIsDigit(str[read_pos]) || str[read_pos] == '.' || str[read_pos] == '-') {
                float_str[float_str_write_pos++] = str[read_pos];
                found_first_digit                = 1;
            }
        }
    }

    return CStringToF32((const char*) float_str);
}

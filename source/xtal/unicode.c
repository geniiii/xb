// TODO(geni): The size variable is not very well named, as it returns size in *code units*
//             The struct's name is also probably not very good as it's not just a codepoint, a codepoint is a u32, perhaps calling it "DecodedCodepoint" is better?
typedef struct {
    u32 codepoint;
    u8  size;
} Xtal_Unicode_Codepoint;

internal force_inline u8 Xtal_Unicode_UTF8Size(u8 c) {
    // clang-format off
    local_persist const u8 sizes[] = {
    	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    	0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0
    };
    // clang-format on

    return sizes[c >> 3];
}

internal force_inline u8 Xtal_Unicode_UTF8SizeFromCodepoint(u32 codepoint) {
    if (codepoint >= 0x80 && codepoint <= 0x7FF) {
        return 2;
    } else if (codepoint >= 0x800 && codepoint <= 0xFFFF) {
        return 3;
    } else if (codepoint >= 0x10000 && codepoint <= 0x10FFF) {
        return 4;
    }

    return 1;
}

//~ Decoding functions

#define Xtal_Unicode_IsLowSurrogate(c)  ((c) >= 0xD800 && (c) <= 0xDBFF)
#define Xtal_Unicode_IsHighSurrogate(c) ((c) >= 0xDC00 && (c) <= 0xDFFF)

internal Xtal_Unicode_Codepoint Xtal_Unicode_CodepointFromUTF16(const u16* c, u64 max) {
    Xtal_Unicode_Codepoint result = {
        .codepoint = c[0],
        .size      = 1,
    };

    // TODO(geni): Handle unpaired surrogates (apparently Windows allows them in filenames?!)
    // NOTE(geni): Check if surrogate pairs
    if (max > result.size && Xtal_Unicode_IsLowSurrogate(c[0]) && Xtal_Unicode_IsHighSurrogate(c[1])) {
        // NOTE(geni): 0x3FF = 0b0000001111111111
        u16 high_surrogate = (c[0] & 0x3FF) << 10;
        u16 low_surrogate  = c[1] & 0x3FF;
        result.codepoint   = high_surrogate | low_surrogate;
        result.size        = 2;
    }

    return result;
}

// NOTE(geni): Length trick taken from the branchless UTF-8 decoder (https://github.com/skeeto/branchless-utf8/)
// TODO(geni): This is not perfect - it doesn't check for any invalid codepoints, overlong encodings, unexpected continuation bytes, etc..
internal Xtal_Unicode_Codepoint Xtal_Unicode_CodepointFromUTF8(const u8* c, u64 max) {
    // NOTE(geni): UTF-8 is variable-length; the number of leading set bits in the first code unit (byte) is the number of code units the codepoint takes up
    //             If there's only one leading set bit, then the code unit (byte) is a continuation to the leading one

    Xtal_Unicode_Codepoint result = {0};

    u8 length = Xtal_Unicode_UTF8Size(c[0]);
    if (max < length) {
        return result;
    }

    switch (length) {
        case 1: {
            result.codepoint = c[0];
            result.size      = 1;
        } break;
        case 2: {
            // NOTE(geni): Ensure second byte is continuation byte
            if (Xtal_Unicode_UTF8Size(c[1]) == 0) {
                // NOTE(geni): 0x1F = 0b00011111, a leading byte with length 2 looks like 0b110xxxxx
                result.codepoint = (c[0] & 0x1F) << 6;
                // NOTE(geni): 0x3F = 0b00111111, a continuation byte looks like 0b10xxxxxx
                result.codepoint |= c[1] & 0x3F;
                result.size = 2;
                break;
            }
            LogError("Rest of bytes aren't continuation bytes?!");
        } break;
        case 3: {
            // NOTE(geni): Ensure second and third byte are continuation bytes
            if (Xtal_Unicode_UTF8Size(c[1]) == 0 && Xtal_Unicode_UTF8Size(c[2]) == 0) {
                // NOTE(geni): 0xF = 0b00001111, a leading byte with length 3 looks like 0b1110xxxx
                result.codepoint = (c[0] & 0xF) << 12;
                // NOTE(geni): 0x3F = 0b00111111, a continuation byte looks like 0b10xxxxxx
                result.codepoint |= (c[1] & 0x3F) << 6;
                result.codepoint |= c[2] & 0x3F;
                result.size = 3;
                break;
            }
            LogError("Rest of bytes aren't continuation bytes?!");
        } break;
        case 4: {
            // NOTE(geni): Ensure second, third and fourth byte are continuation bytes
            if (Xtal_Unicode_UTF8Size(c[1]) == 0 && Xtal_Unicode_UTF8Size(c[2]) == 0 && Xtal_Unicode_UTF8Size(c[3]) == 0) {
                // NOTE(geni): 0x07 = 0b00000111, a leading byte with length 4 looks like 0b11110xxx
                result.codepoint = (c[0] & 0x07) << 18;
                // NOTE(geni): 0x3F = 0b00111111, a continuation byte looks like 0b10xxxxxx
                result.codepoint |= (c[1] & 0x3F) << 12;
                result.codepoint |= (c[2] & 0x3F) << 6;
                result.codepoint |= c[3] & 0x3F;
                result.size = 4;
                break;
            }
            LogError("Rest of bytes aren't continuation bytes?!");
        } break;
        default: {
            LogError("Received continuation byte?!");
        } break;
    }

    return result;
}

//~ Encoding functions

internal u8 Xtal_Unicode_UTF16FromCodepoint(u32 codepoint, u16* out) {
    if (codepoint < 0x10000) {
        *out = (u16) codepoint;
        return 1;
    } else if (codepoint <= 0x10FFF) {
        codepoint -= 0x10000;
        u16 high_surrogate = (u16) (codepoint >> 10) | 0xD800;
        u16 low_surrogate  = (u16) (codepoint & 0x3FF) | 0xDC00;
        out[0]             = high_surrogate;
        out[1]             = low_surrogate;
        return 2;
    }

    *out = L'?';
    LogWarning("Invalid codepoint %x", codepoint);
    return 1;
}

internal u8 Xtal_Unicode_UTF8FromCodepoint(u32 codepoint, u8* out, u64 out_size) {
    if (!out) {
        LogError("Attempted to write UTF-8 glyph to nil buffer");
        return 0;
    }

    u8 size = 0;
    if (codepoint <= 0x7F) {
        size = 1;
    } else if (codepoint <= 0x7FF) {
        size = 2;
    } else if (codepoint <= 0xFFFF) {
        size = 3;
    } else if (codepoint <= 0x10FFF) {
        size = 4;
    }

    if (out_size < size) {
        LogError("Attempted to write UTF-8 glyph of size %u to buffer of size %u", size, out_size);
        *out = '?';
        return 0;
    }

    switch (size) {
        case 1: {
            *out = (u8) codepoint;
        } break;
        case 2: {
            // NOTE(geni): 0xC0 = 0b11000000
            out[0] = 0xC0 | (u8) (codepoint >> 6);
            // NOTE(geni): 0x80 = 0b10000000
            //             0x3F = 0b00111111
            out[1] = 0x80 | (codepoint & 0x3F);
        } break;
        case 3: {  // NOTE(geni): 0xE0 = 0b11100000
            out[0] = 0xE0 | (u8) (codepoint >> 12);
            out[1] = 0x80 | ((codepoint >> 6) & 0x3F);
            out[2] = 0x80 | (codepoint & 0x3F);
        } break;
        case 4: {
            // NOTE(geni): 0xF0 = 0b11110000
            out[0] = 0xF0 | (u8) (codepoint >> 18);
            out[1] = 0x80 | ((codepoint >> 12) & 0x3F);
            out[2] = 0x80 | ((codepoint >> 6) & 0x3F);
            out[3] = 0x80 | (codepoint & 0x3F);
        } break;
        default: {
            *out = '?';
            LogWarning("Invalid codepoint %x", codepoint);
        } break;
    }
    return size;
}

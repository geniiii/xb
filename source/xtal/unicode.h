// TODO(geni): The size variable is not very well named, as it returns size in *code units*
//             The struct's name is also probably not very good as it's not just a codepoint, a codepoint is a u32, perhaps calling it "DecodedCodepoint" is better?
typedef struct {
    u32 codepoint;
    u8  size;
} Xtal_Unicode_Codepoint;

internal force_inline u8 Xtal_Unicode_UTF8Size(u8 c);
internal force_inline u8 Xtal_Unicode_UTF8SizeFromCodepoint(u32 codepoint);

//~ Decoding functions

#define Xtal_Unicode_IsLowSurrogate(c)  ((c) >= 0xD800 && (c) <= 0xDBFF)
#define Xtal_Unicode_IsHighSurrogate(c) ((c) >= 0xDC00 && (c) <= 0xDFFF)

internal Xtal_Unicode_Codepoint Xtal_Unicode_CodepointFromUTF16(const u16* c, u64 max);
internal Xtal_Unicode_Codepoint Xtal_Unicode_CodepointFromUTF8(const u8* c, u64 max);

//~ Encoding functions

internal u8 Xtal_Unicode_UTF16FromCodepoint(u32 codepoint, u16* out);
internal u8 Xtal_Unicode_UTF8FromCodepoint(u32 codepoint, u8* out, u64 out_size);

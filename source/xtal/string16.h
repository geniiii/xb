//~ String16 definition and convenience macros

typedef struct {
    union {
        u16*   s;
        u16*   str;
        wchar* wstr;
        void*  data;
    };
    u64 size;
} String16;

#define S16Lit(s) (String16) S16LitComp(s)
#define S16LitComp(s) \
    { (u16*) (s), ArrayCount(s) - 1 }
#define S16VArg(s) (i32)((s).size), ((s).str)

internal String16        S16_FromWString(u16* wstr);
internal String16        S16_Push(Xtal_MArena* arena, u64 size);
internal String16        S16_PushCopy(Xtal_MArena* arena, String16 str);
internal String16        S16_PushFromS8(Xtal_MArena* arena, String8 str);
internal String16        S16_PushFV(Xtal_MArena* arena, const u16* fmt, va_list args);
internal String16        S16_PushF(Xtal_MArena* arena, const u16* fmt, ...);
internal void            S16_CopyToFixedSizeBuffer(String16 str, void* buf);
internal String16        S16_SubstrOnLastOccurrenceOf(String16 str, u16 c);
internal String16        S16_SubstrAfterLastOccurrenceOf(String16 str, u16 c);
internal String16        S16_SubstrUntilLastOccurrenceOf(String16 str, u16 c);
internal inline String16 S16_SubstrOnIndex(String16 str, u64 index);
internal inline String16 S16_SubstrAfterIndex(String16 str, u64 index);

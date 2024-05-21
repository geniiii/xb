//~ String8 definition and convenience macros

enum {
    StringMatchFlag_CaseInsensitive = (1 << 0),
};
typedef u32 StringMatchFlags;

typedef struct {
    union {
        u8*   s;
        u8*   str;
        char* cstr;
        void* data;
    };
    u64 size;
} String8;

#define S8Lit(s) (String8) S8LitComp(s)
#define S8LitComp(s) \
    { (u8*) (s), ArrayCount(s) - 1 }
#define S8VArg(s)        (i32)((s).size), ((s).str)
#define S8IVArg(s)       ((s).str), (i32) ((s).size)
#define S8AsCStrIVArg(s) ((s).cstr), (i32) ((s).size)
#define S8LocalPersist(name, size)              \
    local_persist char    _##name##_data[size]; \
    local_persist String8 name = {              \
        .cstr = &_##name##_data[0],             \
    }

internal String8        S8_FromWString(u8* wstr);
internal String8        S8_Push(Xtal_MArena* arena, u64 size);
internal String8        S8_PushCopy(Xtal_MArena* arena, String8 str);
internal String8        S8_PushFromS8(Xtal_MArena* arena, String8 str);
internal String8        S8_PushFV(Xtal_MArena* arena, const char* fmt, va_list args);
internal String8        S8_PushF(Xtal_MArena* arena, const char* fmt, ...);
internal void           S8_CopyToFixedSizeBuffer(String8 str, void* buf);
internal String8        S8_SubstrOnLastOccurrenceOf(String8 str, char c);
internal String8        S8_SubstrAfterLastOccurrenceOf(String8 str, char c);
internal String8        S8_SubstrUntilLastOccurrenceOf(String8 str, char c);
internal inline String8 S8_SubstrOnIndex(String8 str, u64 index);
internal inline String8 S8_SubstrAfterIndex(String8 str, u64 index);
internal i64            S8_GetFirstI64(String8 str);

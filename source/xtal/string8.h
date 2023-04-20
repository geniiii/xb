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

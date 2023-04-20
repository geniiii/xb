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

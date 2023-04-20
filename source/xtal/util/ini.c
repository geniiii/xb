internal b32 Xtal_HashMapCompare_String8(String8* a, String8 b, u64 b_hash) {
    return a->size == b.size && XXH64(a->data, a->size, 0) == b_hash && memcmp(a->data, b.data, b.size) == 0;
}

Xtal_HashMapDefine(String8, String8, Xtal_HashMapCompare_String8)
Xtal_HashMapDefine(String8, Xtal_HashMap_String8_String8, Xtal_HashMapCompare_String8)

typedef Xtal_HashMap_String8_String8                      Xtal_INIVarsHashMap;
typedef Xtal_HashMap_String8_Xtal_HashMap_String8_String8 Xtal_INISectionsHashMap;

typedef struct {
    Xtal_INISectionsHashMap sections;
} Xtal_INI;

internal Xtal_INI Xtal_INIParse(String8 file) {
    Xtal_INI result;
    for (u32 i = 0; i < file.size; ++i) {
    }
}
internal Xtal_INIVarsHashMap* Xtal_INIGetSection(Xtal_INI* ini, String8 key);
internal String8*             Xtal_INIGetVar(Xtal_INIVarsHashMap* section, String8 key);

internal

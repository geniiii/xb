#define Xtal_HashMapDeclareNamed(key_type, value_type, type_name) \
    typedef struct {                                              \
        key_type   key;                                           \
        u64        hash;                                          \
        value_type value;                                         \
        u32        psl;                                           \
    } type_name##_Bucket;                                         \
    typedef struct {                                              \
        Xtal_MArena         arena;                                \
        Xtal_MArena         arena_grow;                           \
        type_name##_Bucket* buckets;                              \
        u64                 count;                                \
        u64                 capacity;                             \
    } type_name;                                                  \
    internal type_name type_name##_New(u64 initial_capacity);

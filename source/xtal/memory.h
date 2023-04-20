typedef struct {
    void* base;
    u64   max;
    u64   alloc_position;
    u64   commit_position;
} Xtal_MArena;

typedef struct {
    Xtal_MArena* arena;
    u64 pos;
} Xtal_MArenaTemp;

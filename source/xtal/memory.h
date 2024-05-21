typedef struct {
    void* base;
    u64   max;
    u64   alloc_position;
    u64   commit_position;
} Xtal_MArena;

typedef struct {
    Xtal_MArena* arena;
    u64          pos;
} Xtal_MArenaTemp;

internal Xtal_MArena Xtal_MArenaFromSize(u64 size);
internal Xtal_MArena Xtal_MArenaNew(void);
internal void*       Xtal_MArenaPush(Xtal_MArena* arena, u64 size);
internal void*       Xtal_MArenaPushZero(Xtal_MArena* arena, u64 size);
internal void        Xtal_MArenaPop(Xtal_MArena* arena, u64 size);
internal void        Xtal_MArenaPopAndShrink(Xtal_MArena* arena, u64 size);
internal void        Xtal_MArenaClear(Xtal_MArena* arena);
internal void        Xtal_MArenaClearAndShrink(Xtal_MArena* arena);
internal void        Xtal_MArenaRelease(Xtal_MArena* arena);

internal Xtal_MArenaTemp Xtal_MArenaTempBegin(Xtal_MArena* arena);
internal void            Xtal_MArenaTempEnd(Xtal_MArenaTemp temp);
internal void            Xtal_MArenaTempEndAndShrink(Xtal_MArenaTemp temp);

typedef struct {
    Xtal_MArena arena;
    u32         count;
    u32         max_count;
    u32         stride;
} Xtal_ArenaArrayHeader;

internal void  _Xtal_ArenaArrayGrow(void* array);
internal void* _Xtal_ArenaArrayNew(u32 stride, u32 n);
internal void  Xtal_ArenaArrayDestroy(void* array);

#define Xtal_ArenaArrayNewN(t, n)       ((t*) _Xtal_ArenaArrayNew(sizeof(t), n))
#define Xtal_ArenaArrayNew(t)           Xtal_ArenaArrayNewN(t, 1)
#define Xtal_ArenaArrayGetHeader(array) ((Xtal_ArenaArrayHeader*) (array) -1)
#define Xtal_ArenaArrayCount(array)     (Xtal_ArenaArrayGetHeader(array)->count)
#define Xtal_ArenaArrayPush(array, x) \
    _Xtal_ArenaArrayGrow(array);      \
    array[Xtal_ArenaArrayCount(array) - 1] = x

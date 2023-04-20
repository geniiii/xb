typedef struct {
    Xtal_MArena arena;
    u32         count;
    u32         max_count;
    u32         stride;
} Xtal_ArenaArrayHeader;

internal void* _Xtal_ArenaArrayNew(u32 stride, u32 n) {
    Xtal_MArena            arena  = Xtal_MArenaNew();
    Xtal_ArenaArrayHeader* result = Xtal_MArenaPush(&arena, sizeof(Xtal_ArenaArrayHeader) + stride * n);
    result->arena                 = arena;
    result->count                 = 0;
    result->max_count             = n;
    result->stride                = stride;
    return (void*) (result + 1);
}
internal void* _Xtal_ArenaArrayNewZero(u32 stride, u32 n) {
    void* result = _Xtal_ArenaArrayNew(stride, n);
    Xtal_MemorySet(result, 0, stride * n);
    return result;
}
#define Xtal_ArenaArrayNewN(t, n)       ((t*) _Xtal_ArenaArrayNew(sizeof(t), n))
#define Xtal_ArenaArrayNew(t)           Xtal_ArenaArrayNewN(t, 1)

#define Xtal_ArenaArrayGetHeader(array) ((Xtal_ArenaArrayHeader*) (array) -1)
#define Xtal_ArenaArrayCount(array)     (Xtal_ArenaArrayGetHeader(array)->count)

internal void _Xtal_ArenaArrayGrow(void* array) {
    Xtal_ArenaArrayHeader* header = Xtal_ArenaArrayGetHeader(array);
    if (++header->count > header->max_count) {
        u32 max_count = (u32) Ceil((f32) header->max_count * 1.5f);
        Xtal_MArenaPush(&header->arena, header->stride * (max_count - header->max_count));
        header->max_count = max_count;
    }
}
#define Xtal_ArenaArrayPush(array, x) \
    _Xtal_ArenaArrayGrow(array);      \
    array[Xtal_ArenaArrayCount(array) - 1] = x

internal void Xtal_ArenaArrayDestroy(void* array) {
    Xtal_ArenaArrayHeader* header = Xtal_ArenaArrayGetHeader(array);
    Xtal_MArenaRelease(&header->arena);
}

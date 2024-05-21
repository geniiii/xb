#define XTAL_INVALID_HANDLE_ID 0

typedef struct {
    u32* free_slots;
    u32* generations;
    u32  free_slots_top;
    u32  size;
} Xtal_Pool;

internal Xtal_Pool Xtal_PoolNew(Xtal_MArena* arena, u32 size);
internal u32       Xtal_PoolAllocSlot(Xtal_Pool* pool);
internal void      Xtal_PoolFreeSlot(Xtal_Pool* pool, u32 slot);
internal void      Xtal_PoolFreeAll(Xtal_Pool* pool);

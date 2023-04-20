internal Xtal_Pool Xtal_PoolNew(Xtal_MArena* arena, u32 size) {
    ++size;
    Xtal_Pool result = {
        .size        = size,
        .generations = Xtal_MArenaPushTypeZeroN(arena, *result.generations, size),
        .free_slots  = Xtal_MArenaPushTypeZeroN(arena, *result.free_slots, size),
    };

    // NOTE(geni): Invalid ID is 0
    for (u32 i = size - 1; i >= 1; --i) {
        result.free_slots[result.free_slots_top++] = i;
    }

    return result;
}

internal u32 Xtal_PoolAllocSlot(Xtal_Pool* pool) {
    if (pool->free_slots_top == 0) {
        return XTAL_INVALID_HANDLE_ID;
    }

    u32 slot = pool->free_slots[--pool->free_slots_top];
    if (slot != XTAL_INVALID_HANDLE_ID && slot < pool->size) {
        ++pool->generations[slot];
        return slot;
    }
    return XTAL_INVALID_HANDLE_ID;
}

internal void Xtal_PoolFreeSlot(Xtal_Pool* pool, u32 slot) {
#if XTAL_SLOW
    // NOTE(geni): Check if the slot's already been freed
    for (u32 i = 0; i < pool->free_slots_top; ++i) {
        HardAssert(pool->free_slots[i] != slot);
    }
#endif
    pool->free_slots[pool->free_slots_top++] = slot;
}

internal void Xtal_PoolFreeAll(Xtal_Pool* pool) {
    pool->free_slots_top = 0;

    // NOTE(geni): Invalid ID is 0
    for (u32 i = pool->size - 1; i >= 1; --i) {
        pool->free_slots[pool->free_slots_top++] = i;
        ++pool->generations[i];
    }
}

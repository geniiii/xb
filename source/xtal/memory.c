// TODO(geni): I have a suspicion that reusing arenas post-reload is use-after-free lmao
//             Since reloads should only ever be enabled on debug builds I *think* this is okay?

// TODO(geni): Static arenas (no reservation of virtual memory)

#define XTAL_MARENA_DEFAULT_MAX Megabytes(512)
#define XTAL_MARENA_COMMIT_SIZE Kilobytes(4)
#define XTAL_MARENA_ALIGN       8

//~ Convenience macros

#define Xtal_MArenaPushType(arena, t)         Xtal_MArenaPush(arena, sizeof(t))
#define Xtal_MArenaPushTypeN(arena, t, n)     Xtal_MArenaPush(arena, sizeof(t) * (n))
#define Xtal_MArenaPushTypeZero(arena, t)     Xtal_MArenaPushZero(arena, sizeof(t))
#define Xtal_MArenaPushTypeZeroN(arena, t, n) Xtal_MArenaPushZero(arena, sizeof(t) * (n))

#define Xtal_MArenaPushTypeTemp(t)            Xtal_MArenaPushType(&xtal_os->frame_arena, t)
#define Xtal_MArenaPushTypeTempN(t, n)        Xtal_MArenaPushTypeN(&xtal_os->frame_arena, t, n)
#define Xtal_MArenaPushTypeTempZero(t)        Xtal_MArenaPushTypeZero(&xtal_os->frame_arena, t)
#define Xtal_MArenaPushTypeTempZeroN(t, n)    Xtal_MArenaPushTypeZeroN(&xtal_os->frame_arena, t, n)

#define Xtal_MArenaPushStruct                 Xtal_MArenaPushType
#define Xtal_MArenaPushStructN                Xtal_MArenaPushTypeN
#define Xtal_MArenaPushStructZero             Xtal_MArenaPushTypeZero
#define Xtal_MArenaPushStructZeroN            Xtal_MArenaPushTypeZeroN

#define Xtal_MArenaPushStructTemp             Xtal_MArenaPushTypeTemp
#define Xtal_MArenaPushStructTempN            Xtal_MArenaPushTypeTempN
#define Xtal_MArenaPushStructZeroTemp         Xtal_MArenaPushTypeTempZero
#define Xtal_MArenaPushStructZeroTempN        Xtal_MArenaPushTypeTempZeroN

#define Xtal_MArenaPopType(arena, t)          Xtal_MArenaPop(arena, sizeof(t))
#define Xtal_MArenaPopTypeN(arena, t, n)      Xtal_MArenaPop(arena, sizeof(t) * (n))

#define Xtal_MArenaPopStruct                  Xtal_MArenaPopType
#define Xtal_MArenaPopStructN                 Xtal_MArenaPopTypeN

//~ Functions

internal Xtal_MArena Xtal_MArenaFromSize(u64 size) {
    HardAssert(size > 0);
    return (Xtal_MArena){
        .max  = size,
        .base = xtal_os->Reserve(size),
    };
}
internal Xtal_MArena Xtal_MArenaNew(void) {
    return Xtal_MArenaFromSize(XTAL_MARENA_DEFAULT_MAX);
}

internal void* Xtal_MArenaPush(Xtal_MArena* arena, u64 size) {
    HardAssert(arena != NULL);
    HardAssert(arena->alloc_position + size < arena->max);

    u8* memory = NULL;

    u64 pos             = arena->alloc_position;
    u64 pos_address     = (u64) arena->base + pos;
    u64 aligned_address = pos_address + XTAL_MARENA_ALIGN - 1;
    aligned_address -= aligned_address & (XTAL_MARENA_ALIGN - 1);

    u64 alignment_size = aligned_address - pos_address;
    if (pos + alignment_size + size <= arena->max) {
        memory                = (u8*) pos_address + alignment_size;
        u64 new_pos           = pos + alignment_size + size;
        arena->alloc_position = new_pos;

        if (new_pos > arena->commit_position) {
            u64 commit_grow = new_pos - arena->commit_position;
            commit_grow += XTAL_MARENA_COMMIT_SIZE - 1;
            commit_grow -= commit_grow & (XTAL_MARENA_COMMIT_SIZE - 1);
            xtal_os->Commit((u8*) arena->base + arena->commit_position, commit_grow);
            arena->commit_position += commit_grow;
        }
    }

    return memory;
}

internal void* Xtal_MArenaPushZero(Xtal_MArena* arena, u64 size) {
    HardAssert(arena != NULL);
    void* memory = Xtal_MArenaPush(arena, size);
    Xtal_MemorySet(memory, 0, size);
    return memory;
}

internal void Xtal_MArenaPop(Xtal_MArena* arena, u64 size) {
    HardAssert(arena != NULL);
    u64 pos = arena->alloc_position - size;
    if (arena->alloc_position > pos) {
        arena->alloc_position = pos;
    }
}

internal void Xtal_MArenaPopAndShrink(Xtal_MArena* arena, u64 size) {
    HardAssert(arena != NULL);
    u64 pos = arena->alloc_position - size;
    if (arena->alloc_position > pos) {
        arena->alloc_position = pos;

        u64 decommit_pos = pos;
        decommit_pos += XTAL_MARENA_COMMIT_SIZE - 1;
        decommit_pos -= decommit_pos % XTAL_MARENA_COMMIT_SIZE;
        u64 over_committed = arena->commit_position - decommit_pos;
        over_committed -= over_committed % XTAL_MARENA_COMMIT_SIZE;
        if (decommit_pos > 0) {
            xtal_os->Decommit((u8*) arena + decommit_pos, over_committed);
            arena->commit_position -= over_committed;
        }
    }
}

internal void Xtal_MArenaClear(Xtal_MArena* arena) {
    HardAssert(arena != NULL);
    Xtal_MArenaPop(arena, arena->alloc_position);
}

internal void Xtal_MArenaClearAndShrink(Xtal_MArena* arena) {
    HardAssert(arena != NULL);
    Xtal_MArenaPopAndShrink(arena, arena->alloc_position);
}

internal void Xtal_MArenaRelease(Xtal_MArena* arena) {
    HardAssert(arena != NULL);
    xtal_os->Release(arena->base);
}

internal Xtal_MArenaTemp Xtal_MArenaTempBegin(Xtal_MArena* arena) {
    return (Xtal_MArenaTemp){
        .arena = arena,
        .pos   = arena->alloc_position,
    };
}

internal void Xtal_MArenaTempEnd(Xtal_MArenaTemp temp) {
    Xtal_MArenaPop(temp.arena, temp.arena->alloc_position - temp.pos);
}

internal void Xtal_MArenaTempEndAndShrink(Xtal_MArenaTemp temp) {
    Xtal_MArenaPopAndShrink(temp.arena, temp.arena->alloc_position - temp.pos);
}

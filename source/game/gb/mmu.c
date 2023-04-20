// TODO(geni): Might want to rename this file; the Game Boy doesn't have an MMU.

internal inline b32 GB_DMAInProgress(Gameboy* gb) {
    return gb->mmu.dma.active;
}
internal inline void GB_DMARequest(Gameboy* gb, u8 value) {
    gb->mmu.dma.source            = value;
    gb->mmu.dma.requested         = value;
    gb->mmu.dma.requested_pending = 4;
}
internal void GB_HandleMMU(Gameboy* gb, u8 cycles) {
    u8 masked = gb->mmu.dma.addr & 0xFF;
    if (masked == 0xA0) {
        gb->mmu.dma.active = 0;
    } else if (GB_DMAInProgress(gb)) {
        gb->mmu.dma.cycles += cycles;
        if (gb->mmu.dma.cycles >= 4) {
            gb->mmu.dma.cycles -= 4;

            HardAssert(masked <= 0x9F);
            HardAssert(gb->mmu.dma.addr <= 0xFF9F);

            u16 addr = gb->mmu.dma.addr;
            // TODO(geni): This is most likely broken on MBC3 (memory.c:111)
            u8* get = addr >= 0xC000 && addr <= 0xFFFF ? &gb->wram[(addr - 0xC000) & 8191] : GB_GetU8(gb, addr);

            gb->oam[masked] = get ? *get : 0xFF;
            ++gb->mmu.dma.addr;
        }
    }

    if (gb->mmu.dma.requested_pending) {
        gb->mmu.dma.requested_pending -= cycles;

        if (gb->mmu.dma.requested_pending == 0) {
            gb->mmu.dma.active            = 1;
            gb->mmu.dma.addr              = gb->mmu.dma.requested << 8;
            gb->mmu.dma.requested_pending = 0;
        }
    }
}

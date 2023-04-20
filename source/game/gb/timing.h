internal force_inline b32 GB_IsTimerEnabled(Gameboy* gb) {
    return (gb->cpu.hw_regs.tac & 4) >> 2;
}
internal force_inline u8 GB_GetANDResult(Gameboy* gb) {
    const static u8 timarate_to_bit[] = {9, 3, 5, 7};

    return GetBit(gb->cpu.timer.raw_div,
                  timarate_to_bit[gb->cpu.hw_regs.tac & 3]) &
           GB_IsTimerEnabled(gb);
}

internal force_inline void GB_CheckForFallingEdge(Gameboy* gb) {
    u8 new_and = GB_GetANDResult(gb);
    if (gb->cpu.timer.last_and && !new_and) {
        if (gb->cpu.hw_regs.tima++ == 0xFF) {
            gb->cpu.timer.reset_tima = 1;
        }
    }
    gb->cpu.timer.last_and = new_and;
}

internal force_inline void GB_ClearDIV(Gameboy* gb) {
    gb->cpu.timer.raw_div = 0;
    gb->cpu.hw_regs.div   = 0;
    GB_CheckForFallingEdge(gb);
}

internal force_inline void GB_WriteTAC(Gameboy* gb, u8 value) {
    gb->cpu.hw_regs.tac = value;
    GB_CheckForFallingEdge(gb);
}

internal void GB_CycleT(Gameboy* gb, u8 cycles);
internal void GB_Cycle1M(Gameboy* gb);
internal void GB_Cycle2M(Gameboy* gb);

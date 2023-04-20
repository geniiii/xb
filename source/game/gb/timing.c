// NOTE(geni): See https://hacktix.github.io/GBEDG/timers for details

internal force_inline void GB_HandleTimer(Gameboy* gb, u8 cycles) {
    if (gb->cpu.timer.reset_tima) {
        gb->cpu.hw_regs.tima = gb->cpu.hw_regs.tma;
        GB_RequestInterrupt(gb, Interrupt_Timer);
        gb->cpu.timer.reset_tima = 0;
    }

    gb->cpu.cycles += cycles;

    gb->cpu.timer.raw_div += cycles;
    gb->cpu.hw_regs.div = gb->cpu.timer.raw_div >> 8;

    GB_CheckForFallingEdge(gb);
}

internal inline void GB_CycleT(Gameboy* gb, u8 cycles) {
    GB_HandleMMU(gb, cycles);
    GB_HandleTimer(gb, cycles);
    GB_HandlePPU(gb, cycles);

    switch (gb->mbc.type) {
        case MBCType_MBC3TimerBattery:
        case MBCType_MBC3TimerRAMBattery: {
            GB_MBC3Handle(gb, cycles);
        } break;
    }
}
internal void GB_Cycle1M(Gameboy* gb) {
    GB_CycleT(gb, 4);
}
internal void GB_Cycle2M(Gameboy* gb) {
    GB_Cycle1M(gb);
    GB_Cycle1M(gb);
}

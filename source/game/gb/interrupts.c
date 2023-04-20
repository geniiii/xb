internal inline b32 GB_GetFirstInterrupt(Gameboy* gb, u8* to) {
    u8 masked = gb->cpu.hw_regs.int_flag & gb->cpu.hw_regs.int_enable & 0x1F;
    if (!masked) {
        return 0;
    }

    *to = (u8) Xtal_BitScanForwardU32(masked);
    return 1;
}

internal force_inline void GB_RequestInterrupt(Gameboy* gb, Interrupt id) {
    SetBit(gb->cpu.hw_regs.int_flag, id);
}

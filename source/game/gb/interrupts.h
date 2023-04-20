enum {
    Interrupt_VBlank,
    Interrupt_STAT,
    Interrupt_Timer,
    Interrupt_Serial,
    Interrupt_Joypad,

    Interrupt_Count
};
typedef u8 Interrupt;

// NOTE(geni): Unity build moment
internal void GB_RequestInterrupt(Gameboy* gb, Interrupt id);
internal void GB_HandleInterrupts(Gameboy* gb);

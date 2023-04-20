//~ Miscellaneous

Opcode(ccf) {
    ClearFlag(Flag_N);
    ClearFlag(Flag_H);
    ToggleFlagConditional(Flag_C, !GetFlag(Flag_C));
}
Opcode(scf) {
    ClearFlag(Flag_N);
    ClearFlag(Flag_H);
    SetFlag(Flag_C);
}

Opcode(nop) {
    Unreferenced(gb);
}

Opcode(halt) {
    // TODO(geni): Test HALT bug more thoroughly
    if ((gb->cpu.hw_regs.int_flag & gb->cpu.hw_regs.int_enable & 0x1F) && !gb->cpu.hw_regs.ime) {
        gb->cpu.halt_bug = 1;
    } else {
        gb->cpu.halt = 1;
    }
}

Opcode(stop) {
    Unreferenced(gb);
    // TODO(geni): Unimplemented
    LogWarning("Unimplemented call to STOP");
}

//~ IME

Opcode(di) {
    gb->cpu.hw_regs.ime         = 0;
    gb->cpu.hw_regs.ime_toggled = 0;
}
Opcode(ei) {
    gb->cpu.hw_regs.ime_toggled = 1;
}

//~ Jumps

Opcode(jp_u16) {
    PC = NNNN;
    GB_Cycle1M(gb);
}
Opcode(jp_hl) {
    PC = HL;
}

#define JumpConditional(flag_name, flag) \
    Opcode(jp_##flag_name##_u16) {       \
        u16 loc = NNNN;                  \
        if (GetFlag(flag)) {             \
            PC = loc;                    \
            GB_Cycle1M(gb);              \
        }                                \
    }                                    \
    Opcode(jp_##n##flag_name##_u16) {    \
        u16 loc = NNNN;                  \
        if (!GetFlag(flag)) {            \
            PC = loc;                    \
            GB_Cycle1M(gb);              \
        }                                \
    }
JumpConditional(c, Flag_C)
JumpConditional(z, Flag_Z)
Opcode(jr_i8) {
    PC += (i8) NN;
    GB_Cycle1M(gb);
}

#define JumpConditionalRelative(flag_name, flag) \
    Opcode(jr_##flag_name##_i8) {                \
        i8 offset = (i8) NN;                     \
        if (GetFlag(flag)) {                     \
            PC += offset;                        \
            GB_Cycle1M(gb);                      \
        }                                        \
    }                                            \
    Opcode(jr_##n##flag_name##_i8) {             \
        i8 offset = (i8) NN;                     \
        if (!GetFlag(flag)) {                    \
            PC += offset;                        \
            GB_Cycle1M(gb);                      \
        }                                        \
    }
JumpConditionalRelative(c, Flag_C)
JumpConditionalRelative(z, Flag_Z)

//~ Calls

Opcode(call_u16) {
    SP -= 2;
    u16 loc = NNNN;
    GB_Cycle1M(gb);
    GB_WriteU16(gb, SP, PC);
    PC = loc;
}

#define CallConditional(flag_name, flag) \
    Opcode(call_##flag_name##_u16) {     \
        u16 loc = NNNN;                  \
        if (GetFlag(flag)) {             \
            SP -= 2;                     \
            GB_Cycle1M(gb);              \
            GB_WriteU16(gb, SP, PC);     \
            PC = loc;                    \
        }                                \
    }                                    \
    Opcode(call_##n##flag_name##_u16) {  \
        u16 loc = NNNN;                  \
        if (!GetFlag(flag)) {            \
            SP -= 2;                     \
            GB_Cycle1M(gb);              \
            GB_WriteU16(gb, SP, PC);     \
            PC = loc;                    \
        }                                \
    }
CallConditional(z, Flag_Z)
CallConditional(c, Flag_C)

//~ Returns

Opcode(ret) {
    PC = GB_ReadU16(gb, SP);
    GB_Cycle1M(gb);
    SP += 2;
}
Opcode(reti) {
    Interpret_ei(gb);
    Interpret_ret(gb);
}

#define RetConditional(flag_name, flag) \
    Opcode(ret_##flag_name) {           \
        GB_Cycle1M(gb);                 \
        if (GetFlag(flag)) {            \
            Interpret_ret(gb);          \
        }                               \
    }                                   \
    Opcode(ret_##n##flag_name) {        \
        GB_Cycle1M(gb);                 \
        if (!GetFlag(flag)) {           \
            Interpret_ret(gb);          \
        }                               \
    }
RetConditional(z, Flag_Z)
RetConditional(c, Flag_C)

#define RSTU8(addr)              \
    Opcode(rst_##addr##h) {      \
        SP -= 2;                 \
        GB_Cycle1M(gb);          \
        GB_WriteU16(gb, SP, PC); \
        PC = 0x##addr;           \
    }
RSTU8(00)
// NOTE(geni): As 08 is considered an octal (due to starting with 0), we can't use the macro lmao
Opcode(rst_08h) {
    SP -= 2;
    GB_Cycle1M(gb);
    GB_WriteU16(gb, SP, PC);
    PC = 0x08;
}
RSTU8(10)
RSTU8(18)
RSTU8(20)
RSTU8(28)
RSTU8(30)
RSTU8(38)

Opcode(unused) {
    Unreferenced(gb);
    BreakDebugger();
}

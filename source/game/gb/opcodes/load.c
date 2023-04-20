//~ ld_r8_deref_r16

#define LoadR8DerefR16(a, var_a, b, var_b) \
    Opcode(ld_##a##_deref_##b) {           \
        var_a = GB_ReadU8(gb, var_b);      \
    }
#define Register(name, var) LoadR8DerefR16(name, var, hl, HL)
AllR8
#undef Register
LoadR8DerefR16(a, A, bc, BC)
LoadR8DerefR16(a, A, de, DE)

//~ ld_a_deref_ff00_xxxx

LoadR8DerefR16(a, A, ff00_plus_c, (0xFF00 + C))
LoadR8DerefR16(a, A, ff00_plus_u8, (0xFF00 + NN))

//~ ld_deref_r8_u16

LoadR8DerefR16(a, A, u16, NNNN)

//~ ld_deref_r16_r8

#define LoadDerefR16R8(a, var_a, b, var_b) \
    Opcode(ld_deref_##a##_##b) {           \
        GB_WriteU8(gb, var_a, var_b);      \
    }
#define Register(name, var) LoadDerefR16R8(hl, HL, name, var)
AllR8
#undef Register
LoadDerefR16R8(hl, HL, u8, NN)
LoadDerefR16R8(bc, BC, a, A)
LoadDerefR16R8(de, DE, a, A)
LoadDerefR16R8(u16, NNNN, a, A)

//~ ld_deref_ff00_xxxx_a

LoadDerefR16R8(ff00_plus_u8, (0xFF00 + NN), a, A)
LoadDerefR16R8(ff00_plus_c, (0xFF00 + C), a, A)

//~ ld_deref_r16_incr_a

LoadDerefR16R8(hl, HL++, incr_a, A)

//~ R8-X8 load

#define LoadR8R8(a, var_a, b, var_b) \
    Opcode(ld_##a##_##b) {           \
        var_a = var_b;               \
    }
#define Register(name, var)   \
    LoadR8R8(name, var, a, A) \
    LoadR8R8(name, var, b, B) \
    LoadR8R8(name, var, c, C) \
    LoadR8R8(name, var, d, D) \
    LoadR8R8(name, var, e, E) \
    LoadR8R8(name, var, h, H) \
    LoadR8R8(name, var, l, L)
AllR8
#undef Register

#define LoadR8U8(reg, var)  \
    Opcode(ld_##reg##_u8) { \
        var = NN;           \
    }
CreateAllR8(LoadR8U8)

Opcode(ldi_deref_hl_a) {
    GB_WriteU8(gb, HL++, A);
}
Opcode(ldi_a_deref_hl) {
    A = GB_ReadU8(gb, HL++);
}
Opcode(ldd_deref_hl_a) {
    GB_WriteU8(gb, HL--, A);
}
Opcode(ldd_a_deref_hl) {
    A = GB_ReadU8(gb, HL--);
}

//~ R16-X16 load

#define LoadR16U16(reg, var) \
    Opcode(ld_##reg##_u16) { \
        var = NNNN;          \
    }
CreateAllR16(LoadR16U16)
Opcode(ld_sp_hl) {
    SP = HL;
    GB_Cycle1M(gb);
}

Opcode(ld_hl_sp_plus_i8) {
    ClearFlag(Flag_N);
    ClearFlag(Flag_Z);
    i8 nn = (i8) NN;
    ToggleFlagConditional(Flag_H, (SP & 0xF) + (nn & 0xF) > 0xF);
    ToggleFlagConditional(Flag_C, (SP & 0xFF) + (nn & 0xFF) > 0xFF);
    HL = SP + nn;
    GB_Cycle1M(gb);
}

//~ Stack

Opcode(ld_deref_u16_sp) {
    GB_WriteU16(gb, NNNN, SP);
}

#define PushR16(reg, var)         \
    Opcode(push_##reg) {          \
        GB_Cycle1M(gb);           \
        SP -= 2;                  \
        GB_WriteU16(gb, SP, var); \
    }
PushR16(af, AF)
PushR16(bc, BC)
PushR16(de, DE)
PushR16(hl, HL)

#define PopR16(reg, var)          \
    Opcode(pop_##reg) {           \
        var = GB_ReadU16(gb, SP); \
        SP += 2;                  \
    }
Opcode(pop_af) {
    F = GB_ReadU8(gb, SP++) & 0xF0;
    A = GB_ReadU8(gb, SP++);
}
PopR16(bc, BC)
PopR16(de, DE)
PopR16(hl, HL)

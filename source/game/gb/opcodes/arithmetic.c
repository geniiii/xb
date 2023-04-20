//~ Increment

#define Inc8(reg, var)                                     \
    Opcode(inc_##reg) {                                    \
        ClearFlag(Flag_N);                                 \
        ToggleFlagConditional(Flag_H, (var & 0xF) == 0xF); \
        ToggleFlagConditional(Flag_Z, ++var == 0);         \
    }
CreateAllR8(Inc8)
Opcode(inc_deref_hl) {
    ClearFlag(Flag_N);
    u8 var = GB_ReadU8(gb, HL);
    ToggleFlagConditional(Flag_H, (var & 0xF) == 0xF);
    ToggleFlagConditional(Flag_Z, ++var == 0);
    GB_WriteU8(gb, HL, var);
}

#define Inc16(reg, var) \
    Opcode(inc_##reg) { \
        ++var;          \
        GB_Cycle1M(gb); \
    }
CreateAllR16(Inc16)

//~ Decrement

#define DecR8(reg, var)                                  \
    Opcode(dec_##reg) {                                  \
        SetFlag(Flag_N);                                 \
        ToggleFlagConditional(Flag_H, (var & 0xF) == 0); \
        ToggleFlagConditional(Flag_Z, --var == 0);       \
    }
CreateAllR8(DecR8)
Opcode(dec_deref_hl) {
    SetFlag(Flag_N);
    u8 var = GB_ReadU8(gb, HL);
    ToggleFlagConditional(Flag_H, (var & 0xF) == 0);
    ToggleFlagConditional(Flag_Z, --var == 0);
    GB_WriteU8(gb, HL, var);
}

#define DecR16(reg, var) \
    Opcode(dec_##reg) {  \
        --var;           \
        GB_Cycle1M(gb);  \
    }
CreateAllR16(DecR16)

//~ Bitwise

#define BitwiseOpA(name, op, reg, var, h_flag) \
    Opcode(name##_a_##reg) {                   \
        ClearFlag(Flag_N);                     \
        h_flag(Flag_H);                        \
        ClearFlag(Flag_C);                     \
        A op## = var;                          \
        ToggleFlagConditional(Flag_Z, A == 0); \
    }
#define BitwiseA(name, op, h_flag)       \
    BitwiseOpA(name, op, a, A, h_flag)   \
    BitwiseOpA(name, op, b, B, h_flag)   \
    BitwiseOpA(name, op, c, C, h_flag)   \
    BitwiseOpA(name, op, d, D, h_flag)   \
    BitwiseOpA(name, op, e, E, h_flag)   \
    BitwiseOpA(name, op, h, H, h_flag)   \
    BitwiseOpA(name, op, l, L, h_flag)   \
    BitwiseOpA(name, op, u8, NN, h_flag) \
    BitwiseOpA(name, op, deref_hl, GB_ReadU8(gb, HL), h_flag)

BitwiseA(or, |, ClearFlag)
BitwiseA(and, &, SetFlag)
BitwiseA(xor, ^, ClearFlag)

Opcode(cpl) {
    SetFlag(Flag_N);
    SetFlag(Flag_H);
    A = ~A;
}

// TODO(geni): All of these A < var comparisons may be wrong!

//~ 8-bit Subtraction

internal inline void GB_Sub(Gameboy* gb, u8 var) {
    SetFlag(Flag_N);
    ToggleFlagConditional(Flag_H, (((A & 0xF) - (var & 0xF)) & 0x10) == 0x10);
    ToggleFlagConditional(Flag_C, A < var);
    A -= var;
    ToggleFlagConditional(Flag_Z, A == 0);
}
#define SubA(reg, var)    \
    Opcode(sub_a_##reg) { \
        GB_Sub(gb, var);  \
    }
Opcode(sub_a_a) {
    SetFlag(Flag_Z);
    SetFlag(Flag_N);
    ClearFlag(Flag_H);
    ClearFlag(Flag_C);
    A = 0;
}
CreateAllR8NoA(SubA)
SubA(u8, NN)
SubA(deref_hl, GB_ReadU8(gb, HL))

internal inline void GB_SBC(Gameboy* gb, u8 var) {
    SetFlag(Flag_N);
    ToggleFlagConditional(Flag_H, (((A & 0xF) - (var & 0xF) - GetFlag(Flag_C)) & 0x10) == 0x10);
    u8 result = A - var - GetFlag(Flag_C);
    ToggleFlagConditional(Flag_C, A < var + GetFlag(Flag_C));
    A = result;
    ToggleFlagConditional(Flag_Z, A == 0);
}
#define SubCarryA(reg, var) \
    Opcode(sbc_a_##reg) {   \
        GB_SBC(gb, var);    \
    }
Opcode(sbc_a_a) {
    SetFlag(Flag_N);
    ToggleFlagConditional(Flag_H, GetFlag(Flag_C));
    A = 0 - GetFlag(Flag_C);
    ToggleFlagConditional(Flag_Z, A == 0);
}
CreateAllR8NoA(SubCarryA)
SubCarryA(u8, NN)
SubCarryA(deref_hl, GB_ReadU8(gb, HL))

//~ 8-bit Addition

internal inline void GB_Add(Gameboy* gb, u8 var) {
    ClearFlag(Flag_N);
    ToggleFlagConditional(Flag_H, (((A & 0xF) + (var & 0xF)) & 0x10) == 0x10);
    u16 new_var = A + var;
    ToggleFlagConditional(Flag_C, new_var > XTAL_U8_MAX);
    A = (u8) new_var;
    ToggleFlagConditional(Flag_Z, A == 0);
}
#define AddA(reg, var)    \
    Opcode(add_a_##reg) { \
        GB_Add(gb, var);  \
    }
Opcode(add_a_a) {
    ClearFlag(Flag_N);
    // TODO(geni): Not sure if this is correct
    ToggleFlagConditional(Flag_H, (A & 0xF) >= 8);
    ToggleFlagConditional(Flag_C, A >= 128);
    A *= 2;
    ToggleFlagConditional(Flag_Z, A == 0);
}
CreateAllR8NoA(AddA)
AddA(u8, NN)
AddA(deref_hl, GB_ReadU8(gb, HL))

Opcode(add_sp_i8) {
    i8  nn          = (i8) NN;
    i32 full_result = SP + nn;
    u16 result      = (u16) full_result;

    ClearFlag(Flag_Z);
    ClearFlag(Flag_N);
    ToggleFlagConditional(Flag_H, ((SP ^ nn ^ (full_result & 0xFFFF)) & 0x10) == 0x10);
    ToggleFlagConditional(Flag_C, ((SP ^ nn ^ (full_result & 0xFFFF)) & 0x100) == 0x100);

    SP = result;
    GB_Cycle2M(gb);
}

internal inline void GB_ADC(Gameboy* gb, u8 var) {
    ClearFlag(Flag_N);
    ToggleFlagConditional(Flag_H, (((A & 0xF) + (var & 0xF) + GetFlag(Flag_C)) & 0x10) == 0x10);
    u16 new_var = A + var + GetFlag(Flag_C);
    ToggleFlagConditional(Flag_C, new_var > XTAL_U8_MAX);
    A = (u8) new_var;
    ToggleFlagConditional(Flag_Z, A == 0);
}
#define AddCarryA(reg, var) \
    Opcode(adc_a_##reg) {   \
        GB_ADC(gb, var);    \
    }
Opcode(adc_a_a) {
    ClearFlag(Flag_N);
    ToggleFlagConditional(Flag_H, (((A & 0xF) + (A & 0xF) + GetFlag(Flag_C)) & 0x10) == 0x10);
    u8 new_var = A + A + GetFlag(Flag_C);
    // NOTE(geni): 127 * 2 + 1 = 255, making it impossible for the carry flag to make it overflow
    ToggleFlagConditional(Flag_C, A > XTAL_U8_MAX / 2);
    A = new_var;
    ToggleFlagConditional(Flag_Z, A == 0);
}
CreateAllR8NoA(AddCarryA)
AddCarryA(u8, NN)
AddCarryA(deref_hl, GB_ReadU8(gb, HL))

//~ 16-bit Addition

// TODO(geni): Write our own half-carry check

#define AddHLR16(reg, var)                                            \
    Opcode(add_hl_##reg) {                                            \
        ClearFlag(Flag_N);                                            \
        u32 new_var = HL + var;                                       \
        ToggleFlagConditional(Flag_H, (new_var ^ HL ^ var) & 0x1000); \
        ToggleFlagConditional(Flag_C, new_var > XTAL_U16_MAX);        \
        HL = (u16) new_var;                                           \
        GB_Cycle1M(gb);                                               \
    }
Opcode(add_hl_hl) {
    ClearFlag(Flag_N);
    ToggleFlagConditional(Flag_H, (HL & 0x800) >= 2048);
    ToggleFlagConditional(Flag_C, HL >= 32768);
    HL *= 2;
    GB_Cycle1M(gb);
}
CreateAllR16NoHL(AddHLR16)

//~ Comparison

internal inline void GB_CP(Gameboy* gb, u8 var) {
    SetFlag(Flag_N);
    ToggleFlagConditional(Flag_Z, A == var);
    ToggleFlagConditional(Flag_C, A < var);
    ToggleFlagConditional(Flag_H, (((A & 0xF) - (var & 0xF)) & 0x10) == 0x10);
}
#define CompareA(reg, var) \
    Opcode(cp_a_##reg) {   \
        GB_CP(gb, var);    \
    }
Opcode(cp_a_a) {
    SetFlag(Flag_N);
    SetFlag(Flag_Z);
    ClearFlag(Flag_H);
    ClearFlag(Flag_C);
}
CreateAllR8NoA(CompareA)
CompareA(u8, NN)
CompareA(deref_hl, GB_ReadU8(gb, HL))

Opcode(daa) {
    // TODO(geni): Clean up
    if (!GetFlag(Flag_N)) {
        if (GetFlag(Flag_C) || A > 0x99) {
            A += 0x60;
            SetFlag(Flag_C);
        }
        if (GetFlag(Flag_H) || (A & 0x0F) > 0x09) {
            A += 0x6;
        }
    } else {
        if (GetFlag(Flag_C)) {
            A -= 0x60;
        }
        if (GetFlag(Flag_H)) {
            A -= 0x6;
        }
    }
    ToggleFlagConditional(Flag_Z, A == 0);
    ClearFlag(Flag_H);
}

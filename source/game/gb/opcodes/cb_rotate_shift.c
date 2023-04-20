//~ Rotate left

#define RotateLeftR8(reg, var)                         \
    CBOpcode(rlc_##reg) {                              \
        ClearFlag(Flag_N);                             \
        ClearFlag(Flag_H);                             \
        ToggleFlagConditional(Flag_C, GetBit(var, 7)); \
        var = (var << 1) | (var >> 7);                 \
        ToggleFlagConditional(Flag_Z, var == 0);       \
    }
CreateAllR8(RotateLeftR8)
CBOpcode(rlc_deref_hl) {
    ClearFlag(Flag_N);
    ClearFlag(Flag_H);

    u8 var = GB_ReadU8(gb, HL);
    ToggleFlagConditional(Flag_C, GetBit(var, 7));
    var = (var << 1) | (var >> 7);
    ToggleFlagConditional(Flag_Z, var == 0);
    GB_WriteU8(gb, HL, var);
}

#define RotateLeftThroughCarryR8(reg, var)           \
    CBOpcode(rl_##reg) {                             \
        ClearFlag(Flag_N);                           \
        ClearFlag(Flag_H);                           \
        u8 new_carry = GetBit(var, 7);               \
        var          = (var << 1) | GetFlag(Flag_C); \
        ToggleFlagConditional(Flag_C, new_carry);    \
        ToggleFlagConditional(Flag_Z, var == 0);     \
    }
CreateAllR8(RotateLeftThroughCarryR8)
CBOpcode(rl_deref_hl) {
    ClearFlag(Flag_N);
    ClearFlag(Flag_H);
    u8 var       = GB_ReadU8(gb, HL);
    u8 new_carry = GetBit(var, 7);
    var          = (var << 1) | GetFlag(Flag_C);
    ToggleFlagConditional(Flag_C, new_carry);
    ToggleFlagConditional(Flag_Z, var == 0);
    GB_WriteU8(gb, HL, var);
}

//~ Rotate right

#define RotateRightR8(reg, var)                        \
    CBOpcode(rrc_##reg) {                              \
        ClearFlag(Flag_N);                             \
        ClearFlag(Flag_H);                             \
        ToggleFlagConditional(Flag_C, GetBit(var, 0)); \
        var = ((var & 1) << 7) | (var >> 1);           \
        ToggleFlagConditional(Flag_Z, var == 0);       \
    }
CreateAllR8(RotateRightR8)
CBOpcode(rrc_deref_hl) {
    ClearFlag(Flag_N);
    ClearFlag(Flag_H);

    u8 var = GB_ReadU8(gb, HL);
    ToggleFlagConditional(Flag_C, GetBit(var, 0));
    var = ((var & 1) << 7) | (var >> 1);
    ToggleFlagConditional(Flag_Z, var == 0);
    GB_WriteU8(gb, HL, var);
}

#define RotateRightThroughCarryR8(reg, var)                 \
    CBOpcode(rr_##reg) {                                    \
        ClearFlag(Flag_N);                                  \
        ClearFlag(Flag_H);                                  \
        u8 new_carry = GetBit(var, 0);                      \
        var          = (GetFlag(Flag_C) << 7) | (var >> 1); \
        ToggleFlagConditional(Flag_C, new_carry);           \
        ToggleFlagConditional(Flag_Z, var == 0);            \
    }
CreateAllR8(RotateRightThroughCarryR8)
CBOpcode(rr_deref_hl) {
    ClearFlag(Flag_N);
    ClearFlag(Flag_H);
    u8 var       = GB_ReadU8(gb, HL);
    u8 new_carry = GetBit(var, 0);
    var          = (GetFlag(Flag_C) << 7) | (var >> 1);
    ToggleFlagConditional(Flag_C, new_carry);
    ToggleFlagConditional(Flag_Z, var == 0);
    GB_WriteU8(gb, HL, var);
}

//~ Shift left

#define ShiftLeftArithmeticR8(reg, var)                \
    CBOpcode(sla_##reg) {                              \
        ClearFlag(Flag_N);                             \
        ClearFlag(Flag_H);                             \
        ToggleFlagConditional(Flag_C, GetBit(var, 7)); \
        var = ((i8) var) << 1;                         \
        ToggleFlagConditional(Flag_Z, var == 0);       \
    }
CreateAllR8(ShiftLeftArithmeticR8)
CBOpcode(sla_deref_hl) {
    ClearFlag(Flag_N);
    ClearFlag(Flag_H);

    u8 var = GB_ReadU8(gb, HL);
    ToggleFlagConditional(Flag_C, GetBit(var, 7));
    var = ((i8) var) << 1;
    ToggleFlagConditional(Flag_Z, var == 0);
    GB_WriteU8(gb, HL, var);
}

//~ Shift right

#define ShiftRightArithmeticR8(reg, var)         \
    CBOpcode(sra_##reg) {                        \
        ClearFlag(Flag_N);                       \
        ClearFlag(Flag_H);                       \
        ToggleFlagConditional(Flag_C, var & 1);  \
        var = ((i8) var) >> 1;                   \
        ToggleFlagConditional(Flag_Z, var == 0); \
    }
CreateAllR8(ShiftRightArithmeticR8)
CBOpcode(sra_deref_hl) {
    ClearFlag(Flag_N);
    ClearFlag(Flag_H);

    u8 var = GB_ReadU8(gb, HL);
    ToggleFlagConditional(Flag_C, var & 1);
    var = ((i8) var) >> 1;
    ToggleFlagConditional(Flag_Z, var == 0);

    GB_WriteU8(gb, HL, var);
}

#define ShiftRightLogicR8(reg, var)              \
    CBOpcode(srl_##reg) {                        \
        ClearFlag(Flag_N);                       \
        ClearFlag(Flag_H);                       \
        ToggleFlagConditional(Flag_C, var & 1);  \
        var >>= 1;                               \
        ToggleFlagConditional(Flag_Z, var == 0); \
    }
CreateAllR8(ShiftRightLogicR8)
CBOpcode(srl_deref_hl) {
    ClearFlag(Flag_N);
    ClearFlag(Flag_H);

    u8 var = GB_ReadU8(gb, HL);
    ToggleFlagConditional(Flag_C, var & 1);
    var >>= 1;
    ToggleFlagConditional(Flag_Z, var == 0);

    GB_WriteU8(gb, HL, var);
}

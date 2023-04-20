//~ Swap

#define SwapR8(reg, var)                         \
    CBOpcode(swap_##reg) {                       \
        ClearFlag(Flag_N);                       \
        ClearFlag(Flag_H);                       \
        ClearFlag(Flag_C);                       \
        var = (var << 4) | (var >> 4);           \
        ToggleFlagConditional(Flag_Z, var == 0); \
    }
CreateAllR8(SwapR8)
CBOpcode(swap_deref_hl) {
    ClearFlag(Flag_N);
    ClearFlag(Flag_H);
    ClearFlag(Flag_C);
    u8 var = GB_ReadU8(gb, HL);
    var    = (var << 4) | (var >> 4);
    ToggleFlagConditional(Flag_Z, var == 0);
    GB_WriteU8(gb, HL, var);
}

//~ Bit tests

#define TestBitX8(reg, var, n)                              \
    CBOpcode(bit_##n##_##reg) {                             \
        ToggleFlagConditional(Flag_Z, GetBit(var, n) == 0); \
        ClearFlag(Flag_N);                                  \
        SetFlag(Flag_H);                                    \
    }
#define TestBitsX8(reg, var) \
    TestBitX8(reg, var, 0)   \
    TestBitX8(reg, var, 1)   \
    TestBitX8(reg, var, 2)   \
    TestBitX8(reg, var, 3)   \
    TestBitX8(reg, var, 4)   \
    TestBitX8(reg, var, 5)   \
    TestBitX8(reg, var, 6)   \
    TestBitX8(reg, var, 7)
CreateAllR8(TestBitsX8)
TestBitsX8(deref_hl, GB_ReadU8(gb, HL))

//~ Bit clears

#define ResetBitX8(reg, var, n) \
    CBOpcode(res_##n##_##reg) { \
        ClearBit(var, n);       \
    }
#define ResetBitsX8(reg, var) \
    ResetBitX8(reg, var, 0)   \
    ResetBitX8(reg, var, 1)   \
    ResetBitX8(reg, var, 2)   \
    ResetBitX8(reg, var, 3)   \
    ResetBitX8(reg, var, 4)   \
    ResetBitX8(reg, var, 5)   \
    ResetBitX8(reg, var, 6)   \
    ResetBitX8(reg, var, 7)
CreateAllR8(ResetBitsX8)
#undef ResetBitX8
#define ResetBitX8(reg, var, n)                              \
    CBOpcode(res_##n##_deref_##reg) {                        \
        GB_WriteU8(gb, var, GB_ReadU8(gb, var) & ~(1 << n)); \
    }
ResetBitsX8(hl, HL)

//~ Bit sets

#define SetBitX8(reg, var, n)   \
    CBOpcode(set_##n##_##reg) { \
        SetBit(var, n);         \
    }
#define SetBitsX8(reg, var) \
    SetBitX8(reg, var, 0)   \
    SetBitX8(reg, var, 1)   \
    SetBitX8(reg, var, 2)   \
    SetBitX8(reg, var, 3)   \
    SetBitX8(reg, var, 4)   \
    SetBitX8(reg, var, 5)   \
    SetBitX8(reg, var, 6)   \
    SetBitX8(reg, var, 7)
CreateAllR8(SetBitsX8)
#undef SetBitX8
#define SetBitX8(reg, var, n)                               \
    CBOpcode(set_##n##_deref_##reg) {                       \
        GB_WriteU8(gb, var, GB_ReadU8(gb, var) | (1 << n)); \
    }
SetBitsX8(hl, HL)

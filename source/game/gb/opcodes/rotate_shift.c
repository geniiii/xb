Opcode(rlca) {
    ClearFlag(Flag_Z);
    ClearFlag(Flag_N);
    ClearFlag(Flag_H);
    ToggleFlagConditional(Flag_C, GetBit(A, 7));
    A = (A << 1) | (A >> 7);
}

Opcode(rla) {
    ClearFlag(Flag_Z);
    ClearFlag(Flag_N);
    ClearFlag(Flag_H);
    u8 new_carry = GetBit(A, 7);
    A            = (A << 1) | GetFlag(Flag_C);
    ToggleFlagConditional(Flag_C, new_carry);
}

Opcode(rrca) {
    ClearFlag(Flag_Z);
    ClearFlag(Flag_N);
    ClearFlag(Flag_H);
    ToggleFlagConditional(Flag_C, GetBit(A, 0));
    A = ((A & 0x1) << 7) | (A >> 1);
}

Opcode(rra) {
    ClearFlag(Flag_Z);
    ClearFlag(Flag_N);
    ClearFlag(Flag_H);
    u8 new_carry = GetBit(A, 0);
    A            = (GetFlag(Flag_C) << 7) | (A >> 1);
    ToggleFlagConditional(Flag_C, new_carry);
}

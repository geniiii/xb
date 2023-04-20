#define A    (gb->cpu.regs.a)
#define F    (gb->cpu.regs.flags)
#define AF   (gb->cpu.regs.af)
#define B    (gb->cpu.regs.b)
#define C    (gb->cpu.regs.c)
#define BC   (gb->cpu.regs.bc)
#define D    (gb->cpu.regs.d)
#define E    (gb->cpu.regs.e)
#define DE   (gb->cpu.regs.de)
#define H    (gb->cpu.regs.h)
#define L    (gb->cpu.regs.l)
#define HL   (gb->cpu.regs.hl)
#define SP   (gb->cpu.regs.sp)
#define PC   (gb->cpu.regs.pc)
#define NN   GB_GetNN(gb)
#define NNNN GB_GetNNNN(gb)

// clang-format off
#define CreateAllR8NoA(f) \
    f(b, B) \
    f(c, C) \
    f(d, D) \
    f(e, E) \
    f(h, H) \
    f(l, L)
#define CreateAllR8(f) \
    f(a, A) \
    CreateAllR8NoA(f)
#define CreateAllR16NoHL(f) \
    f(bc, BC) \
    f(de, DE) \
    f(sp, SP)
#define CreateAllR16(f) \
    f(hl, HL) \
    CreateAllR16NoHL(f)
// clang-format on

#define GetFlag(flag)                    GetBit(F, flag)
#define SetFlag(flag)                    SetBit(F, flag)
#define ToggleFlagConditional(flag, ...) ToggleBitConditional(F, flag, __VA_ARGS__)
#define ClearFlag(flag)                  ClearBit(F, flag)

#define Opcode(name)                     internal void Interpret_##name(Gameboy* gb)
#define CBOpcode(name)                   Opcode(cb_##name)

#include "opcodes/arithmetic.c"
#include "opcodes/load.c"
#include "opcodes/rotate_shift.c"
#include "opcodes/control.c"
#include "opcodes/cb_rotate_shift.c"
#include "opcodes/cb_bit.c"

#define OpcodeInfoManual(name, format, size, func_name) {S8LitComp(format), size, func_name},
#define OpcodeInfo(name, format, size)                  OpcodeInfoManual(name, format, size, Interpret_##name)
global const OpcodeInfo opcodes[Opcodes_Count] = {
#include "opcodes.inc"
};
#undef OpcodeInfoManual
#undef OpcodeInfo

#define OpcodeInfo(name, format) {S8LitComp(format), 0, Interpret_cb_##name},
global const OpcodeInfo cb_opcodes[CBOpcodes_Count] = {
#include "cb_opcodes.inc"
};
#undef OpcodeInfo

#undef A
#undef F
#undef AF
#undef B
#undef C
#undef BC
#undef D
#undef E
#undef DE
#undef H
#undef L
#undef HL
#undef SP
#undef PC
#undef NN
#undef NNNN

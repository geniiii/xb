typedef void (*OpcodeFunc)(Gameboy* gb);

typedef struct {
    String8    format;
    u8         size;
    OpcodeFunc func;
} OpcodeInfo;

#define OpcodeInfoManual(name, ...) Opcode_##name,
#define OpcodeInfo(name, ...)       Opcode_##name,
enum Opcodes {
#include "opcodes.inc"
    Opcodes_Count,
};
#undef OpcodeInfoManual
#undef OpcodeInfo

#define OpcodeInfo(name, ...) CBOpcode_##name,
enum CBOpcodes {
#include "cb_opcodes.inc"
    CBOpcodes_Count,
};
#undef OpcodeInfo

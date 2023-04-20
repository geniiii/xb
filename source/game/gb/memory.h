#define VRAM_START     0x8000
#define VRAM_END       0x9FFF

#define RAM_START      0xA000
#define RAM_END        0xBFFF

#define ROM0_END       0x3FFF
#define ROM1_START     0x4000
#define ROM1_END       0x7FFF

#define WRAM_START     0xC000
#define WRAM_END       0xDFFF

#define ECHO_START     0xE000
#define ECHO_END       0xFDFF

#define OAM_START      0xFE00
#define OAM_END        0xFE9F

#define UNUSABLE_START 0xFEA0
#define UNUSABLE_END   0xFEFF

#define IO_START       0xFF00
#define IO_END         0xFF7F

#define HRAM_START     0xFF80
#define HRAM_END       0xFFFE

internal u8* GB_GetU8(Gameboy* gb, u16 addr);

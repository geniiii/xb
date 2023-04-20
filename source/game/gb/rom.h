typedef u8 ROMType;
enum {
#define MBC(name)                         ROMType_##name,
#define MBCFlags(name, flags)             MBC(name)
#define MBCIndex(name, index)             ROMType_##name = index,
#define MBCIndexFlags(name, index, flags) MBCIndex(name, index)
#define MBCUnlicensed(...)
#include "mbcs.inc"
#undef MBC
#undef MBCFlags
#undef MBCIndex
#undef MBCIndexFlags
#undef MBCUnlicensed
    ROMType_Count,
};

#pragma pack(push, 1)
typedef struct {
    // NOTE(geni): Junk
    u8 code[4];
    u8 logo[48];

    // TODO(geni): Not always 16!
    u8 title[16];
    u8 licensee[2];

    u8      sgb;
    ROMType type;

    u8 size;
    u8 ram;

    u8 destination;
    u8 old_licensee;

    u8  version;
    u8  checksum;
    u16 global_checksum;
} ROMHeader;
#pragma pack(pop)

internal inline u32 GB_CalculateROMSize(u8 size) {
    return Kilobytes(32) * (1 << size);
}
internal inline u8 GB_CalculateRAMBanks(u8 size) {
    // TODO(geni): Index 1 should actually be 2KB, not a full 8KB bank
    local_persist const u8 ram_sizes[] = {0, 1, 1, 4, 16, 8};
    return size > 0 && size < ArrayCount(ram_sizes) ? ram_sizes[size] : 0;
}

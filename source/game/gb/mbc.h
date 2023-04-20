enum {
    MBCFlag_HasRAM     = (1 << 0),
    MBCFlag_HasBattery = (1 << 1),
    MBCFlag_HasTimer   = (1 << 2),
    MBCFlag_HasRumble  = (1 << 3),
};
typedef u32 MBCFlags;

typedef u8 ROMType;
enum {
#define MBC(name)                         MBCType_##name,
#define MBCFlags(name, flags)             MBC(name)
#define MBCIndex(name, index)             MBCType_##name = index,
#define MBCIndexFlags(name, index, flags) MBCIndex(name, index)
#define MBCUnlicensed(name, flags)        MBCType_Unlicensed_##name,
#include "mbcs.inc"
#undef MBC
#undef MBCFlags
#undef MBCIndex
#undef MBCIndexFlags
#undef MBCUnlicensed
    MBCType_Count,
};
typedef u32 MBCType;

typedef union {
    struct {
        u8 s;
        u8 m;
        u8 h;
        u8 d;
        u8 f;
    };
    u8 raw[5];
} MBC3_RTC;

typedef struct {
    u8 ramg;
    u8 romb0;
    u8 romb1;
    u8 mode;
} MBC1;

typedef struct {
    u8 ramg;
    u8 romb0;
    u8 romb1;
    u8 ramb;
} MBC5;

// TODO(geni): MBC30 requires GBC support
typedef struct {
    MBC3_RTC rtc;
    MBC3_RTC rtc_latched;
    u8       rtc_mapped;
    u8       rtc_is_latched;
    u32      rtc_subseconds;
    b32      rtc_latched_yet;

    u8 ramg;
    u8 romb;
    u8 ramb;
} MBC3;

typedef struct {
    u8 ramg;
    u8 romb0;
    u8 romb1;
    u8 mode;
} NTOLD2;

typedef struct {
    MBCFlags flags;
    MBCType  type;
    union {
        MBC1   mbc1;
        MBC3   mbc3;
        MBC5   mbc5;
        NTOLD2 ntold2;
    };
    u16 rom_bank0;
    u16 rom_bank;

    b32 ram_enabled;
    u8* ram;
    u8  ram_bank;
    u8  ram_banks;
} MBC;

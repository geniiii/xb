internal MBCFlags GB_GetMBCFlags(MBCType type) {
    local_persist const MBCFlags flags[MBCType_Count] = {
#define MBC(...)
#define MBCIndex(...)
#define MBCFlags(name, flags)             [MBCType_##name] = flags,
#define MBCIndexFlags(name, index, flags) MBCFlags(name, flags)
#define MBCUnlicensed(name, flags)        [MBCType_Unlicensed_##name] = flags,
#include "mbcs.inc"
#undef MBC
#undef MBCIndex
#undef MBCFlags
#undef MBCIndexFlags
#undef MBCUnlicensed
    };

    return flags[type];
}

internal String8 GB_GetMBCName(MBCType type) {
    local_persist const String8 strings[MBCType_Count] = {
#define MBC(name)                         [MBCType_##name] = S8LitComp(#name),
#define MBCIndex(name, index)             MBC(name)
#define MBCFlags(name, flags)             MBC(name)
#define MBCIndexFlags(name, index, flags) MBC(name)
#define MBCUnlicensed(name, flags)        MBC(Unlicensed_##name)
#include "mbcs.inc"
#undef MBC
#undef MBCIndex
#undef MBCFlags
#undef MBCIndexFlags
#undef MBCUnlicensed
    };

    String8 result = type < ArrayCount(strings) ? strings[type] : (String8){0};
    if (result.data == NULL) {
        return S8Lit("Unknown");
    }
    return result;
}

//~ MBC1

internal void GB_MBC1Init(Gameboy* gb) {
    MBC1* mbc1       = &gb->mbc.mbc1;
    mbc1->romb0      = 1;
    gb->mbc.rom_bank = 1;
}

internal force_inline b32 GB_MBC1HandleWrite(Gameboy* gb, u16 addr, u8 value) {
    MBC1* mbc1 = &gb->mbc.mbc1;

    if (addr <= 0x1FFF) {
        mbc1->ramg          = (value & 0xF) == 0xA;
        gb->mbc.ram_enabled = mbc1->ramg;
    } else if (addr <= 0x3FFF) {
        mbc1->romb0 = Max(value & 0x1F, 1);
    } else if (addr <= 0x5FFF) {
        mbc1->romb1 = value & 3;
    } else if (addr <= 0x7FFF) {
        mbc1->mode = value & 1;
    } else {
        return 0;
    }

    gb->mbc.rom_bank0 = ((mbc1->romb1 << 5) * mbc1->mode) & gb->rom_banks - 1;
    gb->mbc.rom_bank  = ((mbc1->romb1 << 5) | mbc1->romb0) & gb->rom_banks - 1;
    gb->mbc.ram_bank  = (mbc1->romb1 & gb->mbc.ram_banks - 1) * mbc1->mode;
    return 1;
}

//~ MBC2

internal force_inline b32 GB_MBC2HandleWrite(Gameboy* gb, u16 addr, u8 value) {
    if (addr <= 0x3FFF) {
        if (addr & 0x100) {
            gb->mbc.rom_bank = Max(value & 0xF, 1) & gb->rom_banks - 1;
        } else {
            gb->mbc.ram_enabled = (value & 0xF) == 0xA;
        }
        return 1;
    } else if (addr >= 0xA000 && addr <= 0xBFFF) {
        if (gb->mbc.ram_enabled) {
            gb->mbc.ram[addr & 0x1FF] = 0xF0 | (value & 0xF);
        }
        return 1;
    }

    return 0;
}

internal force_inline u8* GB_MBC2HandleGet(Gameboy* gb, u16 addr) {
    if (addr >= 0xA000 && addr <= 0xBFFF) {
        return &gb->mbc.ram[addr & 0x1FF];
    }
    return NULL;
}

//~ MBC3

enum {
    MBC3_FBit_Day,
    MBC3_FBit_Halt = 6,
    MBC3_FBit_DayCarry,
    MBC3_FBitCount,
};
enum {
    MBC3_Register_S,
    MBC3_Register_M,
    MBC3_Register_H,
    MBC3_Register_DL,
    MBC3_Register_DH,
};

internal void GB_MBC3Init(Gameboy* gb) {
    MBC3* mbc3       = &gb->mbc.mbc3;
    mbc3->romb       = 1;
    gb->mbc.rom_bank = 1;

    if (gb->mbc.flags & MBCFlag_HasTimer) {
        Xtal_OSDateTime time = xtal_os->GetDateTime();
        mbc3->rtc.s          = time.seconds;
        mbc3->rtc.m          = time.minutes;
        mbc3->rtc.h          = time.hour;
        mbc3->rtc.d          = time.year_day & 0xFF;
        mbc3->rtc.f |= GetBit(time.year_day, 8);
    }
}

internal force_inline void _GB_MBC3IncrementRTC(Gameboy* gb) {
    MBC3* mbc3 = &gb->mbc.mbc3;
    if (++mbc3->rtc.s == 60) {
        mbc3->rtc.s = 0;
        if (++mbc3->rtc.m == 60) {
            mbc3->rtc.m = 0;
            if (++mbc3->rtc.h == 24) {
                mbc3->rtc.h = 0;
                u16 day     = ((GetBit(mbc3->rtc.f, MBC3_FBit_Day) << 8) | mbc3->rtc.d) + 1;
                if (GetBit(day, 9)) {
                    SetBit(mbc3->rtc.f, MBC3_FBit_DayCarry);
                }
                mbc3->rtc.d = day & 0xFF;
                // TODO(geni): There's probably a better bitwise op I can do for this
                ToggleBitConditional(mbc3->rtc.f, MBC3_FBit_Day, GetBit(day, 8));
            }
        }
    }
}

internal force_inline void GB_MBC3Handle(Gameboy* gb, u8 cycles) {
    MBC3* mbc3 = &gb->mbc.mbc3;
    if (!mbc3->ramg || GetBit(mbc3->rtc.f, MBC3_FBit_Halt)) {
        return;
    }

    mbc3->rtc_subseconds += cycles;
    if (mbc3->rtc_subseconds >= 4194304) {
        mbc3->rtc_subseconds -= 4194304;
        _GB_MBC3IncrementRTC(gb);
    }
}

internal force_inline b32 GB_MBC3HandleRead(Gameboy* gb, u16 addr, u8* out) {
    MBC3* mbc3 = &gb->mbc.mbc3;
    if (addr >= 0xA000 && addr <= 0xBFFF && mbc3->ramg && mbc3->rtc_mapped) {
        if (!mbc3->rtc_latched_yet) {
            *out = 0xFF;
            return 1;
        }

        u8 reg    = mbc3->rtc_mapped - 8;
        u8 result = reg <= 0xC ? mbc3->rtc_latched.raw[reg] : 0xFF;
        switch (reg) {
            case MBC3_Register_S:
            case MBC3_Register_M: {
                result &= 0x3F;
            } break;
            case MBC3_Register_H: {
                result &= 0x1F;
            } break;
            case MBC3_Register_DH: {
                result &= 0xC1;
            } break;
            default: break;
        }
        *out = result;
        return 1;
    }

    return 0;
}

internal force_inline b32 GB_MBC3HandleWrite(Gameboy* gb, u16 addr, u8 value) {
    MBC3* mbc3    = &gb->mbc.mbc3;
    b32   has_rtc = gb->mbc.flags & MBCFlag_HasTimer;

    if (addr <= 0x1FFF) {
        mbc3->ramg          = (value & 0xF) == 0xA;
        gb->mbc.ram_enabled = mbc3->ramg;
        // TODO(geni): Not sure this is necessary
        // gb->mbc.ram_bank = (mbc3->ramb & gb->mbc.ram_banks - 1) * (mbc3->ramg && mbc3->rtc_mapped == 0);
    } else if (addr <= 0x3FFF) {
        mbc3->romb       = value & 0x7F;
        gb->mbc.rom_bank = Max(mbc3->romb, 1) & gb->rom_banks - 1;
    } else if (addr <= 0x5FFF) {
        value &= 0xF;
        if ((value >= 4 && value <= 7) || (value >= 0xD && value <= 0xF)) {
            // NOTE(geni): RAMB has been set to an invalid value. We just pretend RAM is disabled so that it reads 0xFF.
            gb->mbc.ram_enabled = 0;
            mbc3->rtc_mapped    = 0;
        } else {
            if (has_rtc && value >= 8) {
                mbc3->rtc_mapped = value;
            } else if (gb->mbc.ram != NULL) {
                mbc3->ramb       = value & 3;
                mbc3->rtc_mapped = 0;
                gb->mbc.ram_bank = (mbc3->ramb & gb->mbc.ram_banks - 1) * mbc3->ramg;
            }
            gb->mbc.ram_enabled = mbc3->ramg;
        }
    } else if (has_rtc && addr <= 0x7FFF) {
        value &= 0xF;
        if (!mbc3->rtc_is_latched && value == 1) {
            mbc3->rtc_latched     = mbc3->rtc;
            mbc3->rtc_latched_yet = 1;
        }
        mbc3->rtc_is_latched = value == 1;
    } else if (has_rtc && mbc3->ramg && mbc3->rtc_mapped > 0 && addr >= 0xA000 && addr <= 0xBFFF) {
        u8 reg = mbc3->rtc_mapped - 8;
        if (reg == MBC3_Register_S) {
            mbc3->rtc_subseconds = 0;
        }
        mbc3->rtc.raw[reg]         = value;
        mbc3->rtc_latched.raw[reg] = value;
    } else {
        return 0;
    }

    return 1;
}

//~ MBC5

internal void GB_MBC5Init(Gameboy* gb) {
    MBC5* mbc5       = &gb->mbc.mbc5;
    mbc5->romb1      = 1;
    gb->mbc.rom_bank = 1;
}

internal force_inline b32 GB_MBC5HandleWrite(Gameboy* gb, u16 addr, u8 value) {
    MBC5* mbc5 = &gb->mbc.mbc5;

    if (addr <= 0x1FFF) {
        mbc5->ramg          = value == 0xA;
        gb->mbc.ram_enabled = mbc5->ramg;
        gb->mbc.ram_bank    = (mbc5->ramb & gb->mbc.ram_banks - 1) * mbc5->ramg;
    } else if (addr <= 0x2FFF) {
        mbc5->romb0      = value;
        gb->mbc.rom_bank = ((mbc5->romb1 << 8) | mbc5->romb0) & gb->rom_banks - 1;
    } else if (addr <= 0x3FFF) {
        mbc5->romb1      = value & 1;
        gb->mbc.rom_bank = ((mbc5->romb1 << 8) | mbc5->romb0) & gb->rom_banks - 1;
    } else if (addr <= 0x5FFF) {
        mbc5->ramb       = value & 0xF;
        gb->mbc.ram_bank = (mbc5->ramb & gb->mbc.ram_banks - 1) * mbc5->ramg;
    } else {
        return 0;
    }

    return 1;
}

//~ NTOLD2

internal void GB_NTOLD2Init(Gameboy* gb) {
    NTOLD2* mbc      = &gb->mbc.ntold2;
    mbc->romb0       = 0;
    mbc->romb1       = 1;
    gb->mbc.rom_bank = 1;
    //    gb->mbc.ram_enabled = 1;
}

global const u8 _gb_ntold2pattern[] = {
    // clang-format off
    1, 2, 0, 3, 4, 5, 6, 7,
    // clang-format on
};

internal force_inline u8 _GB_NTOLDReorder(u8 value, const u8* pattern) {
    u8 new_value = 0;
    for (u8 i = 0; i < 8; ++i) {
        new_value |= GetBit(value, pattern[i]) << i;
    }
    return new_value;
}

internal b32 GB_NTOLD2HandleWrite(Gameboy* gb, u16 addr, u8 value) {
    NTOLD2* mbc = &gb->mbc.ntold2;
    if (addr <= 0x1FFF) {
        mbc->ramg           = (value & 0xF) == 0xA;
        gb->mbc.ram_enabled = mbc->ramg;
        return 1;
    } else if (addr <= 0x3FFF) {
        /*         if (mbc->mode) {
                    value = _GB_NTOLDReorder(value, _gb_ntold2pattern);
                } */
        mbc->romb1       = value & 0x7F;
        gb->mbc.rom_bank = Max(mbc->romb1, 1) & gb->rom_banks - 1;
        return 1;
    } else if (addr >= 0x5000 && addr <= 0x5FFF) {
        u8 op = addr & 3;
        switch (op) {
            case 0: {
                LogWarning("Unimplemented");
            } break;
            case 1: {
                value &= 0x3F;
                mbc->romb0 = value << 1;
                if (mbc->romb0 > 0) {
                    gb->mbc.rom_bank0 = mbc->romb0 & gb->rom_banks - 1;
                    mbc->romb1        = mbc->romb0 + 1;
                    gb->mbc.rom_bank  = mbc->romb1 & gb->rom_banks - 1;
                }
            } break;
            case 2: {
                if ((value & 0xF0) == 0xE0) {
                    gb->mbc.ram_banks = 4;
                    if (!gb->mbc.ram) {
                        gb->mbc.ram = Xtal_MArenaPushZero(&gb->arena, gb->mbc.ram_banks * Kilobytes(8));
                    } else {
                        // TODO(geni): The RAM should probably be a dynamic array
                        LogError("Unimplemented! The guest will break in unexpected ways and likely attempt to execute 0xFF soon.");
                    }
                }
                gb->rom_banks = (0x10 - (value & 0xF)) << 1;
            } break;
            case 3: {
                mbc->mode   = (value & 0x10) == 0x10;
                u8 new_bank = mbc->romb1;
                if (mbc->mode) {
                    new_bank = _GB_NTOLDReorder(new_bank, _gb_ntold2pattern);
                }
                mbc->romb1       = new_bank;
                gb->mbc.rom_bank = Max(mbc->romb1, 1) & gb->rom_banks - 1;
            } break;
            default: {
                LogWarning("Unimplemented");
            } break;
        }
        return 1;
    }
    return 0;
}

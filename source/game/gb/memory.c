internal force_inline void GB_CheckLYC(Gameboy* gb) {
    if (gb->cpu.hw_regs.ly == gb->cpu.hw_regs.lyc) {
        SetBit(gb->cpu.hw_regs.stat, STATBit_LYC);
        if (GetBit(gb->cpu.hw_regs.stat, STATBit_LYCInt)) {
            GB_RequestInterrupt(gb, Interrupt_STAT);
        }
    } else {
        ClearBit(gb->cpu.hw_regs.stat, STATBit_LYC);
    }
}

internal inline u8* GB_GetU8(Gameboy* gb, u16 addr) {
    // NOTE(geni): ROM
    if (addr <= ROM0_END) {
        return &gb->rom[addr + (Kilobytes(16) * gb->mbc.rom_bank0)];
    }
    if (addr <= ROM1_END) {
        return &gb->rom[addr - ROM1_START + (Kilobytes(16) * gb->mbc.rom_bank)];
    }

    if (addr <= VRAM_END) {
        // TODO(geni): Block access during specific PPU modes (requires Pixel FIFO for correct timings)
        return &gb->vram[addr - VRAM_START];
    }

    // NOTE(geni): External RAM
    if (addr <= RAM_END) {
        if (gb->mbc.ram == NULL) {
            LogWarning("Guest attempted to access RAM when none is present on cartridge");
            return NULL;
        }
        if (!gb->mbc.ram_enabled) {
            LogWarning("Guest attempted to access disabled RAM");
            return NULL;
        }
        switch (gb->mbc.type) {
            case MBCType_MBC2:
            case MBCType_MBC2Battery: {
                return GB_MBC2HandleGet(gb, addr);
            }
        }

        return &gb->mbc.ram[addr - RAM_START + (Kilobytes(8) * gb->mbc.ram_bank)];
    }

    // NOTE(geni): WRAM
    if (addr <= WRAM_END) {
        return &gb->wram[addr - WRAM_START];
    }

    // NOTE(geni): Echo memory
    if (addr <= ECHO_END) {
        return &gb->wram[addr - ECHO_START];
    }

    // NOTE(geni): OAM
    if (addr <= OAM_END) {
        // TODO(geni): Block access during specific PPU modes (requires Pixel FIFO for correct timings)
        if (GB_DMAInProgress(gb)) {
            return NULL;
        }
        return &gb->oam[addr - OAM_START];
    }

    // NOTE(geni): Unusable
    if (addr <= UNUSABLE_END) {
        // TODO(geni): Proper implementation of this region
        return NULL;
    }

    if (addr <= IO_END) {
        switch (addr & 0xFF) {
            case 0x00: return &gb->cpu.hw_regs.p1;
            case 0x01: return &gb->cpu.hw_regs.sb;
            case 0x02: return &gb->cpu.hw_regs.sc;
            case 0x04: return &gb->cpu.hw_regs.div;
            case 0x05: return &gb->cpu.hw_regs.tima;
            case 0x06: return &gb->cpu.hw_regs.tma;
            case 0x07: return &gb->cpu.hw_regs.tac;
            case 0x0F: return &gb->cpu.hw_regs.int_flag;
            case 0x40: return &gb->cpu.hw_regs.lcdc;
            case 0x41: return &gb->cpu.hw_regs.stat;
            case 0x42: return &gb->cpu.hw_regs.scy;
            case 0x43: return &gb->cpu.hw_regs.scx;
            case 0x44: return &gb->cpu.hw_regs.ly;
            case 0x45: return &gb->cpu.hw_regs.lyc;
            case 0x46: return &gb->mmu.dma.source;
            case 0x47: return &gb->cpu.hw_regs.bgp;
            case 0x48: return &gb->cpu.hw_regs.obp0;
            case 0x49: return &gb->cpu.hw_regs.obp1;
            case 0x4A: return &gb->cpu.hw_regs.wy;
            case 0x4B: return &gb->cpu.hw_regs.wx;
        }
        return NULL;
        // TODO(geni): Proper behaviour for each individual I/O port
    }

    if (addr <= HRAM_END) {
        return &gb->hram[addr - HRAM_START];
    }

    if (addr == 0xFFFF) {
        return &gb->cpu.hw_regs.int_enable;
    }

    LogError("Guest attempted to access unimplemented memory at %X", addr);
    return NULL;
}

internal u8 GB_ReadU8NoCycle(Gameboy* gb, u16 addr) {
    switch (gb->mbc.type) {
        case MBCType_MBC3TimerBattery:
        case MBCType_MBC3TimerRAMBattery: {
            u8 out;
            if (GB_MBC3HandleRead(gb, addr, &out)) {
                return out;
            }
        } break;
    }

    if (addr == 0xFF00) {
        u8 p1 = gb->cpu.hw_regs.p1;
        if (!GetBit(p1, 4)) {
            // NOTE(geni): Direction keys
            return (p1 & 0x30) | (gb->input & 0xF);
        } else if (!GetBit(p1, 5)) {
            // NOTE(geni): Action keys
            return (p1 & 0x30) | (gb->input >> 4);
        }
        return 0x3F;
    }
    if (addr == 0xFF0F) {
        return gb->cpu.hw_regs.int_flag | 0xE0;
    }
    if (addr == 0xFFFF) {
        return gb->cpu.hw_regs.int_enable | 0xE0;
    }

    u8* got = GB_GetU8(gb, addr);
    return got ? *got : 0xFF;
}
internal u8 GB_ReadU8(Gameboy* gb, u16 addr) {
    GB_Cycle1M(gb);
    return GB_ReadU8NoCycle(gb, addr);
}
internal u16 GB_ReadU16(Gameboy* gb, u16 addr) {
    return GB_ReadU8(gb, addr) | (GB_ReadU8(gb, addr + 1) << 8);
}

internal void GB_WriteU8NoCycle(Gameboy* gb, u16 addr, u8 value) {
    b32 mbc_write = 0;
    switch (gb->mbc.type) {
        case MBCType_MBC1:
        case MBCType_MBC1RAM:
        case MBCType_MBC1RAMBattery: {
            mbc_write = GB_MBC1HandleWrite(gb, addr, value);
        } break;
        case MBCType_MBC3:
        case MBCType_MBC3RAM:
        case MBCType_MBC3RAMBattery:
        case MBCType_MBC3TimerBattery:
        case MBCType_MBC3TimerRAMBattery: {
            mbc_write = GB_MBC3HandleWrite(gb, addr, value);
        } break;
        case MBCType_MBC2:
        case MBCType_MBC2Battery: {
            mbc_write = GB_MBC2HandleWrite(gb, addr, value);
        } break;
        case MBCType_MBC5:
        case MBCType_MBC5RAM:
        case MBCType_MBC5RAMBattery: {
            mbc_write = GB_MBC5HandleWrite(gb, addr, value);
        } break;
        case MBCType_Unlicensed_NTOLD2: {
            mbc_write = GB_NTOLD2HandleWrite(gb, addr, value);
        } break;
    }
    if (mbc_write) {
        return;
    }

    if (addr <= ROM1_END) {
        // LogWarning("Guest attempted to write to ROM at %X", addr);
        return;
    }

    // TODO(geni): This is probably really dumb
    if (addr >= IO_START && addr <= IO_END) {
        switch (addr & 0xFF) {
            case 0x04: GB_ClearDIV(gb); break;
            case 0x05: {
                if (!gb->cpu.timer.reset_tima) {
                    gb->cpu.hw_regs.tima = value;
                }
            } break;
            case 0x07: GB_WriteTAC(gb, value); break;
            case 0x44: break;
            case 0x45: {
                gb->cpu.hw_regs.lyc = value;
                // TODO(geni): Not sure about this
                GB_CheckLYC(gb);
            } break;
            case 0x46: GB_DMARequest(gb, value); break;
            default: goto write;
        }
        return;
    }

write:
    u8* got = GB_GetU8(gb, addr);
    if (got) {
        *got = value;
    }
}
internal void GB_WriteU8(Gameboy* gb, u16 addr, u8 value) {
    GB_Cycle1M(gb);
    GB_WriteU8NoCycle(gb, addr, value);
}
internal void GB_WriteU16(Gameboy* gb, u16 addr, u16 value) {
    GB_WriteU8(gb, addr, value & 0xFF);
    GB_WriteU8(gb, addr + 1, value >> 8);
}

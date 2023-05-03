internal u8 GB_GetNN(Gameboy* gb) {
    return GB_ReadU8(gb, gb->cpu.regs.pc++);
}
internal u16 GB_GetNNNN(Gameboy* gb) {
    u16 result = GB_ReadU16(gb, gb->cpu.regs.pc);
    gb->cpu.regs.pc += 2;
    return result;
}

internal void GB_Init(Gameboy* gb) {
    Xtal_MemorySet(&gb->cpu, 0, sizeof gb->cpu);
    Xtal_MemorySet(&gb->ppu, 0, sizeof gb->ppu);
    Xtal_MemorySet(&gb->rom_header, 0, sizeof gb->rom_header);
    Xtal_MemorySet(&gb->mmu, 0, sizeof gb->mmu);
    Xtal_MemorySet(&gb->mbc, 0, sizeof gb->mbc);
    Xtal_MemorySet(gb->wram, 0, sizeof gb->wram);
    Xtal_MemorySet(gb->vram, 0, sizeof gb->vram);
    Xtal_MemorySet(gb->hram, 0, sizeof gb->hram);
    Xtal_MemorySet(gb->oam, 0, sizeof gb->oam);

    gb->cpu.regs.a = 0x01;
    gb->cpu.regs.b = 0xFF;
    gb->cpu.regs.c = 0x13;
    gb->cpu.regs.e = 0xC1;
    gb->cpu.regs.h = 0x84;
    gb->cpu.regs.l = 0x03;

    gb->cpu.regs.pc = 0x0100;

    gb->cpu.hw_regs.p1       = 0xCF;
    gb->cpu.hw_regs.sc       = 0x7E;
    gb->cpu.timer.raw_div    = 0x18 << 8;
    gb->cpu.hw_regs.div      = gb->cpu.timer.raw_div >> 8;
    gb->cpu.hw_regs.tac      = 0xF8;
    gb->cpu.hw_regs.int_flag = 0xE1;
    gb->cpu.hw_regs.lcdc     = 0x91;
    gb->cpu.hw_regs.stat     = 0x81;
    gb->cpu.hw_regs.ly       = 0x91;
    gb->cpu.hw_regs.bgp      = 0xFC;
    gb->mmu.dma.addr         = 0xFFFF;
    gb->mmu.dma.source       = 0xFF;

    gb->cpu.regs.sp = 0xFFFE;

    gb->ppu.mode = PPUMode_OAMScan;
}

internal i32 GB_OpenROMDialog(void* user_data) {
    Unreferenced(user_data);

    if (game->gb.pending_arena.base == NULL) {
        game->gb.pending_arena = Xtal_MArenaNew();
    }
    Xtal_FilePickerResult file = xtal_os->CreateFileOpenDialog(&game->gb.pending_arena);
    if (file.succeeded) {
        game->gb.pending_rom_path = file.path;
        // TODO(geni): I'm not sure atomics are necessary
        Xtal_AtomicI32Exchange(&game->gb.pending_reload, 1);
    }

    return 1;
}
internal void GB_CreateOpenROMDialog(void) {
    Xtal_Thread thread = xtal_os->CreateThread(GB_OpenROMDialog, NULL, XTAL_THREAD_DEFAULTSTACKSIZE);
    xtal_os->DetachThread(&thread);
}

typedef struct {
    u8       ram;
    u8       size;
    MBCFlags flags;
    MBCType  type;
} GB_UnlicensedHeaderOverride;
internal b32 GB_GetUnlicensedROMTypeFromHash(u64 hash, GB_UnlicensedHeaderOverride* unlicensed) {
    // NOTE(geni): No real point in using a hashmap for this, I'm pretty sure a switch case will usually be faster anyways
    b32 result = 1;
    switch (hash) {
        case 0x3D05EDE56F5158B6: {  // NOTE(geni): "SONIC 7"
            *unlicensed = (GB_UnlicensedHeaderOverride){
                .type  = MBCType_Unlicensed_NTOLD2,
                .flags = MBCFlag_HasRAM,
                .size  = 4,
                .ram   = 3,
            };
        } break;
        case 0xB941FE7ED5E51F5: {  // NOTE(geni): "23 in 1"
            *unlicensed = (GB_UnlicensedHeaderOverride){
                .type  = MBCType_Unlicensed_NTOLD2,
                .flags = MBCFlag_HasRAM,
                .size  = 5,
                .ram   = 3,
            };
        } break;

        default: {
            result = 0;
        } break;
    }
    return result;
}

internal void
GB_LoadROM(Gameboy* gb, String8 path) {
    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
    Xtal_OS_File    file    = xtal_os->OpenFile(path, Xtal_OS_FilePermission_Read, Xtal_OS_FileOpen_OpenExisting);
    u8*             rom_data;
    u64             rom_size;
    xtal_os->LoadEntireFile(file, scratch.arena, (void**) &rom_data, &rom_size);
    xtal_os->CloseFile(file);

    gb->rom_header   = *(ROMHeader*) (rom_data + 0x100);
    gb->mbc.rom_bank = 1;
    gb->mbc.flags    = GB_GetMBCFlags(gb->rom_header.type);
    gb->mbc.type     = gb->rom_header.type;
    // TODO(geni): Switch to seed 0 (gotta rehash headers)
    gb->rom_header_hash = XXH64(&gb->rom_header, sizeof gb->rom_header, 0xFFFFFFFFFFFFFFFFull);
    GB_UnlicensedHeaderOverride override;
    b32                         unlicensed = GB_GetUnlicensedROMTypeFromHash(gb->rom_header_hash, &override);
    if (unlicensed) {
        gb->mbc.type        = override.type;
        gb->mbc.flags       = override.flags;
        gb->rom_header.ram  = override.ram;
        gb->rom_header.size = override.size;
        LogWarning("Loading unlicensed ROM (header hash: 0x%08llX)", gb->rom_header_hash);
    }
    gb->rom_banks         = 1 << (gb->rom_header.size + 1);
    u32 reported_rom_size = GB_CalculateROMSize(gb->rom_header.size);
    gb->rom               = Xtal_MArenaPushZero(&gb->arena, reported_rom_size);
    Xtal_MemoryCopy(gb->rom, rom_data, reported_rom_size);
    Xtal_MArenaTempEnd(scratch);

    if (gb->mbc.flags & MBCFlag_HasRAM || gb->mbc.type == MBCType_ROMOnly) {
        switch (gb->mbc.type) {
            case MBCType_MBC2:
            case MBCType_MBC2Battery: {
                gb->mbc.ram = Xtal_MArenaPushZero(&gb->arena, 512);
                for (u32 i = 0; i < 512; ++i) {
                    gb->mbc.ram[i] = 0xF0;
                }
            } break;
            default: {
                gb->mbc.ram_banks = GB_CalculateRAMBanks(gb->rom_header.ram);
                if (gb->mbc.ram_banks > 0) {
                    gb->mbc.ram = Xtal_MArenaPushZero(&gb->arena, gb->mbc.ram_banks * Kilobytes(8));
                }
            } break;
        }
    }

    switch (gb->mbc.type) {
        case MBCType_ROMOnly: {
            gb->mbc.ram_enabled = 1;
            gb->mbc.rom_bank    = 1;
        } break;
        case MBCType_MBC1:
        case MBCType_MBC1RAM:
        case MBCType_MBC1RAMBattery: {
            GB_MBC1Init(gb);
        } break;
        case MBCType_MBC3:
        case MBCType_MBC3RAM:
        case MBCType_MBC3RAMBattery:
        case MBCType_MBC3TimerBattery:
        case MBCType_MBC3TimerRAMBattery: {
            GB_MBC3Init(gb);
        } break;
        case MBCType_MBC5:
        case MBCType_MBC5RAM:
        case MBCType_MBC5RAMBattery:
        case MBCType_MBC5Rumble:
        case MBCType_MBC5RumbleRAM:
        case MBCType_MBC5RumbleRAMBattery: {
            GB_MBC5Init(gb);
        } break;
        case MBCType_Unlicensed_NTOLD2: {
            GB_NTOLD2Init(gb);
        } break;

        default: {
            Log("Either loading an unsupported MBC type, or the game doesn't use an MBC");
            gb->mbc.rom_bank = 1;
        }
    }

    gb->rom_loaded = 1;
}

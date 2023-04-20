#define GB_2BPPToPalID(low, high, x) ((low >> (7 - x) & 1) | ((high >> (7 - x) & 1) << 1))
#define GB_PalIDToColorID(pal)       ((gb->cpu.hw_regs.bgp >> (pal * 2)) & 3)

#define PPUMODE_HBLANK_CYCLES        204
#define PPUMODE_VBLANK_CYCLES        456
#define PPUMODE_DRAW_CYCLES          172
#define PPUMODE_OAMSCAN_CYCLES       80

global const u32 colors[4] = {
    0xFFFFFFFF,
    0xFFAAAAAA,
    0xFF555555,
    0xFF000000,
};

internal force_inline u8 GB_DrawScanlineGetPalID(u8 x_offset, u8 y_offset, const u8* tilemap, const u8* tiledata, b32 signed_addr) {
    u8 tile_id = tilemap[y_offset / 8 * 32 + x_offset / 8];

    u8 tile_x = x_offset % 8;
    u8 tile_y = y_offset % 8;

    // NOTE(geni): Multiply by 16 and 2 because 2BPP
    u8 low_byte  = tiledata[(signed_addr ? (i8) tile_id : tile_id) * 16 + tile_y * 2];
    u8 high_byte = tiledata[(signed_addr ? (i8) tile_id : tile_id) * 16 + tile_y * 2 + 1];

    return GB_2BPPToPalID(low_byte, high_byte, tile_x);
}

internal force_inline void GB_DrawScanlineBG(Gameboy* gb, u8 x, const u8* tilemap, const u8* tiledata, b32 signed_addr) {
    u8 y = gb->ppu.scanline;

    u8 pal_id                                     = GB_DrawScanlineGetPalID(x + gb->cpu.hw_regs.scx, y + gb->cpu.hw_regs.scy, tilemap, tiledata, signed_addr);
    gb->ppu.fb[y * FRAMEBUFFER_WIDTH + x]         = colors[GB_PalIDToColorID(pal_id)];
    gb->ppu.bg_palette[y * FRAMEBUFFER_WIDTH + x] = pal_id;
}

internal force_inline b32 GB_DrawScanlineW(Gameboy* gb, u8 x, const u8* tilemap, const u8* tiledata, b32 signed_addr) {
    u8 y_offset = gb->ppu.window_counter;
    if (y_offset >= SCREEN_HEIGHT || gb->ppu.scanline < gb->cpu.hw_regs.wy) {
        return 0;
    }

    u8 x_offset = x - gb->cpu.hw_regs.wx + 7;
    if (x_offset >= SCREEN_WIDTH || gb->cpu.hw_regs.wx >= SCREEN_WIDTH) {
        return 0;
    }

    u8 pal_id                                                    = GB_DrawScanlineGetPalID(x_offset, y_offset, tilemap, tiledata, signed_addr);
    gb->ppu.fb[gb->ppu.scanline * FRAMEBUFFER_WIDTH + x]         = colors[GB_PalIDToColorID(pal_id)];
    gb->ppu.bg_palette[gb->ppu.scanline * FRAMEBUFFER_WIDTH + x] = pal_id;
    return 1;
}

internal void GB_DrawScanlineSprites(Gameboy* gb) {
    b32 large = GetBit(gb->cpu.hw_regs.lcdc, LCDCBit_OBJSize);

    SpriteEntry* sprites[10];
    u8           drawn = 0;
    for (u8 i = 0; i < 40 && drawn < 10; ++i) {
        SpriteEntry* spr = (SpriteEntry*) gb->oam + i;

        u8 y = spr->y - 16;
        if (y <= gb->ppu.scanline && (large ? spr->y : y + 8) > gb->ppu.scanline) {
            sprites[drawn++] = spr;
        }
    }

    // NOTE(geni): Wacky inline insertion sort
    for (u32 i = 1; i < drawn; ++i) {
        for (u32 j = i; j > 0 && sprites[j - 1]->x <= sprites[j]->x; --j) {
            SpriteEntry* temp = sprites[j - 1];
            sprites[j - 1]    = sprites[j];
            sprites[j]        = temp;
        }
    }

    for (u32 i = 0; i < drawn; ++i) {
        SpriteEntry* spr = sprites[i];

        u8 x = spr->x - 8;
        u8 y = spr->y - 16;

        if (y <= gb->ppu.scanline && (large ? spr->y : y + 8) > gb->ppu.scanline) {
            b32 hflip = GetBit(spr->flags, OAMBit_XFlip);
            b32 yflip = GetBit(spr->flags, OAMBit_YFlip);

            u8 tile_y = gb->ppu.scanline - y;
            if (yflip) {
                tile_y = (large ? 15 : 7) - tile_y;
            }

            for (u8 dx = 0; dx < 8; ++dx) {
                u32 fb_pos = gb->ppu.scanline * FRAMEBUFFER_WIDTH + x + dx;
                if (x + dx >= 0 && x + dx <= SCREEN_WIDTH) {
                    if (GetBit(spr->flags, OAMBIT_BGWindowOverOBJ) && gb->ppu.bg_palette[fb_pos] != 0) {
                        continue;
                    }

                    u8 tile = spr->index;
                    if (large && (tile_y <= 7 || (yflip && tile_y > 7))) {
                        tile &= 0xFE;
                    }

                    u8 low_byte  = gb->vram[tile * 16 + tile_y * 2];
                    u8 high_byte = gb->vram[tile * 16 + tile_y * 2 + 1];

                    u8 pal_id  = hflip ? (low_byte >> dx & 1) | ((high_byte >> dx & 1) << 1) : GB_2BPPToPalID(low_byte, high_byte, dx);
                    u8 palette = GetBit(spr->flags, OAMBit_Palette) ? gb->cpu.hw_regs.obp1 : gb->cpu.hw_regs.obp0;
                    u8 color   = (palette >> (pal_id * 2)) & 3;
                    if (pal_id != 0) {
                        gb->ppu.fb[fb_pos] = colors[color];
                    }
                }
            }
        }
    }
}

internal inline void GB_DrawScanline(Gameboy* gb) {
    u8* bg_tilemap      = GetBit(gb->cpu.hw_regs.lcdc, LCDCBit_BGTileMapArea) ? &gb->vram[0x1C00] : &gb->vram[0x1800];
    u8* w_tilemap       = GetBit(gb->cpu.hw_regs.lcdc, LCDCBit_WindowTileMapArea) ? &gb->vram[0x1C00] : &gb->vram[0x1800];
    b32 signed_tiledata = !GetBit(gb->cpu.hw_regs.lcdc, LCDCBit_TileDataArea);
    u8* tiledata        = signed_tiledata ? &gb->vram[0x1000] : gb->vram;

    b32 draw_bg     = GetBit(gb->cpu.hw_regs.lcdc, LCDCBit_BGWindowEnable);
    b32 draw_window = GetBit(gb->cpu.hw_regs.lcdc, LCDCBit_WindowEnable);

    b32 drew_window = 0;
    for (u8 x = 0; x < FRAMEBUFFER_WIDTH; ++x) {
        if (draw_bg) {
            GB_DrawScanlineBG(gb, x, bg_tilemap, tiledata, signed_tiledata);
            if (draw_window) {
                drew_window |= GB_DrawScanlineW(gb, x, w_tilemap, tiledata, signed_tiledata);
            }
        } else {
            gb->ppu.fb[gb->ppu.scanline * FRAMEBUFFER_WIDTH + x] = colors[gb->cpu.hw_regs.bgp & 3];
        }
    }

    if (GetBit(gb->cpu.hw_regs.lcdc, LCDCBit_OBJEnable)) {
        GB_DrawScanlineSprites(gb);
    }

    if (drew_window) {
        ++gb->ppu.window_counter;
    }
}

internal force_inline void GB_SetLY(Gameboy* gb, u8 value) {
    gb->cpu.hw_regs.ly = value;
    GB_CheckLYC(gb);
}

internal void GB_HandlePPU(Gameboy* gb, u8 cycles) {
    // TODO(geni): Horrendously janky

    HardAssert(gb->ppu.scanline <= 153);

    if (!GetBit(gb->cpu.hw_regs.lcdc, LCDCBit_Enable)) {
        // NOTE(geni): 0x78 is 0b01111000
        //    gb->ppu.cycles       = 0;
        gb->cpu.hw_regs.stat = gb->cpu.hw_regs.stat & 0xFC;
        return;
    }

    gb->ppu.cycles += cycles;
    switch (gb->ppu.mode) {
        case PPUMode_OAMScan: {
            if (gb->ppu.cycles >= PPUMODE_OAMSCAN_CYCLES) {
                gb->ppu.cycles -= PPUMODE_OAMSCAN_CYCLES;
                gb->ppu.mode = PPUMode_Draw;
            }
        } break;
        case PPUMode_Draw: {
            if (gb->ppu.cycles >= PPUMODE_DRAW_CYCLES) {
                gb->ppu.cycles -= PPUMODE_DRAW_CYCLES;
                gb->ppu.mode = PPUMode_HBlank;
                if (GetBit(gb->cpu.hw_regs.stat, STATBit_HBlankInt)) {
                    GB_RequestInterrupt(gb, Interrupt_STAT);
                }
                GB_DrawScanline(gb);
            }
        } break;
        case PPUMode_HBlank: {
            if (gb->ppu.cycles >= PPUMODE_HBLANK_CYCLES) {
                gb->ppu.cycles -= PPUMODE_HBLANK_CYCLES;
                ++gb->ppu.scanline;
                if (gb->ppu.scanline == 144) {
                    if (GetBit(gb->cpu.hw_regs.stat, STATBit_VBlankInt)) {
                        GB_RequestInterrupt(gb, Interrupt_STAT);
                    }
                    GB_RequestInterrupt(gb, Interrupt_VBlank);
                    gb->ppu.mode = PPUMode_VBlank;
                    XGL_TexUpdate(renderer.gb_fb, game->gb.ppu.fb);
                } else {
                    gb->ppu.mode = PPUMode_OAMScan;
                }
                GB_SetLY(gb, gb->ppu.scanline);
            }
        } break;
        case PPUMode_VBlank: {
            if (gb->ppu.cycles >= PPUMODE_VBLANK_CYCLES) {
                gb->ppu.cycles -= PPUMODE_VBLANK_CYCLES;
                ++gb->ppu.scanline;
                if (gb->ppu.scanline == 154) {
                    if (GetBit(gb->cpu.hw_regs.stat, STATBit_OAMScanInt)) {
                        GB_RequestInterrupt(gb, Interrupt_STAT);
                    }
                    gb->ppu.scanline       = 0;
                    gb->ppu.window_counter = 0;
                    gb->ppu.mode           = PPUMode_OAMScan;
                }
                GB_SetLY(gb, gb->ppu.scanline);
            }
        } break;
    }

    gb->cpu.hw_regs.stat = (gb->cpu.hw_regs.stat & 0xFC) | (gb->ppu.mode & ~0xFC);
}

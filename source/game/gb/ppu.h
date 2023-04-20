enum {
    PPUMode_HBlank,
    PPUMode_VBlank,
    PPUMode_OAMScan,
    PPUMode_Draw,
};
typedef u8 PPUMode;

enum {
    STATBit_LYC = 2,
    STATBit_HBlankInt,
    STATBit_VBlankInt,
    STATBit_OAMScanInt,
    STATBit_LYCInt,
};

enum {
    LCDCBit_BGWindowEnable,
    LCDCBit_OBJEnable,
    LCDCBit_OBJSize,
    LCDCBit_BGTileMapArea,
    LCDCBit_TileDataArea,
    LCDCBit_WindowEnable,
    LCDCBit_WindowTileMapArea,
    LCDCBit_Enable,
};

#define SCREEN_WIDTH       160
#define SCREEN_HEIGHT      144
#define FRAMEBUFFER_WIDTH  SCREEN_WIDTH
#define FRAMEBUFFER_HEIGHT SCREEN_HEIGHT

typedef struct {
    PPUMode mode;
    u32     cycles;
    u8      scanline;
    u8      window_counter;
    b32     fb_needs_update;

    u32 fb[FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT];
    u8  bg_palette[FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT];
} PPU;

// clang-format off
#define Flags \
    Flag(C)  \
    Flag(H)  \
    Flag(N)  \
    Flag(Z)
// clang-format on
enum {
    Flag_NotActuallyAFlag = 3,
#define Flag(name) Flag_##name,
    Flags
#undef Flag
};

typedef struct {
    union {
        struct {
            union {
                struct {
                    u8 flags;
                    u8 a;
                };
                u16 af;
            };
            union {
                struct {
                    u8 c;
                    u8 b;
                };
                u16 bc;
            };
            union {
                struct {
                    u8 e;
                    u8 d;
                };
                u16 de;
            };
            union {
                struct {
                    u8 l;
                    u8 h;
                };
                u16 hl;
            };

            u16 sp;
            u16 pc;
        };
        u8 raw[12];
    } regs;
    u32 cycles;

    struct {
        struct {
            // NOTE(geni): Joypad input
            u8 p1;

            // NOTE(geni): Serial
            u8 sb;
            u8 sc;

            // NOTE(geni): Division and Timer
            u8 div;
            u8 tima;
            u8 tma;
            u8 tac;

            u8 int_flag;

            // NOTE(geni): LCD
            u8 lcdc;
            u8 stat;
            u8 scy;
            u8 scx;
            u8 ly;
            u8 lyc;
            u8 bgp;
            u8 obp0;
            u8 obp1;
            u8 wy;
            u8 wx;
        };

        u8  int_enable;
        b32 ime;
        b32 ime_toggled;
    } hw_regs;

    struct {
        b32 reset_tima;
        u16 raw_div;
        u8  last_and;
    } timer;

    b32 halt;
    b32 halt_bug;

    b32 vblank;
} CPU;

// clang-format off
#define AllR8NoA \
    Register(b, B) \
    Register(c, C) \
    Register(d, D) \
    Register(e, E) \
    Register(h, H) \
    Register(l, L)

#define AllR8 \
    Register(a, A) \
    AllR8NoA
// clang-format on

typedef struct {
    struct {
        b32 active;
        u16 addr;
        u8  source;

        u8 requested;
        u8 requested_pending;

        u8 cycles;
    } dma;
} MMU;

typedef struct {
    CPU       cpu;
    PPU       ppu;
    MMU       mmu;
    ROMHeader rom_header;
    MBC       mbc;

    volatile b32 pending_reload;
    String8      pending_rom_path;

    u8* rom;
    u16 rom_banks;
    u8  wram[Kilobytes(8)];
    u8  vram[Kilobytes(8)];
    u8  hram[127];
    // TODO(geni): Might be better to just use SpriteEntry and cast to u8* when necessary
    u8 oam[sizeof(SpriteEntry) * 40];

    u8 last_input;
    u8 input;

    Xtal_MArena arena;
    // TODO(geni): Uhh.. maybe not the best solution.
    Xtal_MArena pending_arena;

    u64 rom_header_hash;
    b32 rom_loaded;
} Gameboy;

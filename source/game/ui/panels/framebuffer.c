#define VRAM_FRAMEBUFFER_WIDTH  128
#define VRAM_FRAMEBUFFER_HEIGHT 256

PANEL_UPDATE(Framebuffer) {
    Unreferenced(ui);

    if (!game->gb.rom_loaded) {
        return 1;
    }

    struct nk_image fb = nk_image_id((int) renderer.gb_fb.handle);
    // TODO(geni): Figure out why we need the magic values 14 and 50 to make this fit correctly
    if (nk_begin(ctx, "Framebuffer",
                 nk_rect(50, 50, FRAMEBUFFER_WIDTH * 3 + 14, FRAMEBUFFER_HEIGHT * 3 + 50),
                 NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
        struct nk_rect fb_bounds = nk_window_get_bounds(ctx);

        vec2s new_size = {fb_bounds.w - FMod(fb_bounds.w, FRAMEBUFFER_WIDTH), fb_bounds.h - FMod(fb_bounds.h, FRAMEBUFFER_HEIGHT)};
        new_size       = Xtal_Vec2MaintainAspectRatio(new_size, (vec2s){FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT});
        nk_layout_row_static(ctx, new_size.height, (int) new_size.width, 1);

        nk_image(ctx, fb);
    }
    nk_end(ctx);

    return 1;
}

PANEL_UPDATE(VRAM) {
    Unreferenced(ui);

    local_persist u32 vram_framebuffer[VRAM_FRAMEBUFFER_WIDTH * VRAM_FRAMEBUFFER_HEIGHT];

    Gameboy*  gb       = &game->gb;
    const u8* tiledata = gb->vram;

    // NOTE(geni): Stupid layouting code, don't bother looking at this for too long
    {
        const u32 max_tiles   = (VRAM_FRAMEBUFFER_WIDTH * VRAM_FRAMEBUFFER_HEIGHT) / 64;
        const u8  max_tiles_x = VRAM_FRAMEBUFFER_WIDTH / 8;

        for (u32 tile_id = 0; tile_id < max_tiles; ++tile_id) {
            for (u8 tile_y = 0; tile_y < 8; ++tile_y) {
                // NOTE(geni): Multiply by 16 and 2 because 2BPP
                u8 low_byte  = tiledata[tile_id * 16 + tile_y * 2];
                u8 high_byte = tiledata[tile_id * 16 + tile_y * 2 + 1];

                u32 y = 8 * (tile_id / max_tiles_x) + tile_y;

                for (u8 tile_x = 0; tile_x < 8; ++tile_x) {
                    u8  pal_id = GB_2BPPToPalID(low_byte, high_byte, tile_x);
                    u32 x      = 8 * (tile_id % max_tiles_x) + tile_x;

                    vram_framebuffer[y * VRAM_FRAMEBUFFER_WIDTH + x] = colors[GB_PalIDToColorID(pal_id)];
                }
            }
        }
        XGL_TexUpdate(renderer.gb_vram, vram_framebuffer);
    }

    struct nk_image fb = nk_image_id((int) renderer.gb_vram.handle);
    // TODO(geni): Figure out why we need the magic values 14 and 50 to make this fit correctly
    if (nk_begin(ctx, "VRAM",
                 nk_rect(50, 50, VRAM_FRAMEBUFFER_WIDTH * 2 + 14, VRAM_FRAMEBUFFER_HEIGHT * 2 + 50),
                 NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_CLOSABLE | NK_WINDOW_BORDER)) {
        struct nk_rect fb_bounds = nk_window_get_bounds(ctx);

        vec2s new_size = {fb_bounds.w - FMod(fb_bounds.w, VRAM_FRAMEBUFFER_WIDTH), fb_bounds.h - FMod(fb_bounds.h, VRAM_FRAMEBUFFER_HEIGHT)};
        new_size       = Xtal_Vec2MaintainAspectRatio(new_size, (vec2s){VRAM_FRAMEBUFFER_WIDTH, VRAM_FRAMEBUFFER_HEIGHT});
        nk_layout_row_static(ctx, new_size.height, (int) new_size.width, 1);

        nk_image(ctx, fb);
    }
    nk_end(ctx);

    return 1;
}

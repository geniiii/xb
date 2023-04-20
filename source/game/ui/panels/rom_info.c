PANEL_UPDATE(ROMInfo) {
    Unreferenced(ui);

    b32 result = nk_begin(ctx, "ROM Info", nk_rect(5, 5, 300, 150),
                          NK_WINDOW_BORDER | NK_WINDOW_CLOSABLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_TITLE);
    if (result) {
        Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);

        nk_layout_row_dynamic(ctx, 12, 2);
        NK_LabelS8(ctx, S8Lit("MBC Type:"), NK_TEXT_LEFT);
        NK_LabelS8(ctx, S8_PushF(scratch.arena, "0x%02X", game->gb.mbc.type), NK_TEXT_RIGHT);

        NK_LabelS8(ctx, S8Lit("ROM header hash:"), NK_TEXT_LEFT);
        NK_LabelS8(ctx, S8_PushF(scratch.arena, "0x%08llX", game->gb.rom_header_hash), NK_TEXT_RIGHT);

        NK_LabelS8(ctx, S8Lit("ROM size:"), NK_TEXT_LEFT);
        NK_LabelS8(ctx, S8_PushFormattedBytes(scratch.arena, game->gb.rom_banks * Kilobytes(16)), NK_TEXT_RIGHT);

        NK_LabelS8(ctx, S8Lit("RAM size:"), NK_TEXT_LEFT);
        NK_LabelS8(ctx, S8_PushFormattedBytes(scratch.arena, game->gb.mbc.ram_banks * Kilobytes(8)), NK_TEXT_RIGHT);

        Xtal_MArenaTempEnd(scratch);
    }
    nk_end(ctx);

    return result;
}

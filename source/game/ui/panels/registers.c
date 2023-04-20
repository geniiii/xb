PANEL_UPDATE(Registers) {
    Unreferenced(ui);

    b32 result = 1;
    if (nk_begin(ctx, "Registers", nk_rect(500, 50, 175, 300), NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCROLL_AUTO_HIDE | NK_WINDOW_SCALABLE | NK_WINDOW_CLOSABLE | NK_WINDOW_BORDER)) {
        nk_layout_row_dynamic(ctx, 12, 2);

        char v[6 + 1];
        int  v_size;

        nk_text(ctx, "PC:", 3, NK_TEXT_LEFT);
        v_size = stbsp_sprintf(v, "0x%04X", game->gb.cpu.regs.pc);
        nk_text(ctx, v, v_size, NK_TEXT_RIGHT);

        nk_text(ctx, "SP:", 3, NK_TEXT_LEFT);
        v_size = stbsp_sprintf(v, "0x%04X", game->gb.cpu.regs.sp);
        nk_text(ctx, v, v_size, NK_TEXT_RIGHT);

        nk_layout_row_dynamic(ctx, 2, 1);
        NK_HorizontalSeparator(ctx);
        nk_layout_row_dynamic(ctx, 12, 2);

#define Register(var, name)                                     \
    NK_LabelS8(ctx, S8Lit(#name ":"), NK_TEXT_LEFT);            \
    v_size = stbsp_sprintf(v, "0x%02X", game->gb.cpu.regs.var); \
    nk_text(ctx, v, v_size, NK_TEXT_RIGHT);
        AllR8
#undef Register

        if (nk_tree_push(ctx, NK_TREE_TAB, "Flags", NK_MAXIMIZED)) {
            nk_layout_row_dynamic(ctx, 12, 2);
#define Flag(name)                                   \
    NK_LabelS8(ctx, S8Lit(#name ":"), NK_TEXT_LEFT); \
    NK_LabelS8(ctx, GetBit(game->gb.cpu.regs.flags, Flag_##name) ? S8Lit("1") : S8Lit("0"), NK_TEXT_RIGHT);
            Flags
#undef Flag
            nk_tree_pop(ctx);
        }
    } else {
        result = 0;
    }
    nk_end(ctx);
    return result;
}

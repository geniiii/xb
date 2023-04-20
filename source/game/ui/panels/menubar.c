PANEL_UPDATE(Menubar) {
    if (nk_begin(ctx, "Menubar", nk_rect(0, 0, (f32) xtal_os->window_size.width, 20),
                 NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {
        nk_menubar_begin(ctx);

        nk_layout_row_begin(ctx, NK_DYNAMIC, 15, 3);
        {
            nk_layout_row_push(ctx, 0.1f);

            if (nk_menu_begin_label(ctx, "File", NK_TEXT_LEFT, nk_vec2(120, 200))) {
                nk_layout_row_dynamic(ctx, 20, 1);

                if (nk_menu_item_label(ctx, "Load", NK_TEXT_LEFT)) {
                    GB_CreateOpenROMDialog();
                }
                if (nk_menu_item_label(ctx, "Quit", NK_TEXT_LEFT)) {
                    xtal_os->quit = 1;
                }

                nk_menu_end(ctx);
            }

            if (nk_menu_begin_label(ctx, "Debug", NK_TEXT_LEFT, nk_vec2(120, 200))) {
                nk_layout_row_dynamic(ctx, 20, 1);
                if (nk_menu_item_label(ctx, "Registers", NK_TEXT_LEFT)) {
                    UITogglePanel(ui, UIPanel_Registers);
                }
                if (nk_menu_item_label(ctx, "MBC Registers", NK_TEXT_LEFT)) {
                    UITogglePanel(ui, UIPanel_MBCRegisters);
                }
                if (nk_menu_item_label(ctx, "ROM Info", NK_TEXT_LEFT)) {
                    UITogglePanel(ui, UIPanel_ROMInfo);
                }
                if (nk_menu_item_label(ctx, "VRAM", NK_TEXT_LEFT)) {
                    UITogglePanel(ui, UIPanel_VRAM);
                }
                nk_menu_end(ctx);
            }

            if (nk_menu_begin_label(ctx, "Emulation", NK_TEXT_LEFT, nk_vec2(120, 200))) {
                nk_layout_row_dynamic(ctx, 20, 1);

                if (nk_menu_item_label(ctx, "Speed", NK_TEXT_LEFT)) {
                    UITogglePanel(ui, UIPanel_Speed);
                }

                // TODO(geni): Pause, restart, etc

                nk_menu_end(ctx);
            }
        }
        nk_layout_row_end(ctx);

        nk_menubar_end(ctx);
    }
    nk_end(ctx);

    return 1;
}

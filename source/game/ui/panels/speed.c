PANEL_UPDATE(Speed) {
    Unreferenced(ui);

    b32 result = 1;
    if (nk_begin(ctx, "Speed", nk_rect(500, 100, 200, 80), NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCROLL_AUTO_HIDE | NK_WINDOW_SCALABLE | NK_WINDOW_CLOSABLE | NK_WINDOW_BORDER)) {
        nk_layout_row_dynamic(ctx, 0.0f, 1);
        u32 tick_rate = xtal_os->GetTickRate();
        nk_property_int(ctx, "Tick rate", 1, (int*) &tick_rate, 6000, 5, 5);
        if (tick_rate != xtal_os->GetTickRate()) {
            xtal_os->SetTickRate(tick_rate);
        }
    } else {
        result = 0;
    }
    nk_end(ctx);
    return result;
}

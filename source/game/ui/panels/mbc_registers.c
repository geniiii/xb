internal void _UIPanel_MBCRegisters_MBC3RTCRegisters(struct nk_context* ctx, MBC3_RTC* rtc, String8 tab_name) {
    // TODO(geni): The way we hash these really sucks
    if (nk_tree_push_hashed(ctx, NK_TREE_TAB, tab_name.cstr, NK_MINIMIZED, tab_name.data, (int) tab_name.size, __LINE__)) {
        nk_layout_row_dynamic(ctx, 12, 2);

        char v[6 + 1];
        int  v_size;
#define Register(name, var)                         \
    NK_LabelS8(ctx, S8Lit(name ":"), NK_TEXT_LEFT); \
    v_size = stbsp_sprintf(v, "0x%02X", var);       \
    nk_text(ctx, v, v_size, NK_TEXT_RIGHT)
#define Flag(name, index)                           \
    NK_LabelS8(ctx, S8Lit(name ":"), NK_TEXT_LEFT); \
    NK_LabelS8(ctx, GetBit(rtc->f, index) ? S8Lit("1") : S8Lit("0"), NK_TEXT_RIGHT)
        Register("S", rtc->s);
        Register("M", rtc->m);
        Register("H", rtc->h);
        Register("D", rtc->d);
        Register("F", rtc->f);
        if (nk_tree_push_hashed(ctx, NK_TREE_TAB, "Flags", NK_MAXIMIZED, tab_name.data, (int) tab_name.size, __LINE__ + 1)) {
            nk_layout_row_dynamic(ctx, 12, 2);
            Flag("RTCD Bit 8", MBC3_FBit_Day);
            Flag("Timer Halt", MBC3_FBit_Halt);
            Flag("RTCD Carry", MBC3_FBit_DayCarry);
            nk_tree_pop(ctx);
        }
#undef Register
#undef Flag
        nk_tree_pop(ctx);
    }
}
internal void _UIPanel_MBCRegisters_MBC3RTC(struct nk_context* ctx) {
    if (nk_tree_push(ctx, NK_TREE_TAB, "RTC", NK_MINIMIZED)) {
        _UIPanel_MBCRegisters_MBC3RTCRegisters(ctx, &game->gb.mbc.mbc3.rtc, S8Lit("Registers"));
        _UIPanel_MBCRegisters_MBC3RTCRegisters(ctx, &game->gb.mbc.mbc3.rtc_latched, S8Lit("Latched"));

        nk_layout_row_dynamic(ctx, 12, 2);
        char v[10 + 1];
        int  v_size;
        NK_LabelS8(ctx, S8Lit("Subseconds:"), NK_TEXT_LEFT);
        v_size = stbsp_sprintf(v, "0x%08X", game->gb.mbc.mbc3.rtc_subseconds);
        nk_text(ctx, v, v_size, NK_TEXT_RIGHT);

        NK_LabelS8(ctx, S8Lit("RTCL:"), NK_TEXT_LEFT);
        v_size = stbsp_sprintf(v, "0x%02X", game->gb.mbc.mbc3.rtc_is_latched);
        nk_text(ctx, v, v_size, NK_TEXT_RIGHT);

        nk_tree_pop(ctx);
    }
}

PANEL_UPDATE(MBCRegisters) {
    Unreferenced(ui);

    b32 result = 1;
    if (nk_begin(ctx, "MBC", nk_rect(500, 50, 250, 300), NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCROLL_AUTO_HIDE | NK_WINDOW_SCALABLE | NK_WINDOW_CLOSABLE | NK_WINDOW_BORDER)) {
        nk_layout_row_dynamic(ctx, 12, 2);

        NK_LabelS8(ctx, S8Lit("MBC type:"), NK_TEXT_LEFT);
        NK_LabelS8(ctx, GB_GetMBCName(game->gb.mbc.type), NK_TEXT_RIGHT);

#define Register(var, name)                         \
    NK_LabelS8(ctx, S8Lit(name ":"), NK_TEXT_LEFT); \
    v_size = stbsp_sprintf(v, "0x%02X", var);       \
    nk_text(ctx, v, v_size, NK_TEXT_RIGHT)

        char v[6 + 1];
        int  v_size;

        Register(game->gb.mbc.rom_bank0, "ROM bank 0");
        Register(game->gb.mbc.rom_bank, "ROM bank 1");
        Register(game->gb.mbc.ram_enabled, "RAM enabled");
        Register(game->gb.mbc.ram_bank, "RAM bank");

        if (nk_tree_push(ctx, NK_TREE_TAB, "Internal registers", NK_MINIMIZED)) {
            switch (game->gb.mbc.type) {
                case MBCType_MBC1:
                case MBCType_MBC1RAM:
                case MBCType_MBC1RAMBattery: {
                    nk_layout_row_dynamic(ctx, 12, 2);
                    Register(game->gb.mbc.mbc1.ramg, "RAMG");
                    Register(game->gb.mbc.mbc1.mode, "MODE");
                    Register(game->gb.mbc.mbc1.romb0, "ROMB0");
                    Register(game->gb.mbc.mbc1.romb1, "ROMB1");
                } break;
                case MBCType_MBC3TimerBattery:
                case MBCType_MBC3TimerRAMBattery: {
                    _UIPanel_MBCRegisters_MBC3RTC(ctx);
                }  // NOTE(geni): Fallthrough!
                case MBCType_MBC3:
                case MBCType_MBC3RAM:
                case MBCType_MBC3RAMBattery: {
                    nk_layout_row_dynamic(ctx, 12, 2);
                    Register(game->gb.mbc.mbc3.ramg, "RAMG");
                    Register(game->gb.mbc.mbc3.romb, "ROMB");
                    Register(game->gb.mbc.mbc3.ramb, "RAMB");
                } break;
            }
            nk_tree_pop(ctx);
        }
#undef Register
    } else {
        result = 0;
    }
    nk_end(ctx);
    return result;
}

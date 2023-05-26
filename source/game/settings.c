typedef struct {
    u32 tickrate;
} GameSettings;

internal GameSettings GameSettingsNew(Xtal_INI* ini) {
    GameSettings result = {
        .tickrate = 60,
    };

    Xtal_INIVarsHashMap* gb_section = Xtal_INIGetSection(ini, S8Lit("gb"));
    if (gb_section) {
        String8* tickrate = Xtal_INIGetVar(gb_section, S8Lit("tickrate"));
        if (tickrate) {
            result.tickrate = S8_GetFirstI64(*tickrate);
        }

        // TODO(geni): Add more options (like color palette and stuff)
    }

    return result;
}

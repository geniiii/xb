internal void InitUI(UI* ui) {
    UISetPanel(ui, UIPanel_Menubar);
    UISetPanel(ui, UIPanel_Framebuffer);
}

internal void UpdateUI(UI* ui) {
    struct nk_context* ctx = &ui->nk_ctx;

    if (!game->hide_ui) {
#define Panel(name)                              \
    if (UIHasPanel(ui, UIPanel_##name)) {        \
        if (!UIPanel_##name##_Update(ui, ctx)) { \
            UIUnsetPanel(ui, UIPanel_##name);    \
        }                                        \
    }
#include "ui/panels.inc"
#undef Panel
    }

    struct nk_style_item background    = ctx->style.window.fixed_background;
    ctx->style.window.fixed_background = nk_style_item_hide();
    if (nk_begin(ctx, "Messages",
                 nk_rect(5, 25, (f32) xtal_os->window_size.width - 5, (f32) xtal_os->window_size.height - 5),
                 NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_NOT_INTERACTIVE | NK_WINDOW_BACKGROUND)) {
        nk_layout_row_dynamic(ctx, 15, 1);

        if (game->paused) {
            NK_LabelS8(ctx, S8Lit("Paused"), NK_TEXT_LEFT);
        }

        // TODO(geni): Proper FPS sampling
        NK_LabelS8(ctx, S8_PushF(&xtal_os->frame_arena, "FPS: %u", (u32) (1.0f / xtal_os->frametime)), NK_TEXT_LEFT);

        for (Message* message = NULL; GetNextActiveMessage(&message);) {
            struct nk_color color = ctx->style.text.color;
            color.a               = (nk_byte) Floor((message->max_age - message->age / message->max_age) * 255.0f);
            NK_LabelColoredS8(ctx, (String8){message->data, message->size}, NK_TEXT_LEFT, color);
        }
    }
    nk_end(ctx);
    ctx->style.window.fixed_background = background;
}

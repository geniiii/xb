enum {
#define Panel(name) UIPanel_##name,
#include "ui/panels.inc"
#undef Panel

    UIPanel_Count,
};
typedef u64 UIPanel;
#define UI_PANELS_ARRAY_SIZE ((UIPanel_Count + sizeof(UIPanel) - 1) / sizeof(UIPanel))
typedef UIPanel UIPanels[UI_PANELS_ARRAY_SIZE];

typedef struct {
    NK_Context        nk_internal;
    struct nk_context nk_ctx;
    UIPanels          panels;
} UI;

internal inline void UISetPanel(UI* ui, UIPanel panel) {
    ui->panels[panel / sizeof(panel)] |= (1ull << (panel % sizeof(panel)));
}
internal inline void UIUnsetPanel(UI* ui, UIPanel panel) {
    ui->panels[panel / sizeof(panel)] &= ~(1ull << (panel % sizeof(panel)));
}
internal inline b32 UIHasPanel(UI* ui, UIPanel panel) {
    return ui->panels[panel / sizeof(panel)] & (1ull << (panel % sizeof(panel)));
}
internal inline void UITogglePanel(UI* ui, UIPanel panel) {
    ui->panels[panel / sizeof(panel)] ^= (1ull << (panel % sizeof(panel)));
}

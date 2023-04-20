typedef struct {
    b32   state_changed;
    State state;

    Input
    inputs[Input_Count];
    Input
    previous_inputs[Input_Count];

    Message messages[MAX_MESSAGES];
    UI      ui;
    b32     hide_ui;

    b32 paused;

    Gameboy gb;
} GameContext;

global GameContext* game;
global Renderer     renderer;

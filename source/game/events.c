internal void UpdateInput(Xtal_OSEvent* event) {
    for (InputKind i = 0; i < Input_Count; ++i) {
        Input
        *input = &game->inputs[i];
        if (input->key == event->key) {
            if (event->type == Xtal_OSEventType_KeyPress && input->modifiers == event->modifiers) {
                input->pressed = !game->previous_inputs[i].down;
                input->value   = 1.0f;
                input->down    = 1;
            } else if (event->type == Xtal_OSEventType_KeyRelease) {
                input->released = 1;
                input->value    = 0.0f;
                input->down     = 0;
                input->pressed  = 0;
            }
        }
        game->previous_inputs[i] = *input;
    }
}

internal void ResetInputs(void) {
    for (InputKind i = 0; i < Input_Count; ++i) {
        Input
        *input = &game->inputs[i];
        if (input->pressed || input->released) {
            input->pressed  = 0;
            input->released = 0;
        }
    }
}

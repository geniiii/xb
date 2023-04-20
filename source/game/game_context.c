internal void GameSwitchState(GameContext* ctx, State new_state) {
    Log("Switching game state to %u...", new_state);
    game->state_changed = 1;
    state_hotunload_callbacks[ctx->state]();
    ctx->state = new_state;
    state_hotload_callbacks[ctx->state](NULL, 0);
}

internal GameContext* GameContextNew(Xtal_MArena* arena) {
    GameContext* result = Xtal_MArenaPushTypeZero(arena, GameContext);
    {
#define Input(id, _key) result->inputs[Input_##id].key = _key;
#define InputModifiers(id, _key, mod) \
    Input(id, _key)                   \
    result->inputs[Input_##id].modifiers = mod;
#include "inputs.inc"
#undef Input
#undef InputModifiers
        Xtal_MemoryCopy(result->previous_inputs, result->inputs, sizeof result->inputs);
        result->state = State_Game;
    }
    return result;
}

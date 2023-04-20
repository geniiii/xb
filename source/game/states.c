// NOTE(geni): These are all declared in the matching header
// NOTE(geni): We can't do all of these in a macro as the preprocessor only does one pass, meaning we can't do #include directives inside macros :(
State_Update* state_update_callbacks[State_Count] = {
#define State(name) [State_##name] = name##_Update,
#include "states.inc"
#undef State
};

State_Tick* state_tick_callbacks[State_Count] = {
#define State(name) [State_##name] = name##_Tick,
#include "states.inc"
#undef State
};

State_HotLoad* state_hotload_callbacks[State_Count] = {
#define State(name) [State_##name] = name##_HotLoad,
#include "states.inc"
#undef State
};

State_HotUnload* state_hotunload_callbacks[State_Count] = {
#define State(name) [State_##name] = name##_HotUnload,
#include "states.inc"
#undef State
};

State_Draw* state_draw_callbacks[State_Count] = {
#define State(name) [State_##name] = name##_Draw,
#include "states.inc"
#undef State
};

// NOTE(geni): Defined in matching header
#undef Callbacks

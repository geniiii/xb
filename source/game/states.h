enum {
    State_Null,
#define State(name) State_##name,
#include "states.inc"
#undef State
    State_Count,
};
typedef u32 State;

// clang-format off
#define Callbacks \
Callback(Update) \
Callback(Tick) \
Callback(HotLoad) \
Callback(HotUnload) \
Callback(Draw)
// clang-format on

#define CallbackName
#define Callback(name) typedef Xtal_Application##name##Callback State_##name;
Callbacks
#undef Callback

extern State_Update*    state_update_callbacks[];
extern State_Tick*      state_tick_callbacks[];
extern State_HotLoad*   state_hotload_callbacks[];
extern State_HotUnload* state_hotunload_callbacks[];
extern State_Draw*      state_draw_callbacks[];

typedef struct {
    f32          value;
    b32          pressed;
    b32          down;
    b32          released;
    KeyModifiers modifiers;
    Key          key;
} Input;

enum {
#define Input(id, key)               Input_##id,
#define InputModifiers(id, key, mod) Input(id, key)
#include "inputs.inc"
#undef Input
#undef InputModifiers
    Input_Count,
};
typedef u32 InputKind;

typedef struct {
    b32 left_held;
    b32 left_pressed;
    b32 right_held;
    b32 right_pressed;
} Mouse;

global Mouse mouse;

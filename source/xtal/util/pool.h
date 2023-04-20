#define XTAL_INVALID_HANDLE_ID 0

typedef struct {
    u32* free_slots;
    u32* generations;
    u32  free_slots_top;
    u32  size;
} Xtal_Pool;

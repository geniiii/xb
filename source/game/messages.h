#define MAX_MESSAGES     128
#define MAX_MESSAGE_SIZE 256

typedef struct {
    b8 active;
    struct {
        u8  data[MAX_MESSAGE_SIZE];
        u64 size;
    };
    f32   age;
    f32   max_age;
    Color color;
} Message;

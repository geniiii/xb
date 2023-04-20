typedef struct {
    struct nk_buffer            cmds;
    struct nk_draw_null_texture null;
    XGL_VBO                     vbo;
    XGL_VAO                     vao;
    XGL_VBO                     ebo;
    XGL_Tex                     font;
    GLsync                      fence;
} NK_Device;

typedef struct {
    f32 position[2];
    f32 uv[2];
    u8  color[4];
} NK_Vertex;

typedef struct {
    Xtal_MArena          arena;
    Xtal_MArena          atlas_arena;
    u64                  ctx_marker;
    struct nk_font_atlas atlas;
    NK_Device            device;
} NK_Context;

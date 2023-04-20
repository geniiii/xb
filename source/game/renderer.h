typedef struct {
    Xtal_MArena arena;

    XGL_Shader shaders[XGL_SType_Count];

    XGL_VAO quad_vao;
    XGL_VAO blank_vao;
    XGL_VBO quad_vbo;

    XGL_Tex gb_fb;
    XGL_Tex gb_vram;
    
    mat4s ortho_projection;
    mat4s camera_matrix;
} Renderer;

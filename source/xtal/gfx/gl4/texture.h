typedef enum {
    XGL_TexFormat_R8B8G8A8 = GL_RGBA8,
    XGL_TexFormat_Depth    = GL_DEPTH_COMPONENT16,
} XGL_TexFormat;
typedef enum {
    XGL_TexGPUFormat_RGBA  = GL_RGBA,
    XGL_TexGPUFormat_Depth = GL_DEPTH_COMPONENT,
} XGL_TexGPUFormat;
typedef enum {
    XGL_TexFiltering_Nearest = GL_NEAREST,
    XGL_TexFiltering_Linear  = GL_LINEAR,
} XGL_TexFiltering;
typedef enum {
    XGL_TexWrap_ClampToEdge = GL_CLAMP_TO_EDGE,
    XGL_TexWrap_Repeat      = GL_REPEAT,
} XGL_TexWrap;

typedef struct {
    ivec2s           size;
    u32              handle;
    XGL_TexFormat    format;
    XGL_TexGPUFormat gpu_format;
} XGL_Tex;
typedef struct {
    ivec2s           size;
    u32              handle;
    XGL_TexFormat    format;
    XGL_TexGPUFormat gpu_format;
    u32              max_layers;
    u32              layers;
} XGL_TexArray;

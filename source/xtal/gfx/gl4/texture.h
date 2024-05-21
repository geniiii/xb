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

internal void         XGL_TexParamI32(XGL_Tex tex, GLenum param, i32 value);
internal void         XGL_TexParamF32(XGL_Tex tex, GLenum param, f32 value);
internal XGL_Tex      XGL_TexNew(ivec2s size, XGL_TexFormat format, XGL_TexGPUFormat gpu_format, XGL_TexFiltering filtering, XGL_TexWrap wrap);
internal XGL_Tex      XGL_TexNewWithData(ivec2s size, XGL_TexFormat format, XGL_TexGPUFormat gpu_format, XGL_TexFiltering filtering, XGL_TexWrap wrap, const void* data);
internal void         XGL_TexUpdate(XGL_Tex tex, const void* data);
internal void         XGL_TexDestroy(XGL_Tex tex);
internal void         XGL_TexBindToUnit(XGL_Tex tex, GLuint unit);
internal XGL_TexArray XGL_TexArrayNew(ivec2s size, XGL_TexFormat format, XGL_TexGPUFormat gpu_format, XGL_TexFiltering filtering, XGL_TexWrap wrap, u32 layer_count);
internal void         XGL_TexArrayUpdate(XGL_TexArray* tex, u32 layer, const void* data);
internal void         XGL_TexArrayBindToUnit(XGL_TexArray* tex, GLuint unit);
internal void         XGL_TexArrayDestroy(XGL_TexArray tex);

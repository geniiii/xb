#include "../../generated/shaders.c"

typedef struct {
    XGL_SType       type;
    GLuint          handle;
    XGL_ShaderInfo* info;
} XGL_Shader;

internal b32        XGL_ShaderCompileFromData(String8 data, GLenum type, GLuint* handle_ptr);
internal XGL_Shader XGL_ShaderNew(XGL_SType type);
internal void       XGL_ShaderBind(XGL_Shader shader);
internal void       XGL_ShaderUnbind(void);
internal void       XGL_ShaderDestroy(XGL_Shader shader);
internal void       XGL_ShaderUniformF32(XGL_Shader shader, i32 id, f32 f);
internal void       XGL_ShaderUniformI32(XGL_Shader shader, i32 id, i32 i);
internal void       XGL_ShaderUniformU32(XGL_Shader shader, i32 id, u32 i);
internal void       XGL_ShaderUniformVec2(XGL_Shader shader, i32 id, vec2s vec);
internal void       XGL_ShaderUniformVec3(XGL_Shader shader, i32 id, vec3s vec);
internal void       XGL_ShaderUniformVec4(XGL_Shader shader, i32 id, vec4s vec);
internal void       XGL_ShaderUniformVec2i(XGL_Shader shader, i32 id, ivec2s vec);
internal void       XGL_ShaderUniformVec3i(XGL_Shader shader, i32 id, ivec3s vec);
internal void       XGL_ShaderUniformVec4i(XGL_Shader shader, i32 id, ivec4s vec);
internal void       XGL_ShaderUniformMat4(XGL_Shader shader, i32 id, mat4s* mat);

typedef struct XGL_Sprite {
    vec3s pos;
    vec4s tint;
    // XGL_Tex tex;
} XGL_Sprite;

#define XGL_Sprite(...) \
    (XGL_Sprite) { __VA_ARGS__ }

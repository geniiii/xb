#define ColorRGBA(r, g, b, a) ((u32) ((r) | (g) << 8 | (b) << 16 | (a) << 24))
#define Color(r, g, b)        ColorRGBA(r, g, b, XTAL_U8_MAX)
#define ColorR(rgba)          (rgba & 0xFF)
#define ColorG(rgba)          ((rgba >> 8) & 0xFF)
#define ColorB(rgba)          ((rgba >> 16) & 0xFF)
#define ColorA(rgba)          ((rgba >> 24) & 0xFF)

typedef u32 Color;

// TODO(geni): SIMD
internal inline vec4s ColorToVec4(Color color) {
    return (vec4s){
        .r = (f32) ColorR(color) / XTAL_U8_MAX,
        .g = (f32) ColorG(color) / XTAL_U8_MAX,
        .b = (f32) ColorB(color) / XTAL_U8_MAX,
        .a = (f32) ColorA(color) / XTAL_U8_MAX,
    };
}

// TODO(geni): SIMD
internal inline Color Vec4ToColor(vec4s color) {
    return ColorRGBA((u32) (color.r * XTAL_U8_MAX), (u32) (color.g * XTAL_U8_MAX), (u32) (color.b * XTAL_U8_MAX), (u32) (color.a * XTAL_U8_MAX));
}

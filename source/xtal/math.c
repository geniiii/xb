#if defined(_MSC_VER)
#include <intrin.h>
#elif
#ifdef __has_builtin
#if __has_builtin(__builtin_clzll)
#define XTAL_HAS_BUILTIN_CLZ 1
#endif
#elif defined(__GNUC__)
#if (__GNUC__ > 3)
#define XTAL_HAS_BUILTIN_CLZ 1
#elif defined(__GNUC_MINOR__)
#if (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define XTAL_HAS_BUILTIN_CLZ 1
#endif
#endif
#endif
#endif

#define FMod                              fmodf
#define F64Mod                            fmod
#define FAbs                              fabsf
#define F64Abs                            fabs
#define Abs                               abs

#define SquareRoot                        sqrtf

#define PI                                (3.1415926535897f)
#define Sin                               sinf
#define Cos                               cosf
#define Tan                               tanf
#define ATan                              atanf

#define Floor                             floorf
#define Ceil                              ceilf
#define Sign(x)                           ((i32) ((((i32) 0) < (x)) - ((x) < ((i32) 0))))

#define Min(a, b)                         ((a) > (b) ? (b) : (a))
#define Max(a, b)                         ((a) < (b) ? (b) : (a))
#define Clamp(x, min, max)                Max(min, Min((x), max))

#define GetBit(x, bit)                    (((x) >> bit) & 1)
#define SetBit(x, bit)                    (x) |= (1 << bit)
#define ClearBit(x, bit)                  (x) &= ~(1 << bit)
#define ToggleBitConditional(x, bit, ...) (x) = (((x) & ~(1 << (bit))) | (-!!(__VA_ARGS__) & (1 << (bit))))

#define XTAL_I8_MIN                       (-127)
#define XTAL_I8_MAX                       127
#define XTAL_U8_MIN                       0u
#define XTAL_U8_MAX                       255u
#define XTAL_I16_MIN                      (-32767)
#define XTAL_I16_MAX                      32767
#define XTAL_U16_MIN                      0u
#define XTAL_U16_MAX                      65535u
#define XTAL_I32_MIN                      (-2147483647)
#define XTAL_I32_MAX                      2147483647
#define XTAL_U32_MIN                      0u
#define XTAL_U32_MAX                      4294967295u
#define XTAL_F32_MAX                      (3.402823e+38f)

#define XTAL_F32_EPSILON                  (1.19209e-07f)
#define XTAL_F64_EPSILON                  (2.22045e-16)

// TODO(geni): Check if negation works here
#define _XtalGeneric_RotL(type, type_name, size)                    \
    internal force_inline type Xtal_RotL##type_name(type x, u8 n) { \
        return (x << n) | (x >> (-n & (size - 1)));                 \
    }

#define _XtalAMD64_RotL(type, type_name, size)                      \
    internal force_inline type Xtal_RotL##type_name(type x, u8 n) { \
        return _rotl##size(x, n);                                   \
    }

#if defined(_MSC_VER) && !defined(__clang__)
// NOTE(geni): Fuck you, Microsoft
#define _rotl32    _rotl
#define _Xtal_RotL _XtalAMD64_RotL
#else
#define _Xtal_RotL _XtalGeneric_RotL
#endif

// clang-format off
_Xtal_RotL(u8, U8, 8)
_Xtal_RotL(u16, U16, 16)
_Xtal_RotL(u32, U32, 32)
_Xtal_RotL(u64, U64, 64)
#if defined(_MSC_VER) && !defined(__clang__)
#undef _rotl32
#endif
    // clang-format on

    internal force_inline u64 Xtal_NearestPowerOf2U64(u64 x) {
#ifdef _MSC_VER
    unsigned long index;
    _BitScanReverse64(&index, x - 1);
    return 1ull << (index + 1);
#elif XTAL_HAS_BUILTIN_CLZ
    // TODO(geni): Untested
    return x == 1 ? 2 : 1ull << (64 - __builtin_clzll(x - 1));
#else
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    x++;
#endif
}

internal force_inline u32 Xtal_BitScanForwardU32(u32 x) {
#ifdef _MSC_VER
    unsigned long index;
    _BitScanForward(&index, x);
    return index;
#elif XTAL_HAS_BUILTIN_CLZ
    // TODO(geni): Untested
    return x == 1 ? 1 : __builtin_clzl(x);
#else
    // NOTE(geni): de Bruijn trick that I stole off somewhere
    local_persist const u8 pos[] = {0, 1, 28, 2, 29, 14, 24, 3,
                                    30, 22, 20, 15, 25, 17, 4, 8, 31, 27, 13, 23, 21, 19,
                                    16, 7, 26, 12, 18, 6, 11, 5, 10, 9};
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x = (x >> 1) + 1;
    return pos[(v * 0x077CB531) >> 27];
#endif
}

internal inline b32
Xtal_F32Equal(f32 a, f32 b) {
    return FAbs(a - b) <= XTAL_F32_EPSILON * Max(Max(1.0f, FAbs(a)), FAbs(b));
}

internal inline b32 Xtal_F64Equal(f64 a, f64 b) {
    return F64Abs(a - b) <= XTAL_F64_EPSILON * Max(Max(1.0f, F64Abs(a)), F64Abs(b));
}

internal inline b32 Xtal_IVec2Equal(ivec2s a, ivec2s b) {
    return a.x == b.x && a.y == b.y;
}
internal inline vec2s Xtal_IVec2ToVec2(ivec2s v) {
    return (vec2s){(f32) v.x, (f32) v.y};
}
internal inline ivec2s Xtal_Vec2ToIVec2(vec2s v) {
    return (ivec2s){(u32) v.x, (u32) v.y};
}

internal inline vec2s Xtal_Vec2MaintainAspectRatio(vec2s o, vec2s t) {
    f32 ro = o.width / o.height;
    f32 rt = t.width / t.height;
    return ro > rt ? (vec2s){t.width * o.height / t.height, o.height} : (vec2s){o.width, t.height * o.width / t.width};
}

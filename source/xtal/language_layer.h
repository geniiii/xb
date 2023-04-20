//~ Helper macros

#ifndef Xtal_MemoryCopy
#define Xtal_MemoryCopy memcpy
#endif

#ifndef Xtal_MemoryMove
#define Xtal_MemoryMove memmove
#endif

#ifndef Xtal_MemorySet
#define Xtal_MemorySet memset
#endif

#define global        static
#define internal      static
#define local_persist static
#if defined(_MSC_VER) && !defined(__clang__)
#define force_inline __forceinline
#elif defined(__clang__) || defined(__GNUC__)
#define force_inline inline __attribute__((always_inline))
#else
#define force_inline inline
#endif

#if XTAL_BUILD_WIN32
#pragma section(".roglob", read)
#define read_only __declspec(allocate(".roglob"))
#else
#define read_only
#endif

#define ArrayCount(a)   (sizeof(a) / sizeof((a)[0]))
#define Kilobytes(n)    (n << 10)
#define Megabytes(n)    (n << 20)
#define Gigabytes(n)    (((u64) n) << 30)
#define Unreferenced(a) ((void) (a))

#if XTAL_BUILD_WIN32
#define thread_local __declspec(thread)
#else
#define thread_local __thread
#endif

//~ Base types

typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef i8        b8;
typedef i16       b16;
typedef i32       b32;
typedef i64       b64;
typedef float     f32;
typedef double    f64;
typedef uintptr_t uptr;

typedef wchar_t wchar;

//~ Assertions

#if XTAL_DEBUG
#define _Assert(b, crash)                                                \
    do {                                                                 \
        if (!(b)) {                                                      \
            _AssertFailure(S8Lit(#b), __LINE__, S8Lit(__FILE__), crash); \
        }                                                                \
    } while (0)
#else
#define _Assert(...)
#endif
// TODO(geni): Kind of a bad name
#define HardAssert(b) _Assert(b, 1)
#define SoftAssert(b) _Assert(b, 0)
#define StaticAssert  _Static_assert
#undef Assert
#define Assert HardAssert

internal inline void BreakDebugger(void) {
#if _MSC_VER && XTAL_BUILD_WIN32
    __debugbreak();
#else
    // TODO(geni): BreakDebugger for Linux/macOS
#endif
}

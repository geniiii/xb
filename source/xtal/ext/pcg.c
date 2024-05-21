// NOTE(geni): "unary minus operator applied to unsigned type, result still unsigned"
#pragma warning(push)
#pragma warning(disable : 4146)
#define PCG_HAS_128BIT_OPS 0
#if __clang__ && (__x86_64__ || __i386__)
#define PCG_USE_INLINE_ASM 1
#endif
#include "pcg/include/pcg_variants.h"
#pragma warning(pop)

internal force_inline u64 Xtal_Rand_HashU64(u64 h);
internal pcg32_random_t   Xtal_RandNew(u32 seed);
internal u32              Xtal_Rand_NextU32(pcg32_random_t* state);
internal u32              Xtal_Rand_NextU32Range(pcg32_random_t* state, u32 low, u32 high);
internal f32              Xtal_Rand_NextNormF32(pcg32_random_t* state);
internal f32              Xtal_Rand_NextF32(pcg32_random_t* state, f32 low, f32 high);

#define Xtal_Rand_ShuffleArray(array, array_size, type, state) \
    do {                                                       \
        for (i32 i = array_size - 1; i > 0; i--) {             \
            u32  j    = Xtal_Rand_NextU32Range(state, 0, i);   \
            type temp = array[j];                              \
            array[j]  = array[i];                              \
            array[i]  = temp;                                  \
        }                                                      \
    } while (0)

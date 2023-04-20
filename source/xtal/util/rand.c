internal inline u64 Xtal_Rand_HashU64(u64 h) {
    h = (h ^ (h >> 30)) * 0xBF58476D1CE4E5B9ull;
    h = (h ^ (h >> 27)) * 0x94D049BB133111EBull;
    h = h ^ (h >> 31);
    return h;
}

internal pcg32_random_t Xtal_RandNew(u32 seed) {
    pcg32_random_t result;
    pcg32_srandom_r(&result, seed, Xtal_Rand_HashU64(seed) & 0xFFFFFFFF);
    return result;
}

internal u32 Xtal_Rand_NextU32(pcg32_random_t* state) {
    return pcg32_random_r(state);
}
internal u32 Xtal_Rand_NextU32Range(pcg32_random_t* state, u32 low, u32 high) {
    return low + pcg32_boundedrand_r(state, high - low);
}
internal f32 Xtal_Rand_NextNormF32(pcg32_random_t* state) {
    return (f32) ((Xtal_Rand_NextU32(state) >> 8) * 0x1.0p-24);
}
internal f32 Xtal_Rand_NextF32(pcg32_random_t* state, f32 low, f32 high) {
    return low + (high - low) * Xtal_Rand_NextNormF32(state);
}

#define Xtal_Rand_ShuffleArray(array, array_size, type, state) \
    do {                                                       \
        for (i32 i = array_size - 1; i > 0; i--) {             \
            u32  j    = Xtal_Rand_NextU32Range(state, 0, i);   \
            type temp = array[j];                              \
            array[j]  = array[i];                              \
            array[i]  = temp;                                  \
        }                                                      \
    } while (0)

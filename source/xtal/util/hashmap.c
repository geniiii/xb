// TODO(geni): Try using arena arrays and https://gist.github.com/pervognsen/2470bd6e992d83bb59de11ddcd30895f for growing
// TODO(geni): HashMapGet may be broken
// NOTE(geni): This really makes me wish I was using Odin instead

#define _Xtal_HashMapOccupied(x)                               (x != 0)
#define Xtal_HashMapDefine(key_type, value_type, compare_func) Xtal_HashMapDefineNamed(key_type, value_type, Xtal_HashMap_##key_type##_##value_type, compare_func)
#define Xtal_HashMapDefineNamed(key_type, value_type, type_name, compare_func)                                    \
    typedef struct {                                                                                              \
        key_type   key;                                                                                           \
        u64        hash;                                                                                          \
        value_type value;                                                                                         \
        u32        psl;                                                                                           \
    } type_name##_Bucket;                                                                                         \
    typedef struct {                                                                                              \
        Xtal_MArena         arena;                                                                                \
        Xtal_MArena         arena_grow;                                                                           \
        type_name##_Bucket* buckets;                                                                              \
        u64                 count;                                                                                \
        u64                 capacity;                                                                             \
    } type_name;                                                                                                  \
    internal type_name type_name##_New(u64 initial_capacity) {                                                    \
        type_name result = {                                                                                      \
            .capacity   = Xtal_NearestPowerOf2U64(initial_capacity),                                              \
            .arena      = Xtal_MArenaNew(),                                                                       \
            .arena_grow = Xtal_MArenaNew(),                                                                       \
        };                                                                                                        \
        result.buckets = Xtal_MArenaPushTypeZeroN(&result.arena, type_name##_Bucket, result.capacity);            \
        return result;                                                                                            \
    }                                                                                                             \
    internal b32 type_name##_Get(type_name* hm, key_type key, u64 hash, value_type** out) {                       \
        u64 bucket_index = hash & hm->capacity - 1;                                                               \
        if (!_Xtal_HashMapOccupied(hm->buckets[bucket_index].hash)) {                                             \
            return 0;                                                                                             \
        }                                                                                                         \
        u32 psl = 0;                                                                                              \
        while ((_Xtal_HashMapOccupied(hm->buckets[bucket_index].hash) && hm->buckets[bucket_index].psl > psl) ||  \
               hm->buckets[bucket_index].hash != hash) {                                                          \
            ++bucket_index;                                                                                       \
            if (bucket_index >= hm->capacity) {                                                                   \
                return 0;                                                                                         \
            }                                                                                                     \
            bucket_index &= hm->capacity - 1;                                                                     \
            ++psl;                                                                                                \
        }                                                                                                         \
        if (compare_func(&hm->buckets[bucket_index].key, key, hash)) {                                            \
            if (out) {                                                                                            \
                *out = &hm->buckets[bucket_index].value;                                                          \
            }                                                                                                     \
            return 1;                                                                                             \
        }                                                                                                         \
        return 0;                                                                                                 \
    }                                                                                                             \
    internal u64 _##type_name##_InsertNoGrow(type_name* hm, key_type key, u64 hash, value_type value) {           \
        type_name##_Bucket new = {                                                                                \
            .key   = key,                                                                                         \
            .hash  = hash,                                                                                        \
            .value = value,                                                                                       \
        };                                                                                                        \
        u64 bucket_index = hash & hm->capacity - 1;                                                               \
        u32 psl          = 0;                                                                                     \
        while (_Xtal_HashMapOccupied(hm->buckets[bucket_index].hash)) {                                           \
            if (psl > hm->buckets[bucket_index].psl) {                                                            \
                new.psl                   = psl;                                                                  \
                type_name##_Bucket temp   = hm->buckets[bucket_index];                                            \
                hm->buckets[bucket_index] = new;                                                                  \
                new                       = temp;                                                                 \
                psl                       = new.psl;                                                              \
            }                                                                                                     \
            bucket_index = (bucket_index + 1) & hm->capacity - 1;                                                 \
            ++psl;                                                                                                \
        }                                                                                                         \
        type_name##_Bucket* bucket = &hm->buckets[bucket_index];                                                  \
        new.psl                    = psl;                                                                         \
        *bucket                    = new;                                                                         \
        ++hm->count;                                                                                              \
        return bucket_index;                                                                                      \
    }                                                                                                             \
    internal void type_name##_Grow(type_name* hm, u64 new_capacity) {                                             \
        type_name new = {                                                                                         \
            .capacity   = new_capacity,                                                                           \
            .arena      = hm->arena_grow,                                                                         \
            .arena_grow = hm->arena,                                                                              \
        };                                                                                                        \
        new.buckets = Xtal_MArenaPushTypeZeroN(&new.arena, type_name##_Bucket, new.capacity);                     \
        for (u32 i = 0; i < hm->capacity; ++i) {                                                                  \
            if (_Xtal_HashMapOccupied(hm->buckets[i].hash)) {                                                     \
                _##type_name##_InsertNoGrow(&new, hm->buckets[i].key, hm->buckets[i].hash, hm->buckets[i].value); \
            }                                                                                                     \
        }                                                                                                         \
        Xtal_MArenaClear(&hm->arena);                                                                             \
        *hm = new;                                                                                                \
    }                                                                                                             \
    internal u64 type_name##_Insert(type_name* hm, key_type key, u64 hash, value_type value) {                    \
        if (2 * hm->count >= hm->capacity) {                                                                      \
            type_name##_Grow(hm, 2 * hm->capacity);                                                               \
        }                                                                                                         \
        return _##type_name##_InsertNoGrow(hm, key, hash, value);                                                 \
    }                                                                                                             \
    internal void type_name##_Destroy(type_name* hm) {                                                            \
        Xtal_MArenaRelease(hm->arena);                                                                            \
        Xtal_MArenaRelease(hm->arena_grow);                                                                       \
    }

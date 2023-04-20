#define Xtal_PriorityQueueDefine(type, name)                                                                               \
    typedef i32 (*Xtal_PriorityQueueCompareType##name)(type, type);                                                        \
    typedef struct {                                                                                                       \
	Xtal_PriorityQueueCompareType##name Compare;                                                                       \
	type*                               array;                                                                         \
    } Xtal_PriorityQueue##name;                                                                                            \
    internal inline void _Xtal_PriorityQueue##name##Swap(Xtal_PriorityQueue##name* queue, u32 a, u32 b) {                  \
	type tmp        = queue->array[a];                                                                                 \
	queue->array[a] = queue->array[b];                                                                                 \
	queue->array[b] = tmp;                                                                                             \
    }                                                                                                                      \
    internal void _Xtal_PriorityQueue##name##FixUp(Xtal_PriorityQueue##name* queue, u32 p) {                               \
	while (p > 1 && queue->Compare(queue->array[p], queue->array[p / 2]) < 0) {                                        \
	    _Xtal_PriorityQueue##name##Swap(queue, p, p / 2);                                                              \
	    p /= 2;                                                                                                        \
	}                                                                                                                  \
    }                                                                                                                      \
    internal void _Xtal_PriorityQueue##name##FixDown(Xtal_PriorityQueue##name* queue, u32 p) {                             \
	while (p * 2 <= Xtal_ArenaArrayCount(queue->array) - 1) {                                                          \
	    u32 l = p * 2;                                                                                                 \
	    if (l < Xtal_ArenaArrayCount(queue->array) - 1 && queue->Compare(queue->array[l], queue->array[l + 1]) > 0) {  \
		++l;                                                                                                       \
	    }                                                                                                              \
	    if (queue->Compare(queue->array[p], queue->array[l]) < 0) {                                                    \
		break;                                                                                                     \
	    }                                                                                                              \
	    _Xtal_PriorityQueue##name##Swap(queue, p, l);                                                                  \
	    p = l;                                                                                                         \
	}                                                                                                                  \
    }                                                                                                                      \
    internal void Xtal_PriorityQueue##name##Insert(Xtal_PriorityQueue##name* queue, type val) {                            \
	Xtal_ArenaArrayPush(queue->array, val);                                                                            \
	_Xtal_PriorityQueue##name##FixUp(queue, Xtal_ArenaArrayCount(queue->array) - 1);                                   \
    }                                                                                                                      \
    internal void Xtal_PriorityQueue##name##RemoveMin(Xtal_PriorityQueue##name* queue) {                                   \
	_Xtal_PriorityQueue##name##Swap(queue, 1, Xtal_ArenaArrayCount(queue->array) - 1);                                 \
	--Xtal_ArenaArrayCount(queue->array);                                                                              \
	_Xtal_PriorityQueue##name##FixDown(queue, 1);                                                                      \
    }                                                                                                                      \
    internal type Xtal_PriorityQueue##name##Min(Xtal_PriorityQueue##name* queue) {                                         \
	return queue->array[1];                                                                                            \
    }                                                                                                                      \
    internal u32 Xtal_PriorityQueue##name##Count(Xtal_PriorityQueue##name* queue) {                                        \
	return Xtal_ArenaArrayCount(queue->array) - 1;                                                                     \
    }                                                                                                                      \
    internal Xtal_PriorityQueue##name Xtal_PriorityQueue##name##NewN(Xtal_PriorityQueueCompareType##name compare, u32 n) { \
	Xtal_PriorityQueue##name queue = {                                                                                 \
	    .array   = _Xtal_ArenaArrayNew(sizeof(type), n + 1),                                                           \
	    .Compare = compare,                                                                                            \
	};                                                                                                                 \
	++Xtal_ArenaArrayCount(queue.array);                                                                               \
	return queue;                                                                                                      \
    }                                                                                                                      \
    internal Xtal_PriorityQueue##name Xtal_PriorityQueue##name##New(Xtal_PriorityQueueCompareType##name compare) {         \
	return Xtal_PriorityQueue##name##NewN(compare, 1);                                                                 \
    }                                                                                                                      \
    internal void Xtal_PriorityQueue##name##Destroy(Xtal_PriorityQueue##name* queue) {                                     \
	Xtal_ArenaArrayDestroy(queue->array);                                                                              \
    }

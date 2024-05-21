internal S8ListNode* S8ListPush(S8List* list, Xtal_MArena* arena, String8 str) {
    S8ListNode* node = Xtal_MArenaPushStructZero(arena, S8ListNode);
    node->str        = str;
    Xtal_SLPush(list->first, list->last, node);
    return node;
}
internal S8ListNode* S8ListPushFV(S8List* list, Xtal_MArena* arena, const char* fmt, va_list args) {
    va_list args2;
    va_copy(args2, args);

    String8 new      = S8_PushFV(arena, fmt, args2);
    S8ListNode* node = Xtal_MArenaPushStructZero(arena, S8ListNode);
    node->str        = new;
    Xtal_SLPush(list->first, list->last, node);
    return node;
}
internal S8ListNode* S8ListPushF(S8List* list, Xtal_MArena* arena, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    S8ListNode* result = S8ListPushFV(list, arena, fmt, args);
    va_end(args);
    return result;
}

internal String8 S8ListJoin(S8List* list, Xtal_MArena* arena, String8 separator) {
    u64 required_size = 0;
    for (S8ListNode* i = list->first; i != NULL; i = i->next) {
        required_size += i->str.size + separator.size;
    }

    String8 result = S8_Push(arena, required_size);
    u8*     write  = result.str;
    for (S8ListNode* node = list->first; node != NULL; node = node->next) {
        Xtal_MemoryCopy(write, node->str.str, node->str.size);
        write += node->str.size;
        if (separator.size > 0 && node != list->last) {
            Xtal_MemoryCopy(write, separator.str, separator.size);
            write += separator.size;
        }
    }

    return result;
}

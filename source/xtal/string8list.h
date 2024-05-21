typedef struct S8ListNode S8ListNode;
struct S8ListNode {
    String8     str;
    S8ListNode* next;
};

typedef struct {
    S8ListNode* first;
    S8ListNode* last;
} S8List;

internal S8ListNode* S8ListPush(S8List* list, Xtal_MArena* arena, String8 str);
internal S8ListNode* S8ListPushF(S8List* list, Xtal_MArena* arena, const char* fmt, ...);
internal S8ListNode* S8ListPushFV(S8List* list, Xtal_MArena* arena, const char* fmt, va_list args);
internal String8     S8ListJoin(S8List* list, Xtal_MArena* arena, String8 separator);

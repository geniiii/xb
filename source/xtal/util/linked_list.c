#define Xtal_SLPush(first, last, n) (((last) == NULL) ? ((first) = (last) = n) : ((last)->next = (n), (last) = (n), (n)->next = NULL))

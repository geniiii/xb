internal Xtal_Thread _CreateThread(i32 (*thread_proc)(void*), void* user_data, u32 stack_size) {
    return (Xtal_Thread){
        .handle = CreateThread(NULL, stack_size, (LPTHREAD_START_ROUTINE) thread_proc, user_data, 0, NULL),
    };
}
internal void JoinThread(Xtal_Thread* thread) {
    WaitForSingleObject((HANDLE) thread->handle, INFINITE);
}
internal void DetachThread(Xtal_Thread* thread) {
    CloseHandle((HANDLE) thread->handle);
    thread->handle = NULL;
}
internal void DestroyThread(Xtal_Thread* thread) {
    JoinThread(thread);
    CloseHandle((HANDLE) thread->handle);
    thread->handle = NULL;
}


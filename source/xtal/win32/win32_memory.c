internal void* W32_HeapAlloc(u64 size) {
    return HeapAlloc(GetProcessHeap(), 0, size);
}
internal void* W32_HeapRealloc(void* memory, u64 size) {
    return HeapReAlloc(GetProcessHeap(), 0, memory, size);
}
internal void W32_HeapFree(void* memory) {
    HeapFree(GetProcessHeap(), 0, memory);
}

internal void* Reserve(u64 size) {
    return VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
}
internal void Release(void* memory) {
    VirtualFree(memory, 0, MEM_RELEASE);
}
internal void Commit(void* memory, u64 size) {
    VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE);
}
internal void Decommit(void* memory, u64 size) {
    VirtualFree(memory, size, MEM_DECOMMIT);
}

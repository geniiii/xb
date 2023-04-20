// TODO(geni): Creating message boxes for each error may be a bad idea. Perhaps we should let the game handle errors instead?

//~ File operations

internal Xtal_OS_File OpenFileS16(String16 path, Xtal_OS_FilePermission permissions, Xtal_OS_FileOpen open_method) {
    HANDLE file = CreateFileW(path.s, permissions, FILE_SHARE_READ, NULL, open_method, 0, 0);

    if (file == INVALID_HANDLE_VALUE) {
        Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
        OutputErrorMessageS16(L"File I/O Error", L"Could not open file \"%.*s\"; error: %.*s", S16VArg(path), FormatErrorCodeS16(scratch.arena, GetLastError()));
        Xtal_MArenaTempEnd(scratch);
    }

    return (Xtal_OS_File){
        .id = (u64) file,
        // TODO(geni): This function doesn't set its return value's path correctly, which leads to incorrect error messages in subsequent calls to file I/O functions
        .path = S8Lit("SHOOT GENI IF YOU SEE THIS"),
    };
}

internal u64 _GetFileSize(Xtal_OS_File file) {
    LARGE_INTEGER result = {0};
    if (!GetFileSizeEx((HANDLE) file.id, &result)) {
        Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
        String16        path    = S16_PushFromS8(scratch.arena, file.path);
        OutputErrorMessageS16(L"File I/O Error", L"Could not get size of file \"%.*s\"; error: %.*s", S16VArg(path), FormatErrorCodeS16(scratch.arena, GetLastError()));
        Xtal_MArenaTempEnd(scratch);
    }
    return result.LowPart | ((u64) result.HighPart << 32);
}

internal void CloseFile(Xtal_OS_File file) {
    CloseHandle((HANDLE) file.id);
}

internal void WriteToFile(Xtal_OS_File file, void* data, u64 data_size, u64 offset, Xtal_OS_FileSeek write_method) {
    LONG dist_high = (LONG) (offset >> 32);
    if (SetFilePointer((HANDLE) file.id, (LONG) (offset & 0xFFFFFFFF), &dist_high, write_method) == INVALID_SET_FILE_POINTER) {
        // NOTE(geni): Failed to set file pointer
        goto fail;
    }

    DWORD bytes_written = 0;
    if (!WriteFile((HANDLE) file.id, data, (DWORD) data_size, &bytes_written, 0)) {
        // NOTE(geni): ERROR_IO_PENDING is not technically an error
        if (GetLastError() != ERROR_IO_PENDING) {
            // NOTE(geni): Failed to write file
            goto fail;
        }
    }
    if (bytes_written != data_size) {
        goto fail;
    }

    return;

fail:
    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
    String16        path    = S16_PushFromS8(scratch.arena, file.path);
    OutputErrorMessageS16(L"File I/O Error", L"Could not save to \"%.*s\"; error: %.*s", S16VArg(path), FormatErrorCodeS16(scratch.arena, GetLastError()));
    Xtal_MArenaTempEnd(scratch);
}

internal void SaveToFile(Xtal_OS_File file, void* data, u64 data_size) {
    WriteToFile(file, data, data_size, 0, Xtal_OS_FileSeek_FromStart);
}

internal void AppendToFile(Xtal_OS_File file, void* data, u64 data_size) {
    WriteToFile(file, data, data_size, 0, Xtal_OS_FileSeek_FromEnd);
}

internal void ReadFromFileTo(Xtal_OS_File file, void* data, u64 data_size, u64 offset, Xtal_OS_FileSeek read_method) {
    LONG dist_high = (LONG) (offset >> 32);
    if (SetFilePointer((HANDLE) file.id, (LONG) (offset & 0xFFFFFFFF), &dist_high, read_method) == INVALID_SET_FILE_POINTER) {
        // NOTE(geni): Failed to set file pointer
        goto fail;
    }

    DWORD bytes_read = 0;
    if (!ReadFile((HANDLE) file.id, data, (DWORD) data_size, &bytes_read, NULL)) {
        // NOTE(geni): ERROR_IO_PENDING is not technically an error
        if (GetLastError() != ERROR_IO_PENDING) {
            // NOTE(geni): Failed to read file
            goto fail;
        }
    }
    if (data_size != bytes_read) {
        LogWarning("Requested %llu bytes, received %llu bytes instead", data_size, bytes_read);
    }

    return;

fail:
    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
    String16        path    = S16_PushFromS8(scratch.arena, file.path);
    OutputErrorMessageS16(L"File I/O Error", L"Could not read from \"%.*s\"; error: %.*s", S16VArg(path), FormatErrorCodeS16(scratch.arena, GetLastError()));
    Xtal_MArenaTempEnd(scratch);
}

internal void LoadEntireFile(Xtal_OS_File file, Xtal_MArena* arena, void** data, u64* data_size) {
    *data      = 0;
    *data_size = 0;

    u64 file_size;
    if (!GetFileSizeEx((HANDLE) file.id, (LARGE_INTEGER*) &file_size)) {
        // NOTE(geni): Failed to get file size
        goto fail;
    }

    void* read_data = Xtal_MArenaPush(arena, file_size + 1);
    DWORD bytes_read;

    if (!ReadFile((HANDLE) file.id, read_data, (DWORD) file_size, &bytes_read, NULL)) {
        // NOTE(geni): ERROR_IO_PENDING is not technically an error
        if (GetLastError() != ERROR_IO_PENDING) {
            // NOTE(geni): Failed to read file
            Xtal_MArenaPop(arena, file_size + 1);
            goto fail;
        }
    }

    ((u8*) read_data)[file_size] = '\0';

    *data      = read_data;
    *data_size = bytes_read;

    return;

fail:
    Xtal_MArenaTemp scratch = Xtal_GetScratch(arena, 1);
    String16        path    = S16_PushFromS8(scratch.arena, file.path);
    OutputErrorMessageS16(L"File I/O Error", L"Could not read from \"%.*s\"; error: %.*s", S16VArg(path), FormatErrorCodeS16(scratch.arena, GetLastError()));
    Xtal_MArenaTempEnd(scratch);
}

internal void DeleteFileS16(String16 path) {
    if (!DeleteFileW(path.s)) {
        Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
        OutputErrorMessageS16(L"File I/O Error", L"Could not remove file \"%.*s\"; error: %.*s", S16VArg(path), FormatErrorCodeS16(scratch.arena, GetLastError()));
        Xtal_MArenaTempEnd(scratch);
    }
}

internal void DeleteDirectoryS16(String16 path) {
    // TODO(geni): Not thread safe with relative paths, so I guess we just append the current working directory if it's a relative path..

    SHFILEOPSTRUCT operation = {
        .wFunc  = FO_DELETE,
        .pFrom  = path.wstr,
        .fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
    };

    int result = SHFileOperationW(&operation);
    if (result) {
        Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
        // TODO(geni): Apparently not all error codes returned by this map to the Win32 ones, go figure..
        OutputErrorMessageS16(L"File I/O Error", L"Could not remove directory \"%.*s\"; error: %.*s", S16VArg(path), FormatErrorCodeS16(scratch.arena, result));
        Xtal_MArenaTempEnd(scratch);
    }
}

internal b32 FileExistsS16(String16 path) {
    return GetFileAttributesW(path.s) != INVALID_FILE_ATTRIBUTES;
}

internal b32 CopyFileS16(String16 source, String16 dest) {
    return CopyFileW(source.s, dest.s, 0);
}

internal FILETIME GetLastWriteTime(String16 filename) {
    WIN32_FILE_ATTRIBUTE_DATA data;
    return GetFileAttributesExW(filename.s, GetFileExInfoStandard, &data) != 0 ? data.ftLastWriteTime : (FILETIME){0};
}

internal u64 GetLastWriteTimeUnix(String16 filename) {
    return FileTimeToTimestamp(GetLastWriteTime(filename));
}

//~ Directory operations

internal b32 MakeDirectoryS16(String16 path) {
    return CreateDirectoryW(path.s, 0);
}

internal b32 IsDirectoryS16(String16 path) {
    // NOTE(geni): According to ReactOS, this should also check if the directory exists in the first place
    return PathIsDirectoryW(path.s);
}
internal b32 DirectoryExistsS16(String16 path) {
    return IsDirectoryS16(path);
}

typedef struct DirectoryPathNode DirectoryPathNode;
struct DirectoryPathNode {
    DirectoryPathNode* next;
    String16           path;
};

typedef struct {
    DirectoryPathNode* first;
    DirectoryPathNode* last;
} DirectoryPathList;

typedef struct {
    String16                  path;
    Xtal_OSDirectoryItem      item;
    DirectoryPathList         recurse_list;
    Xtal_OSDirectoryIterFlags flags;
    HANDLE                    file;
    Xtal_MArena*              arena;
} DirectoryIterState;

// TODO(geni): Directory iteration results in a significant number of (fairly inexpensive) allocations, maybe we should cut down on them a little?

internal Xtal_OSDirectoryItem* _BeginIterDirectory(DirectoryIterState* state, String16 path) {
    if (!IsDirectoryS16(path)) {
        OutputErrorMessageS16(L"File I/O Error", L"Failed to list directory \"%.*s\" as it's not a directory", S16VArg(path));
        return NULL;
    }

    String16 new_path;
    if (path.s[path.size - 1] == L'\\' || path.s[path.size - 1] == L'/') {
        // NOTE(geni): We don't use PathCchCombineEx or PathCombineW because they're literally useless;
        //             PathCchCombineEx adds a trailing backslash even when there's already a forward slash because it's fucking stupid, and PathCombineW doesn't support UNC/NT/whatever paths
        new_path = (String16){
            .s    = Xtal_MArenaPushTypeN(state->arena, u16, path.size + 1 + 1),
            .size = path.size + 1,
        };
        Xtal_MemoryCopy(new_path.s, path.s, path.size * sizeof(u16));
        new_path.s[new_path.size - 1] = L'*';
        new_path.s[new_path.size]     = L'\0';
    } else {
        // TODO(geni): Handle
        return NULL;
    }

    WIN32_FIND_DATAW find_data;
    state->file = FindFirstFileW(new_path.s, &find_data);
    if (state->file == INVALID_HANDLE_VALUE) {
        OutputErrorMessage("File I/O Error", "FindFirstFile returned INVALID_HANDLE_VALUE?!");
    }
    state->path = S16_PushCopy(state->arena, path);
    return &state->item;
}

internal Xtal_OSDirectoryItem* BeginIterDirectoryS16(Xtal_MArena* arena, String16 path, Xtal_OSDirectoryIterFlags flags) {
    DirectoryIterState* state = Xtal_MArenaPushZero(arena, sizeof(*state));
    state->flags              = flags;
    state->arena              = arena;
    return _BeginIterDirectory(state, path);
}

internal b32 IterDirectory(Xtal_OSDirectoryItem* item) {
    // TODO(geni): This stupid function doesn't create properly qualified paths for regular files according to me from May 2022
    // TODO(geni): Use NtQueryDirectoryFile instead of FindFirstFile/FindNextFile (maybe)

    // NOTE(geni): Dear god
    DirectoryIterState* state = (DirectoryIterState*) (((u8*) item) - offsetof(DirectoryIterState, item));

    if (item == NULL || state->file == INVALID_HANDLE_VALUE) {
        // NOTE(geni): Early return if BeginIterDirectory failed
        return 0;
    }

    b32 recursive    = state->flags & Xtal_OSDirectoryIterFlag_Recursive;
    b32 include_dirs = state->flags & Xtal_OSDirectoryIterFlag_IncludeDirectories;

    WIN32_FIND_DATAW find_data = {0};
    // NOTE(geni): Here be dragons
    while (1) {
        if (!FindNextFileW(state->file, &find_data)) {
            FindClose(state->file);
            if (recursive && state->recurse_list.first != NULL) {
                // NOTE(geni): Recurse if necessary
                // TODO(geni): Handle case where path isn't a valid directory correctly
                _BeginIterDirectory(state, state->recurse_list.first->path);
                state->recurse_list.first = state->recurse_list.first->next;
                continue;
            }
            return 0;
        }

        // NOTE(geni): Skips "." and ".."
        if (find_data.cFileName[0] == L'.' &&
            (find_data.cFileName[1] == L'\0' || (find_data.cFileName[1] == L'.' && find_data.cFileName[2] == L'\0'))) {
            continue;
        }

        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && (recursive || include_dirs)) {
            u32 filename_len = CalculateWCStringLength(find_data.cFileName);

            String16 path = {
                .s    = Xtal_MArenaPushTypeN(state->arena, u16, state->path.size + filename_len + 1 + 1),
                .size = state->path.size + filename_len + 1,
            };
            Xtal_MemoryCopy(path.s, state->path.s, state->path.size * sizeof(u16));
            Xtal_MemoryCopy(path.s + state->path.size, find_data.cFileName, filename_len * sizeof(u16));
            path.s[path.size - 1] = L'/';
            path.s[path.size]     = L'\0';

            // TODO(geni): Prepend subdirectory name to item name
            if (recursive) {
                DirectoryPathNode* node = Xtal_MArenaPushStruct(state->arena, DirectoryPathNode);
                node->path              = path;
                Xtal_SLPush(state->recurse_list.first, state->recurse_list.last, node);
            }
            if (include_dirs) {
                item->path      = S8_PushFromS16(state->arena, path);
                item->name      = S8_PushFromS16(state->arena, (String16){.s = find_data.cFileName, .size = filename_len});
                item->extension = S8_SubstrAfterLastOccurrenceOf(item->name, '.');
                item->flags     = Xtal_OSDirectoryItemFlag_IsDirectory;
                return 1;
            }

            continue;
        }

        break;
    }

    item->name      = S8_PushFromS16(state->arena, S16_FromWString(find_data.cFileName));
    item->extension = S8_SubstrAfterLastOccurrenceOf(item->name, '.');
    item->flags     = 0;

    return 1;
}

//~ Memory-mapped files

internal b32 MemoryMapCreateFile(String16 path, Xtal_OS_MMapFlags open_method, Xtal_OS_MMapFlags permissions, HANDLE* file_out) {
    DWORD desired_access = GENERIC_READ;
    if (permissions & Xtal_OS_MMapFlags_ReadWrite) desired_access |= GENERIC_WRITE;

    u32    result = 1;
    HANDLE file;
    if ((file = CreateFileW(path.s, desired_access, 0, NULL, open_method, 0, 0)) == INVALID_HANDLE_VALUE) {
        Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
        OutputErrorMessageS16(L"File I/O Error", L"Failed to create file \"%.*s\"; error: %.*s", S16VArg(path), FormatErrorCodeS16(scratch.arena, GetLastError()));
        Xtal_MArenaTempEnd(scratch);
        result = 0;
    }

    *file_out = file;
    return result;
}

internal Xtal_OS_MMapHandle MemoryMapFileS16(String16 path, Xtal_OS_MMapFlags open_method, Xtal_OS_MMapFlags permissions, u64 size, void** out, u64* out_size) {
    b32 create = open_method == Xtal_OS_FileOpen_CreateAlways || open_method == Xtal_OS_FileOpen_CreateNew;
    HardAssert(out != NULL);
    HardAssert(open_method != 0);
    HardAssert(permissions != 0);
    HardAssert(create ? permissions != Xtal_OS_MMapFlags_Read : 1);

    Xtal_OS_MMapHandle result = {0};

    // NOTE(geni): Create file
    HANDLE file;
    if (!MemoryMapCreateFile(path, open_method, permissions, &file)) {
        return result;
    }

    u64 file_size = 0;
    if (open_method != Xtal_OS_FileOpen_CreateAlways) {
        if (!GetFileSizeEx(file, (LARGE_INTEGER*) &file_size)) {
            Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
            OutputErrorMessage("File I/O Error", "Failed to get file size; error: %.*s", FormatErrorCodeS16(scratch.arena, GetLastError()));
            Xtal_MArenaTempEnd(scratch);
            CloseHandle(file);
            return result;
        }
        if (file_size == 0) {
            OutputErrorMessage("File I/O Error", "Tried to mmap empty file");
            CloseHandle(file);
            return result;
        }
    }

    // NOTE(geni): Create file mapping object
    HANDLE file_mapping;
    if ((file_mapping = CreateFileMappingW(file, NULL,
                                           permissions & Xtal_OS_MMapFlags_ReadWrite ? PAGE_READWRITE : PAGE_READONLY,
                                           create ? size >> 32 : 0,
                                           create ? size & 0xFFFFFFFFull : 0,
                                           0)) == NULL) {
        Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
        OutputErrorMessage("File I/O Error", "Failed to create file mapping object; error: %.*s", FormatErrorCodeS16(scratch.arena, GetLastError()));
        Xtal_MArenaTempEnd(scratch);
        CloseHandle(file);
        return result;
    }

    void* data;
    // NOTE(geni): Create file view
    {
        DWORD desired_access = FILE_MAP_READ;
        if (permissions & Xtal_OS_MMapFlags_ReadWrite) desired_access |= FILE_MAP_WRITE;

        if ((data = MapViewOfFile(file_mapping, desired_access, 0, 0, 0)) == NULL) {
            Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
            OutputErrorMessageS16(L"File I/O Error", L"Failed to create file view; error: %.*s", FormatErrorCodeS16(scratch.arena, GetLastError()));
            Xtal_MArenaTempEnd(scratch);
            CloseHandle(file);
            CloseHandle(file_mapping);
            return result;
        }
    }

    // NOTE(geni): According to MSDN:
    //             Although an application may close the file handle used to create a file mapping object,
    //             the system holds the corresponding file open until the last view of the file is unmapped.
    //             Files for which the last view has not yet been unmapped are held open with no sharing restrictions.
    // TODO(geni): Review whether this is the behaviour we're looking for
    CloseHandle(file);

    // NOTE(geni): Allocate slot in pool
    u32 id = Xtal_PoolAllocSlot(&global_os.mmap_handles);
    if (id == XTAL_INVALID_HANDLE_ID) {
        OutputErrorMessage("File I/O Error", "Failed to allocate slot for mmapped file");
        CloseHandle(file_mapping);
        UnmapViewOfFile(data);
        return result;
    }
    // NOTE(geni): Set translation
    global_mmap_handle_translations[id] = (MMapHandleTranslation){
        .object = file_mapping,
        .view   = data,
    };

    *out = data;
    if (out_size) {
        *out_size = file_size;
    }
    result = (Xtal_OS_MMapHandle){
        .id         = id,
        .generation = global_os.mmap_handles.generations[id],
    };

    return result;
}

internal void UnmapFile(Xtal_OS_MMapHandle handle) {
    if (handle.generation != global_os.mmap_handles.generations[handle.id] || handle.id == XTAL_INVALID_HANDLE_ID) {
        LogWarning("Tried to unmap invalid handle");
        return;
    }
    Xtal_PoolFreeSlot(&global_os.mmap_handles, handle.id);
    MMapHandleTranslation translation = global_mmap_handle_translations[handle.id];
    if (!CloseHandle(translation.object)) {
        OutputErrorMessage("File I/O Error", "Failed to close file mapping object of mmap id %u", handle.id);
    }
    if (!UnmapViewOfFile(translation.view)) {
        OutputErrorMessage("File I/O Error", "Failed to close file view of mmap id %u", handle.id);
    }
}

//~ S8 wrappers

internal Xtal_OS_File _OpenFile(String8 path, Xtal_OS_FilePermission permissions, Xtal_OS_FileOpen open_method) {
    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
    Xtal_OS_File    result  = OpenFileS16(S16_PushFromS8(scratch.arena, path), permissions, open_method);
    Xtal_MArenaTempEnd(scratch);
    result.path = path;
    return result;
}
internal void DeleteFile(String8 path) {
    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
    DeleteFileS16(S16_PushFromS8(scratch.arena, path));
    Xtal_MArenaTempEnd(scratch);
}
internal void DeleteDirectory(String8 path) {
    Xtal_MArenaTemp scratch  = Xtal_GetScratch(NULL, 0);
    String16        s16_path = S16_PushFromS8(scratch.arena, path);
    // NOTE(geni): Because SHFileOperation is a dumb fucking function and requires doubly null-terminated paths
    Xtal_MArenaPushTypeTemp(u16);
    s16_path.s[s16_path.size + 1] = L'\0';
    DeleteDirectoryS16(s16_path);
    Xtal_MArenaTempEnd(scratch);
}
internal b32 FileExists(String8 path) {
    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
    b32             result  = FileExistsS16(S16_PushFromS8(scratch.arena, path));
    Xtal_MArenaTempEnd(scratch);
    return result;
}
internal b32 CopyFile(String8 source, String8 dest) {
    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
    b32             result  = CopyFileS16(S16_PushFromS8(scratch.arena, source), S16_PushFromS8(scratch.arena, dest));
    Xtal_MArenaTempEnd(scratch);
    return result;
}
internal b32 MakeDirectory(String8 path) {
    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
    b32             result  = MakeDirectoryS16(S16_PushFromS8(scratch.arena, path));
    Xtal_MArenaTempEnd(scratch);
    return result;
}
internal b32 DirectoryExists(String8 path) {
    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
    b32             result  = DirectoryExistsS16(S16_PushFromS8(scratch.arena, path));
    Xtal_MArenaTempEnd(scratch);
    return result;
}
internal Xtal_OSDirectoryItem* BeginIterDirectory(Xtal_MArena* arena, String8 path, Xtal_OSDirectoryIterFlags flags) {
    Xtal_MArenaTemp       scratch = Xtal_GetScratch(arena, 1);
    Xtal_OSDirectoryItem* result  = BeginIterDirectoryS16(arena, S16_PushFromS8(scratch.arena, path), flags);
    Xtal_MArenaTempEnd(scratch);
    return result;
}
internal Xtal_OS_MMapHandle MemoryMapFile(String8 path, Xtal_OS_MMapFlags open_method, Xtal_OS_MMapFlags permissions, u64 size, void** data, u64* data_size) {
    Xtal_MArenaTemp    scratch = Xtal_GetScratch(NULL, 0);
    Xtal_OS_MMapHandle result  = MemoryMapFileS16(S16_PushFromS8(scratch.arena, path), open_method, permissions, size, data, data_size);
    Xtal_MArenaTempEnd(scratch);
    return result;
}

// TODO(geni): Filter by extension(s)

const GUID CLSID_FileOpenDialog = {
    0xDC1C5A9C, 0xE88A, 0x4DDE, {0xA5, 0xA1, 0x60, 0xF8, 0x2A, 0x20, 0xAE, 0xF7}
};
const GUID CLSID_FileSaveDialog = {
    0xC0B4E2F3, 0xBA21, 0x4773, {0x8D, 0xBA, 0x33, 0x5E, 0xC9, 0x46, 0xEB, 0x8B}
};
const GUID IID_IFileOpenDialog = {
    0xD57C7288, 0xD4AD, 0x4768, {0xBE, 0x02, 0x9D, 0x96, 0x95, 0x32, 0xD9, 0x60}
};
const GUID IID_IFileSaveDialog = {
    0x84BCCD23, 0x5FDE, 0x4CDB, {0xAE, 0xA4, 0xAF, 0x64, 0xB8, 0x3D, 0x78, 0xAB}
};

internal Xtal_FilePickerResult CreateFileOpenDialog(Xtal_MArena* arena) {
    Xtal_FilePickerResult file_result = {0};
    HRESULT               result;

    IFileOpenDialog* file_dialog;
    result = COM_CoCreateInstance(&CLSID_FileOpenDialog,
                                  0,
                                  CLSCTX_INPROC_SERVER,
                                  &IID_IFileOpenDialog,
                                  (LPVOID*) (&file_dialog));
    if (result != S_OK) {
        OutputErrorMessage("File Dialog Error", "File dialog creation failed.");
        return file_result;
    }

    DWORD flags;
    file_dialog->lpVtbl->GetOptions(file_dialog, &flags);
    file_dialog->lpVtbl->SetOptions(file_dialog, flags | FOS_FORCEFILESYSTEM);

    result = file_dialog->lpVtbl->Show(file_dialog, 0);
    if (result == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        return file_result;
    } else if (result != S_OK) {
        OutputErrorMessage("File Dialog Error", "Showing file dialog failed.");
        return file_result;
    }

    IShellItem* item_result;
    result = file_dialog->lpVtbl->GetResult(file_dialog, &item_result);
    if (result != S_OK) {
        OutputErrorMessage("File Dialog Error", "Getting opened item failed.");
        return file_result;
    }

    PWSTR file_path = NULL;
    result          = item_result->lpVtbl->GetDisplayName(item_result, SIGDN_FILESYSPATH, &file_path);
    if (result != S_OK) {
        OutputErrorMessage("File Dialog Error", "Getting opened file path failed.");
        return file_result;
    }

    file_result = (Xtal_FilePickerResult){
        .path      = S8_PushFromS16(arena, S16_FromWString(file_path)),
        .succeeded = 1,
    };

    COM_CoTaskMemFree(file_path);
    item_result->lpVtbl->Release(item_result);
    file_dialog->lpVtbl->Release(file_dialog);

    return file_result;
}

internal Xtal_FilePickerResult CreateFileSaveDialog(Xtal_MArena* arena) {
    Xtal_FilePickerResult file_result = {0};
    HRESULT               result;

    IFileSaveDialog* file_dialog;
    result = COM_CoCreateInstance(&CLSID_FileSaveDialog,
                                  0,
                                  CLSCTX_INPROC_SERVER,
                                  &IID_IFileSaveDialog,
                                  (LPVOID*) (&file_dialog));
    if (result != S_OK) {
        OutputErrorMessage("File Dialog Error", "File dialog creation failed.");
        return file_result;
    }

    DWORD flags;
    file_dialog->lpVtbl->GetOptions(file_dialog, &flags);
    file_dialog->lpVtbl->SetOptions(file_dialog, flags | FOS_FORCEFILESYSTEM);

    result = file_dialog->lpVtbl->Show(file_dialog, 0);
    if (result == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        return file_result;
    } else if (result != S_OK) {
        OutputErrorMessage("File Dialog Error", "Showing file dialog failed.");
        return file_result;
    }

    IShellItem* item_result;
    result = file_dialog->lpVtbl->GetResult(file_dialog, &item_result);
    if (result != S_OK) {
        OutputErrorMessage("File Dialog Error", "Getting opened item failed.");
        return file_result;
    }

    PWSTR file_path = NULL;
    result          = item_result->lpVtbl->GetDisplayName(item_result, SIGDN_FILESYSPATH, &file_path);
    if (result != S_OK) {
        OutputErrorMessage("File Dialog Error", "Getting opened file path failed.");
        return file_result;
    }

    file_result = (Xtal_FilePickerResult){
        .path      = S8_PushFromS16(arena, S16_FromWString(file_path)),
        .succeeded = 1,
    };

    COM_CoTaskMemFree(file_path);
    item_result->lpVtbl->Release(item_result);
    file_dialog->lpVtbl->Release(file_dialog);

    return file_result;
}

internal b32 SaveToFileFromPicker(void* data, u64 data_size) {
    Xtal_MArenaTemp       scratch = Xtal_GetScratch(NULL, 0);
    Xtal_FilePickerResult file    = CreateFileSaveDialog(scratch.arena);
    if (file.succeeded) {
        SaveToFile(_OpenFile(file.path, Xtal_OS_FilePermission_Write, Xtal_OS_FileOpen_CreateAlways), data, data_size);
    }
    Xtal_MArenaTempEnd(scratch);
    return file.succeeded;
}
internal b32 AppendToFileFromPicker(void* data, u64 data_size) {
    Xtal_MArenaTemp       scratch = Xtal_GetScratch(NULL, 0);
    Xtal_FilePickerResult file    = CreateFileSaveDialog(scratch.arena);
    if (file.succeeded) {
        AppendToFile(_OpenFile(file.path, Xtal_OS_FilePermission_Write, Xtal_OS_FileOpen_CreateAlways), data, data_size);
    }
    Xtal_MArenaTempEnd(scratch);
    return file.succeeded;
}
internal b32 LoadEntireFileFromPicker(Xtal_MArena* arena, void** data, u64* data_size) {
    Xtal_MArenaTemp       scratch = Xtal_GetScratch(NULL, 0);
    Xtal_FilePickerResult file    = CreateFileOpenDialog(scratch.arena);
    if (file.succeeded) {
        LoadEntireFile(_OpenFile(file.path, Xtal_OS_FilePermission_Write, Xtal_OS_FileOpen_CreateAlways), arena, data, data_size);
    }
    Xtal_MArenaTempEnd(scratch);
    return file.succeeded;
}
internal b32 DeleteFileFromPicker(void) {
    Xtal_MArenaTemp       scratch = Xtal_GetScratch(NULL, 0);
    Xtal_FilePickerResult file    = CreateFileOpenDialog(scratch.arena);
    if (file.succeeded) {
        DeleteFile(file.path);
    }
    Xtal_MArenaTempEnd(scratch);
    return file.succeeded;
}

#if !defined(UNICODE)
#error UNICODE is not defined - things *will* break if you ignore this error!
#endif

internal b32 GetClipboard(Xtal_MArena* arena, String8* out) {
    b32 result = 0;

    // NOTE(geni): We only handle text
    if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        return result;
    }

    if (!OpenClipboard(0)) {
        OutputErrorMessage("Clipboard Error", "Failed to open clipboard.");
        return result;
    }

    // NOTE(geni): Get text
    {
        HANDLE data_handle = GetClipboardData(CF_UNICODETEXT);
        // NOTE(geni): This call won't compile if you don't define UNICODE, because Win32
        u16* text = GlobalLock(data_handle);
        if (text == NULL) {
            // TODO(geni): When the fuck does this even happen!?
            OutputErrorMessage("Clipboard Error", "Failed to lock clipboard contents. (how?!)");
            goto end;
        }
        *out = S8_PushFromS16(arena, S16_FromWString(text));
        GlobalUnlock(data_handle);
        result = 1;
    }

end:
    CloseClipboard();
    return result;
}

internal void SetClipboard(String8 s8_contents) {
    // NOTE(geni): We only handle text

    Xtal_MArenaTemp scratch  = Xtal_GetScratch(NULL, 0);
    String16        contents = S16_PushFromS8(scratch.arena, s8_contents);

    // TODO(geni): Beware, as per MSDN: "If an application calls OpenClipboard with hwnd set to NULL, EmptyClipboard sets the clipboard owner to NULL; this causes SetClipboardData to fail." This doesn't seem to actually be true (at least on 19043.1288), or maybe I'm misunderstanding. I'd rather not find out, honestly
    if (!OpenClipboard(0)) {
        OutputErrorMessage("Clipboard Error", "Failed to open clipboard.");
        goto end;
    }

    if (!EmptyClipboard()) {
        OutputErrorMessage("Clipboard Error", "Failed to empty clipboard.");
        goto end;
    }

    // NOTE(geni): This WILL fail to compile if UNICODE is not defined
    HANDLE contents_copy_handle = GlobalAlloc(GMEM_MOVEABLE, (contents.size + 1) * sizeof(u16));
    if (!contents_copy_handle) {
        OutputErrorMessage("Clipboard Error", "Failed to allocate storage for new clipboard contents.");
        goto end;
    }
    u16* contents_copy = GlobalLock(contents_copy_handle);
    if (contents_copy == NULL) {
        // TODO(geni): When the fuck does this even happen!?
        OutputErrorMessage("Clipboard Error", "Failed to lock new clipboard contents. (how?!)");
        goto end;
    }
    S16_CopyToFixedSizeBuffer(contents, contents_copy);
    contents_copy[contents.size] = L'\0';
    SetClipboardData(CF_UNICODETEXT, contents_copy);
    GlobalUnlock(contents_copy);

end:
    Xtal_MArenaTempEnd(scratch);
    CloseClipboard();
}

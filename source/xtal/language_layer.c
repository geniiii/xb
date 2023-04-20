internal void _AssertFailure(String8 expression, u32 line, String8 file, b32 crash) {
    LogError("[Assertion Failure] Assertion of \"%S\" at %S:%u failed", expression, file, line);
    xtal_os->OutputErrorMessage("Assertion Failure", "Assertion of \"%S\" at %S:%u failed",
                                expression, file, line);
    if (crash) {
#ifdef _DEBUG
        BreakDebugger();
#else
        abort();
#endif
    }
}

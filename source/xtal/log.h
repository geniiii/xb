typedef u32 LogFlags;
enum {
    Log_Info,
    Log_Warning = (1 << 0),
    Log_Error   = (1 << 1),
};
#if XTAL_DEBUG
#define Log(...)        _DebugLog(Log_Info, S8Lit(__FILE__), __LINE__, __VA_ARGS__)
#define LogWarning(...) _DebugLog(Log_Warning, S8Lit(__FILE__), __LINE__, __VA_ARGS__)
#define LogError(...)   _DebugLog(Log_Error, S8Lit(__FILE__), __LINE__, __VA_ARGS__)
#define LogEvalB(e)     Log((e) ? "\"" #e "\" == true" : "\"" #e "\" == false")
#define LogEvalU32(e)   Log("\"" #e "\" == %u", (e))
#define LogEvalU64(e)   Log("\"" #e "\" == %llu", (e))
#define LogEvalI32(e)   Log("\"" #e "\" == %d", (e))
#define LogEvalI64(e)   Log("\"" #e "\" == %lld", (e))
#define LogEvalF32(e)   Log("\"" #e "\" == %f", (e))
#define LogEvalF64      LogEvalF32
#else
#define Log(...)
#define LogWarning(...)
#define LogError(...)
#define LogEvalB(e)
#define LogEvalU32(e)
#define LogEvalU64(e)
#define LogEvalI32(e)
#define LogEvalI64(e)
#define LogEvalF32(e)
#define LogEvalF64(e)
#endif

internal void _DebugLog(LogFlags flags, String8 file, u32 line, const char* format, ...);

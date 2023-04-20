typedef u32 LogFlags;
enum {
    Log_Info    = 0,
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

// TODO(geni): Move this to another file
#if XTAL_PLATFORM_LAYER
internal void            WriteToConsole(String8 data);
internal Xtal_OSDateTime GetDateTime(void);
#endif

internal void _DebugLog(LogFlags flags, String8 file, u32 line, const char* format, ...) {
    local_persist thread_local char info_string[4096] = {0};
    local_persist thread_local char log_string[4096]  = {0};

    // NOTE(geni): Create info string
    {
        String8 name = flags & Log_Error ? S8Lit("Error") : (flags & Log_Warning ? S8Lit("Warning") : S8Lit("Info"));
#if XTAL_PLATFORM_LAYER
        Xtal_OSDateTime time = GetDateTime();
#else
        Xtal_OSDateTime time = xtal_os->GetDateTime();
#endif
        u32     info_string_len = stbsp_snprintf(info_string, sizeof info_string, "{%04d-%02d-%02d %02d:%02d:%02d} [%S] (%S:%u): ", time.year, time.month, time.day, time.hour, time.minutes, time.seconds, name, file, line);
        String8 info_str8       = {
                  .cstr = info_string,
                  .size = info_string_len,
        };
#if XTAL_PLATFORM_LAYER
        WriteToConsole(info_str8);
#else
        xtal_os->WriteToConsole(info_str8);
#endif
    }

    // NOTE(geni): Create log string
    {
        va_list args;
        va_start(args, format);
        // TODO(geni): Add ... to end if forced to truncate
        u32 log_string_len = stbsp_vsnprintf(log_string, sizeof log_string, format, args);
        va_end(args);
        String8 log_str8 = {
            .cstr = log_string,
            .size = log_string_len,
        };
#if XTAL_PLATFORM_LAYER
        WriteToConsole(log_str8);
        WriteToConsole(S8Lit("\n"));
#else
        xtal_os->WriteToConsole(log_str8);
        xtal_os->WriteToConsole(S8Lit("\n"));
#endif
    }
}

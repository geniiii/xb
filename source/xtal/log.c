// TODO(geni): Move this to another file
#if XTAL_PLATFORM_LAYER
internal void            WriteToConsole(String8 data);
internal Xtal_OSDateTime GetDateTime(void);
#endif

internal void _DebugLog(LogFlags flags, String8 file, u32 line, const char* format, ...) {
    // TODO(geni): Maybe these should be thread-local static arrays instead
    char info_string[4096];
    char log_string[4096];

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

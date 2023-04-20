internal force_inline u16 GetYearDayFromSysTime(SYSTEMTIME time) {
    local_persist const u16 days_in_year[] = {
        0,
        31,
        31 + 28,
        31 + 28 + 31,
        31 + 28 + 31 + 30,
        31 + 28 + 31 + 30 + 31,
        31 + 28 + 31 + 30 + 31 + 30,
        31 + 28 + 31 + 30 + 31 + 30 + 31,
        31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
        31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
        31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
        31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
    };

    return days_in_year[time.wMonth - 1] + time.wDay + (time.wYear % 4 == 0 && time.wMonth > 2);
}

internal force_inline u64 FileTimeToTimestamp(FILETIME w32_timestamp) {
    return ((u64) w32_timestamp.dwLowDateTime | (u64) w32_timestamp.dwHighDateTime << 32) / 10000000ull - 11644473600ull;
}

internal Xtal_OSDateTime TimestampToDateTime(u64 timestamp) {
    u64      converted_timestamp = (timestamp + 11644473600ull) * 10000000ull;
    FILETIME w32_timestamp       = (FILETIME){
              .dwLowDateTime  = (DWORD) (converted_timestamp & 0xFFFFFFFFFFull),
              .dwHighDateTime = converted_timestamp >> 32,
    };
    SYSTEMTIME time = {0};
    if (FileTimeToSystemTime(&w32_timestamp, &time)) {
        return (Xtal_OSDateTime){
            .year     = time.wYear,
            .month    = (u8) time.wMonth,
            .day      = (u8) time.wDay,
            .hour     = (u8) time.wHour,
            .minutes  = (u8) time.wMinute,
            .seconds  = (u8) time.wSecond,
            .year_day = GetYearDayFromSysTime(time),
        };
    }

    OutputErrorMessage("Error", "Failed to convert timestamp %llu to Xtal_OSDateTime", timestamp);
    return (Xtal_OSDateTime){0};
}

internal u64 DateTimeToTimestamp(Xtal_OSDateTime date_time) {
    SYSTEMTIME w32_date_time = {
        .wYear   = date_time.year,
        .wMonth  = date_time.month,
        .wDay    = date_time.day,
        .wHour   = date_time.hour,
        .wMinute = date_time.minutes,
        .wSecond = date_time.seconds,
    };

    FILETIME w32_timestamp;
    if (SystemTimeToFileTime(&w32_date_time, &w32_timestamp)) {
        return FileTimeToTimestamp(w32_timestamp);
    }

    OutputErrorMessage("Error", "Failed to convert Xtal_OSDateTime to timestamp");
    return 0;
}

internal u64 GetTimestamp(void) {
    // TODO(geni): Error handling

    SYSTEMTIME date_time;
    FILETIME   w32_timestamp;
    GetLocalTime(&date_time);
    if (SystemTimeToFileTime(&date_time, &w32_timestamp)) {
        return FileTimeToTimestamp(w32_timestamp);
    }

    OutputErrorMessage("Error", "Failed to get timestamp");
    return 0;
}

internal Xtal_OSDateTime GetDateTime(void) {
    // TODO(geni): Error handling

    SYSTEMTIME time;
    GetLocalTime(&time);

    return (Xtal_OSDateTime){
        .year     = time.wYear,
        .month    = (u8) time.wMonth,
        .day      = (u8) time.wDay,
        .hour     = (u8) time.wHour,
        .minutes  = (u8) time.wMinute,
        .seconds  = (u8) time.wSecond,
        .year_day = GetYearDayFromSysTime(time),
    };
}

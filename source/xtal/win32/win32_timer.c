#define NANOSECONDS_PER_SECOND  1e9
#define MILLISECONDS_PER_SECOND 1000

typedef struct {
    u32 ticks_per_second;
    u64 counts_per_second;
    u64 nanoseconds_per_tick;
    f64 nanoseconds_per_count;
    f64 milliseconds_per_count;

    u64 _begin_frame;
    u64 _remainder;

    u64 total_ticks;
    u64 ticks_to_do;
} Timer;

global Timer global_win32_timer = {
    .ticks_per_second = TICK_RATE,
};

internal f64 GetDeltaNS(Timer* timer) {
    u64 current_time;
    QueryPerformanceCounter((LARGE_INTEGER*) &current_time);

    return (current_time - timer->_begin_frame) * timer->nanoseconds_per_count;
}
internal f64 GetDelta(Timer* timer) {
    u64 current_time;
    QueryPerformanceCounter((LARGE_INTEGER*) &current_time);

    return (f64) ((current_time - timer->_begin_frame)) / timer->counts_per_second;
}
internal f64 GetTime(Timer* timer) {
    return global_os.current_time + GetDelta(timer);
}

internal void TimerInit(Timer* timer) {
    HardAssert(timer->ticks_per_second > 0);
    if (!QueryPerformanceFrequency((LARGE_INTEGER*) &timer->counts_per_second)) {
        OutputErrorMessage("Timer Error", "QueryPerformanceFrequency failed!");
        // NOTE(geni): This is fatal as game ticks won't be functional
        ExitProcess(1);
    }

    timer->nanoseconds_per_count  = NANOSECONDS_PER_SECOND / timer->counts_per_second;
    timer->milliseconds_per_count = (f64) MILLISECONDS_PER_SECOND / timer->counts_per_second;
    timer->nanoseconds_per_tick   = (u64) ((f64) NANOSECONDS_PER_SECOND / timer->ticks_per_second);
}

internal void TimerSetTickRate(Timer* timer, u32 tickrate) {
    timer->ticks_per_second     = tickrate;
    timer->nanoseconds_per_tick = (u64) ((f64) NANOSECONDS_PER_SECOND / timer->ticks_per_second);
}

internal void TimerBeginFrame(Timer* timer) {
    QueryPerformanceCounter((LARGE_INTEGER*) &timer->_begin_frame);
}

internal void TimerEndFrame(Timer* timer) {
    f64 delta = GetDeltaNS(timer);
    if (delta > 0.3e9) {
        delta = 0.3e9;
    }

    global_os.frametime = delta / NANOSECONDS_PER_SECOND;

    f64 tick_time      = delta + timer->_remainder;
    timer->ticks_to_do = (u64) ((f64) tick_time / timer->nanoseconds_per_tick);
    timer->_remainder  = (u64) F64Mod(tick_time, (f64) timer->nanoseconds_per_tick);

    timer->total_ticks += timer->ticks_to_do;
}

internal inline u64 GetCycles(void) {
    return __rdtsc();
}

//~ Keys

typedef u32 Xtal_CursorStyle;
enum Xtal_CursorStyle {
    Xtal_CursorStyle_Normal,
    Xtal_CursorStyle_HorizontalResize,
    Xtal_CursorStyle_VerticalResize,
};

typedef u32 Key;
enum Key {
#define Key(name, str) Key_##name,
#include "os_key_list.inc"
#undef Key
    Key_Count
};

typedef u32 KeyModifiers;
enum {
    KeyModifier_Ctrl  = (1 << 0),
    KeyModifier_Shift = (1 << 1),
    KeyModifier_Alt   = (1 << 2),
};

internal String8 KeyName(Key index) {
    local_persist String8 strings[Key_Count] = {
#define Key(name, str) S8LitComp(str),
#include "os_key_list.inc"
#undef Key
    };

    return index < ArrayCount(strings) ? strings[index] : S8Lit("(Invalid Key)");
}

//~ Mouse

typedef u32 MouseButton;
enum MouseButton {
    MouseButton_Left,
    MouseButton_Right,
    MouseButton_Middle,
};

//~ File I/O

typedef struct {
    u64     id;
    String8 path;
} Xtal_OS_File;

typedef u32 Xtal_OS_FileOpen;
enum {
#if XTAL_BUILD_WIN32
    Xtal_OS_FileOpen_CreateAlways = 2,
    Xtal_OS_FileOpen_OpenExisting,
    Xtal_OS_FileOpen_CreateNew,
#else
#error Unsupported platform
#endif
};

typedef u32 Xtal_OS_FileSeek;
enum {
#if XTAL_BUILD_WIN32
    Xtal_OS_FileSeek_FromStart = 0,
    Xtal_OS_FileSeek_FromEnd   = 2,
#else
#error Unsupported platform
#endif
};

typedef u32 Xtal_OS_FilePermission;
enum {
    // NOTE(geni): Protection/permission levels
    Xtal_OS_FilePermission_Read  = (1 << 0),
    Xtal_OS_FilePermission_Write = (1 << 1),
};

//~ Directory listing

typedef struct {
    String8 name;
    String8 extension;
    String8 path;
    u64     size;
    u64     flags;
} Xtal_OSDirectoryItem;

// TODO(geni): Maybe remove the Flag suffix from these
typedef u32 Xtal_OSDirectoryIterFlags;
enum {
    Xtal_OSDirectoryIterFlag_IncludeDirectories = (1 << 0),
    Xtal_OSDirectoryIterFlag_Recursive          = (1 << 1),
};
typedef u32 Xtal_OSDirectoryItemFlags;
enum {
    Xtal_OSDirectoryItemFlag_IsDirectory = (1 << 0),
};

//~ Memory mapped files

// TODO(geni): This may honestly be better off just having a void* handle or something, but I don't know how memory mapping works on other platforms
typedef struct {
    u32 id;
    u32 generation;
} Xtal_OS_MMapHandle;

typedef u32 Xtal_OS_MMapFlags;
enum {
    //~ Protection/Permission levels
    Xtal_OS_MMapFlags_Read      = (1 << 0),
    Xtal_OS_MMapFlags_ReadWrite = (1 << 1),
};

//~ Events

typedef u32 Xtal_OSEventType;
enum {
    Xtal_OSEventType_Null,

    //~ Keyboard
    Xtal_OSEventType_KeyStart,
    Xtal_OSEventType_CharacterInput,
    Xtal_OSEventType_KeyPress,
    Xtal_OSEventType_KeyRelease,

    //~ Mouse
    Xtal_OSEventType_MouseStart,
    Xtal_OSEventType_MousePress,
    Xtal_OSEventType_MouseRelease,
    Xtal_OSEventType_MouseMove,
    Xtal_OSEventType_MouseScroll,
    Xtal_OSEventType_MouseEnd,
    // TODO(geni): Is this necessary? How will this work on Linux?
    Xtal_OSEventType_MouseLeave,

    //~ Drag and drop
    Xtal_OSEventType_FileDropped,

    //~ Window state changes
    Xtal_OSEventType_WindowResized,
    Xtal_OSEventType_WindowFocusLost,
    Xtal_OSEventType_WindowFocusGained,

    Xtal_OSEventType_Count,
};

// NOTE(geni): Unicode character input
typedef struct {
    u8 data[4];
    u8 size;
} Xtal_InputCharacter;

typedef struct {
    Xtal_OSEventType type;

    //~ Keyboard
    Key                 key;
    KeyModifiers        modifiers;
    Xtal_InputCharacter character;

    //~ Mouse
    MouseButton mouse_button;
    ivec2s      delta;

    //~ Drag and drop
    String8 path;
} Xtal_OSEvent;

typedef struct {
    u16 year;
    u8  month;
    u8  day;

    u8 hour;
    u8 minutes;
    u8 seconds;
    // NOTE(geni): Unnecessary
    // u16 milliseconds;

    // NOTE(geni): 0 - 365
    u16 year_day;
} Xtal_OSDateTime;

typedef struct {
    void* handle;
} Xtal_Thread;
#define XTAL_THREAD_DEFAULTSTACKSIZE 0

#if XTAL_BUILD_WIN32
internal force_inline void Xtal_AtomicI32Increment(volatile i32* a) {
    _InterlockedIncrement((volatile long*) a);
}
internal force_inline void Xtal_AtomicI32Decrement(volatile i32* a) {
    _InterlockedDecrement((volatile long*) a);
}
internal force_inline i32 Xtal_AtomicI32Exchange(volatile i32* a, i32 b) {
    return _InterlockedExchange((volatile long*) a, (long) b);
}
internal force_inline i32 Xtal_AtomicI32CompareExchange(volatile i32* a, i32 b, i32 comp) {
    return _InterlockedCompareExchange((volatile long*) a, (long) b, (long) comp);
}
#else
#error Unsupported platform
#endif

typedef struct {
    String8 path;
    b32     succeeded;
} Xtal_FilePickerResult;

//~ Platform data

typedef struct {
    //~ State
    // NOTE(geni): Arenas
    Xtal_MArena permanent_arena;
    Xtal_MArena frame_arena;

    // NOTE(geni): Paths
    String8 executable_folder_path;
    String8 working_directory_path;

    b32    quit;
    b32    fullscreen;
    ivec2s window_size;
    f64    current_time;
    f64    frametime;

    b32    cursor_locked;
    b32    raw_mouse_input;
    ivec2s mouse_position;

    // NOTE(geni): Event queue
    u64          event_count;
    Xtal_OSEvent events[8192];

    // NOTE(geni): Memory mapped file handle pool
    Xtal_Pool mmap_handles;

    //~ Memory
    void* (*Reserve)(u64 size);
    void (*Release)(void* memory);
    void (*Commit)(void* memory, u64 size);
    void (*Decommit)(void* memory, u64 size);
    void* (*HeapAlloc)(u64 size);
    void* (*HeapRealloc)(void* memory, u64 size);
    void (*HeapFree)(void* memory);

    //~ File I/O
    Xtal_OS_File (*OpenFile)(String8 path, Xtal_OS_FilePermission permissions, Xtal_OS_FileOpen open_method);
    void (*WriteToFile)(Xtal_OS_File file, void* data, u64 data_size, u64 offset, Xtal_OS_FileSeek write_method);
    void (*SaveToFile)(Xtal_OS_File file, void* data, u64 data_size);
    void (*AppendToFile)(Xtal_OS_File file, void* data, u64 data_size);
    void (*LoadEntireFile)(Xtal_OS_File file, Xtal_MArena* arena, void** data, u64* data_size);
    void (*ReadFromFileTo)(Xtal_OS_File file, void* data, u64 data_size, u64 offset, Xtal_OS_FileSeek read_method);
    u64 (*GetFileSize)(Xtal_OS_File file);
    void (*CloseFile)(Xtal_OS_File file);

    void (*DeleteFile)(String8 path);
    b32 (*CopyFile)(String8 source, String8 dest);
    b32 (*FileExists)(String8 path);

    b32 (*MakeDirectory)(String8 path);
    void (*DeleteDirectory)(String8 path);
    b32 (*DirectoryExists)(String8 path);

    //~ File picker
    // TODO(geni): Filter by extension(s)
    Xtal_FilePickerResult (*CreateFileOpenDialog)(Xtal_MArena* arena);
    Xtal_FilePickerResult (*CreateFileSaveDialog)(Xtal_MArena* arena);
    b32 (*SaveToFileFromPicker)(void* data, u64 data_size);
    b32 (*AppendToFileFromPicker)(void* data, u64 data_size);
    b32 (*LoadEntireFileFromPicker)(Xtal_MArena* arena, void** data, u64* data_size);
    b32 (*DeleteFileFromPicker)(void);

    //~ Memory mapped files
    // NOTE(geni): Size parameter is optional, only used when open_method is CreateAlways
    Xtal_OS_MMapHandle (*MemoryMapFile)(String8 path, Xtal_OS_MMapFlags open_method, Xtal_OS_MMapFlags permissions, u64 size, void** data, u64* data_size);
    Xtal_OS_MMapHandle (*MemoryMapFileHandle)(Xtal_OS_File file, Xtal_OS_MMapFlags permissions, u64 size, void** data, u64* data_size);
    void (*UnmapFile)(Xtal_OS_MMapHandle handle);

    //~ Directories
    Xtal_OSDirectoryItem* (*BeginIterDirectory)(Xtal_MArena* arena, String8 path, Xtal_OSDirectoryIterFlags flags);
    b32 (*IterDirectory)(Xtal_OSDirectoryItem* item);

    //~ Clipboard
    b32 (*GetClipboard)(Xtal_MArena* arena, String8* out);
    void (*SetClipboard)(String8 contents);

    //~ Time
    // NOTE(geni): Xtal timestamps are in Unix time
    u64 (*GetTimestamp)(void);
    u64 (*DateTimeToTimestamp)(Xtal_OSDateTime date_time);
    Xtal_OSDateTime (*GetDateTime)(void);
    Xtal_OSDateTime (*TimestampToDateTime)(u64 timestamp);
    u64 (*GetCycles)(void);
    u32 (*GetTickRate)(void);
    void (*SetTickRate)(u32 tickrate);

    //~ Cursor
    void (*ToggleCursor)(b32 show);

    //~ Raw input
    void (*ToggleRawMouse)(b32 enable);

    //~ Graphics
    void* (*LoadOpenGLProcedure)(const char* name);
    void (*RefreshScreen)(void);

    //~ Threading
    Xtal_Thread (*CreateThread)(i32 (*thread_proc)(void*), void* user_data, u32 stack_size);
    void (*JoinThread)(Xtal_Thread* thread);
    void (*DetachThread)(Xtal_Thread* thread);
    void (*DestroyThread)(Xtal_Thread* thread);

    //~ Utility
    void (*OutputErrorMessage)(const char* error_type, const char* error_format, ...);
    void (*WriteToConsole)(String8 data);
} Xtal_OSState;
global Xtal_OSState* xtal_os;

//~ Entry Points

#ifdef _MSC_VER
#define XTAL_APP_ENTRY_POINT __declspec(dllexport)
#else
#define XTAL_APP_ENTRY_POINT
#endif

#define XTAL_APP_PERMANENT_LOAD XTAL_APP_ENTRY_POINT void Xtal_PermanentLoad(Xtal_OSState* os)
typedef void  Xtal_ApplicationPermanentLoadCallback(Xtal_OSState*);
internal void Xtal_ApplicationPermanentLoadStub(Xtal_OSState* delta) { Unreferenced(delta); }

#define XTAL_APP_HOT_LOAD XTAL_APP_ENTRY_POINT void Xtal_HotLoad(Xtal_OSState* os, b32 reloaded)
typedef void  Xtal_ApplicationHotLoadCallback(Xtal_OSState*, b32);
internal void Xtal_ApplicationHotLoadStub(Xtal_OSState* os, b32 reloaded) {
    Unreferenced(os);
    Unreferenced(reloaded);
}

#define XTAL_APP_HOT_UNLOAD XTAL_APP_ENTRY_POINT void Xtal_HotUnload(void)
typedef void  Xtal_ApplicationHotUnloadCallback(void);
internal void Xtal_ApplicationHotUnloadStub(void) {}

#define XTAL_APP_UPDATE XTAL_APP_ENTRY_POINT void Xtal_Update(f64 delta)
typedef void  Xtal_ApplicationUpdateCallback(f64);
internal void Xtal_ApplicationUpdateStub(f64 delta) { Unreferenced(delta); }

#define XTAL_APP_TICK XTAL_APP_ENTRY_POINT void Xtal_Tick(void)
typedef void  Xtal_ApplicationTickCallback(void);
internal void Xtal_ApplicationTickStub(void) {}

#define XTAL_APP_DRAW XTAL_APP_ENTRY_POINT void Xtal_Draw(f64 delta)
typedef void  Xtal_ApplicationDrawCallback(f64);
internal void Xtal_ApplicationDrawStub(f64 delta) { Unreferenced(delta); }

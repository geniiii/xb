// TODO(geni): Let game side choose whether to accept files
// TODO(geni): Multi-window

//~ Globals

#define WINDOW_TITLE_W (L"" WINDOW_TITLE)

global String16 global_executable_path;
global String16 global_executable_directory;
global String16 global_working_directory;
global String16 global_app_dll_path;
global String16 global_temp_app_dll_path;

global HDC          global_device_context;
global Xtal_OSState global_os;
global HINSTANCE    global_instance_handle;
global HANDLE       global_window_handle;
global HANDLE       global_conout_handle = INVALID_HANDLE_VALUE;

global Xtal_MArena global_os_arena;

global Xtal_CursorStyle global_cursor_style = Xtal_CursorStyle_Normal;

global DWORD global_main_thread_id;
global i32   global_show_command;

#define MAX_MMAP_HANDLES 128
typedef struct {
    HANDLE object;
    void*  view;
} MMapHandleTranslation;
global MMapHandleTranslation global_mmap_handle_translations[MAX_MMAP_HANDLES];

typedef struct {
    ivec2s  pos;
    ivec2s  size;
    LPCWSTR class_name;
    LPCWSTR window_name;
    DWORD   style;
    DWORD   ex_style;
} WindowParams;

//~ Implementations

#include "win32_memory.c"
#include "win32_utilities.c"
#include "win32_timer.c"
#include "win32_time.c"
#include "win32_file_io.c"
#include "win32_app_code.c"
#include "win32_com.c"
#include "win32_filepicker.c"
#include "win32_clipboard.c"
#include "win32_opengl.c"
#include "win32_rawinput.c"
#include "win32_keyboard.c"
#include "win32_threading.c"
#include "win32_resources.h"

internal inline void EndFrame(f64 delta) {
    global_os.current_time += 1.0 / delta;
}
internal u32 GetTickRate(void) {
    return global_win32_timer.ticks_per_second;
}
internal void SetTickRate(u32 tickrate) {
    TimerSetTickRate(&global_win32_timer, tickrate);
}

// NOTE(geni): Resizes window accordingly
internal HWND CreateProperWindow(WindowParams* params) {
    RECT window_size_because_fuck_microsoft = {0, 0, params->size.width, params->size.height};
    AdjustWindowRectEx(&window_size_because_fuck_microsoft, params->style, FALSE, params->ex_style);
    int width  = window_size_because_fuck_microsoft.right - window_size_because_fuck_microsoft.left;
    int height = window_size_because_fuck_microsoft.bottom - window_size_because_fuck_microsoft.top;

    return CreateWindowExW(params->ex_style,
                           params->class_name,
                           params->window_name,
                           params->style,
                           params->pos.x, params->pos.y,
                           width, height,
                           NULL, NULL, global_instance_handle, NULL);
}

global volatile b32 mouse_hover_active = 0;

internal LRESULT CALLBACK WinProc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param) {
    switch (message) {
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        case WM_MOUSEMOVE:
        case WM_MOUSELEAVE:
        case WM_INPUT:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
        case WM_UNICHAR:
        case WM_DROPFILES:
        case WM_SIZE:
        case WM_KILLFOCUS:
        case WM_LBUTTONDOWN:
        case WM_SETFOCUS: {
            PostThreadMessageW(global_main_thread_id, message, w_param, l_param);
        } break;
        case WM_SETCURSOR: {
            if (mouse_hover_active &&
                global_os.mouse_position.x >= 1 && global_os.mouse_position.x <= global_os.window_size.x - 1 &&
                global_os.mouse_position.y >= 1 && global_os.mouse_position.y <= global_os.window_size.y - 1) {
                switch (global_cursor_style) {
                    case Xtal_CursorStyle_Normal: {
                        SetCursor(LoadImageW(0, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
                    } break;
                    case Xtal_CursorStyle_HorizontalResize: {
                        SetCursor(LoadImageW(0, IDC_SIZEWE, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
                    } break;
                    case Xtal_CursorStyle_VerticalResize: {
                        SetCursor(LoadImageW(0, IDC_SIZENS, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
                    } break;
                    default: break;
                }
            } else {
                return DefWindowProcW(window_handle, message, w_param, l_param);
            }
        } break;
        case WM_CLOSE:
        case WM_DESTROY:
        case WM_QUIT: {
            global_os.quit = 1;
        }
        default: {
            return DefWindowProcW(window_handle, message, w_param, l_param);
        }
    }

    return 0;
}

internal void ProcessEvents() {
    // NOTE(geni): Do !!NOT!! call DefWindowProcW here; it won't work.
    //             This isn't called by our WinProc, it's called by our game thread.
    MSG msg;
    while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE)) {
        HWND   window_handle = global_window_handle;
        WPARAM w_param       = msg.wParam;
        LPARAM l_param       = msg.lParam;
        switch (msg.message) {
                //~ Mouse events
            case WM_LBUTTONDOWN: {
                Xtal_OSPushEvent(Xtal_OSMousePressEvent(MouseButton_Left));
            } break;
            case WM_LBUTTONUP: {
                Xtal_OSPushEvent(Xtal_OSMouseReleaseEvent(MouseButton_Left));
            } break;
            case WM_MBUTTONDOWN: {
                Xtal_OSPushEvent(Xtal_OSMousePressEvent(MouseButton_Middle));
            } break;
            case WM_MBUTTONUP: {
                Xtal_OSPushEvent(Xtal_OSMouseReleaseEvent(MouseButton_Middle));
            } break;
            case WM_RBUTTONDOWN: {
                Xtal_OSPushEvent(Xtal_OSMousePressEvent(MouseButton_Right));
            } break;
            case WM_RBUTTONUP: {
                Xtal_OSPushEvent(Xtal_OSMouseReleaseEvent(MouseButton_Right));
            } break;

            case WM_MOUSEWHEEL: {
                if (!global_os.raw_mouse_input) {
                    i16 wheel_delta = HIWORD(w_param);
                    Xtal_OSPushEvent(Xtal_OSMouseScrollEvent((ivec2s){0, wheel_delta / 120}));
                }
            } break;
            case WM_MOUSEHWHEEL: {
                if (!global_os.raw_mouse_input) {
                    i16 wheel_delta = HIWORD(w_param);
                    Xtal_OSPushEvent(Xtal_OSMouseScrollEvent((ivec2s){wheel_delta / 120, 0}));
                }
            } break;

            case WM_MOUSEMOVE: {
                ivec2s last_mouse        = global_os.mouse_position;
                global_os.mouse_position = (ivec2s){(u32) l_param & 0xFFFF, (u32) l_param >> 16};
                Xtal_OSPushEvent(Xtal_OSMouseMoveEvent((ivec2s){global_os.mouse_position.x - last_mouse.x,
                                                                global_os.mouse_position.y - last_mouse.y}));

                if (!mouse_hover_active) {
                    mouse_hover_active                = 1;
                    TRACKMOUSEEVENT track_mouse_event = {
                        .cbSize      = sizeof track_mouse_event,
                        .dwFlags     = TME_LEAVE,
                        .hwndTrack   = window_handle,
                        .dwHoverTime = HOVER_DEFAULT,
                    };
                    TrackMouseEvent(&track_mouse_event);
                }
            } break;
            case WM_MOUSELEAVE: {
                mouse_hover_active = 0;
                Xtal_OSPushEvent(Xtal_OSMouseLeaveEvent());
            } break;

            case WM_INPUT: {
                HandleRawInputMessage(l_param);
            } break;

            //~ Keyboard events

            // NOTE(geni): Key press/release event
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP: {
                HandleKeyPressMessage(w_param, l_param);
            } break;

            // NOTE(geni): Input event
            case WM_CHAR: {
                HandleCharInputMessage(w_param);
            } break;
            case WM_UNICHAR: {
                u32 codepoint = (u32) w_param;

                Xtal_OSEvent event = {
                    .type      = Xtal_OSEventType_CharacterInput,
                    .character = {.size = Xtal_Unicode_UTF8SizeFromCodepoint(codepoint)},
                };
                Xtal_Unicode_UTF8FromCodepoint(codepoint, &event.character.data[0], sizeof event.character.data);
                Xtal_OSPushEvent(event);
            } break;

            //~ Miscellaneous

            // NOTE(geni): Dropped file event
            // TODO(geni): Consider replacing with COM garbage (IDropSource)
            //             Enjoy reading these, future me:
            //             https://docs.microsoft.com/en-us/windows/win32/com/processes--threads--and-apartments
            //             https://docs.microsoft.com/en-us/dotnet/api/microsoft.visualstudio.ole.interop.idroptarget
            //             https://docs.microsoft.com/en-us/windows/win32/api/unknwn/nn-unknwn-iunknown
            case WM_DROPFILES: {
                HDROP    dropped      = (HDROP) w_param;
                u32      needed_bytes = DragQueryFileW(dropped, 0, NULL, 0) + 1;
                String16 path         = {
                            .s    = Xtal_MArenaPushTypeN(&global_os.frame_arena, u16, needed_bytes),
                            .size = needed_bytes - 1,
                };

                // TODO(geni): We only handle the first dropped file
                if (DragQueryFileW(dropped, 0xFFFFFFFF, NULL, 0) > 1) {
                    LogWarning("More than 1 file dropped, only handling first");
                }
                if (DragQueryFileW(dropped, 0, path.s, needed_bytes) == 0) {
                    OutputErrorMessage("Error", "Failed to handle dropped file");
                    DragFinish(dropped);
                    break;
                }
                DragFinish(dropped);
                Xtal_OSPushEvent(Xtal_OSFileDroppedEvent(S8_PushFromS16(&xtal_os->frame_arena, path)));
            } break;

            // NOTE(geni): Update window size
            case WM_SIZE: {
                global_os.window_size = (ivec2s){(i32) (l_param & 0xFFFF), (i32) (l_param >> 16)};
                if (global_os.cursor_locked) {
                    UpdateClipRect();
                }
                Xtal_OSPushEvent(Xtal_OSWindowResizedEvent());
            } break;

            case WM_KILLFOCUS: {
                Xtal_OSPushEvent(Xtal_OSWindowFocusLostEvent());
            } break;
            case WM_SETFOCUS: {
                Xtal_OSPushEvent(Xtal_OSWindowFocusGainedEvent());
            } break;

            default: {
                LogWarning("Received unimplemented message %d", msg.message);
            } break;
        }
    }
}

internal DWORD WINAPI MainThread(LPVOID user_data) {
    HWND window_handle = (HWND) user_data;

    // NOTE(geni): Load application code
    AppCode win32_app_code;
    if (!AppCodeLoad(&win32_app_code)) {
        OutputErrorMessage("Fatal Error", "Application code load failure");
        goto quit;
    }

    // NOTE(geni): Initialize COM
    InitCOM();

    // NOTE(geni): Initialize remaining platform stuff
    {
        global_os.executable_folder_path = S8_PushFromS16(&global_os_arena, global_executable_directory);
        global_os.working_directory_path = S8_PushFromS16(&global_os_arena, global_working_directory);

        global_os.window_size.x = DEFAULT_WINDOW_WIDTH;
        global_os.window_size.y = DEFAULT_WINDOW_HEIGHT;

        global_os.mmap_handles = Xtal_PoolNew(&global_os_arena, MAX_MMAP_HANDLES);

        // TODO(geni): Maybe put these in a .inc (or Metadesk)
        global_os.OpenFile       = _OpenFile;
        global_os.WriteToFile    = WriteToFile;
        global_os.SaveToFile     = SaveToFile;
        global_os.AppendToFile   = AppendToFile;
        global_os.LoadEntireFile = LoadEntireFile;
        global_os.ReadFromFileTo = ReadFromFileTo;
        global_os.GetFileSize    = _GetFileSize;
        global_os.CloseFile      = CloseFile;
        global_os.DeleteFile     = DeleteFile;
        global_os.CopyFile       = CopyFile;
        global_os.FileExists     = FileExists;

        global_os.MakeDirectory   = MakeDirectory;
        global_os.DeleteDirectory = DeleteDirectory;
        global_os.DirectoryExists = DirectoryExists;

        global_os.CreateFileOpenDialog     = CreateFileOpenDialog;
        global_os.CreateFileSaveDialog     = CreateFileSaveDialog;
        global_os.SaveToFileFromPicker     = SaveToFileFromPicker;
        global_os.AppendToFileFromPicker   = AppendToFileFromPicker;
        global_os.LoadEntireFileFromPicker = LoadEntireFileFromPicker;
        global_os.DeleteFileFromPicker     = DeleteFileFromPicker;

        global_os.MemoryMapFile = MemoryMapFile;
        global_os.UnmapFile     = UnmapFile;

        global_os.BeginIterDirectory = BeginIterDirectory;
        global_os.IterDirectory      = IterDirectory;

        global_os.GetClipboard = GetClipboard;
        global_os.SetClipboard = SetClipboard;

        global_os.GetTimestamp        = GetTimestamp;
        global_os.DateTimeToTimestamp = DateTimeToTimestamp;
        global_os.GetDateTime         = GetDateTime;
        global_os.TimestampToDateTime = TimestampToDateTime;
        global_os.GetCycles           = GetCycles;
        global_os.GetTickRate         = GetTickRate;
        global_os.SetTickRate         = SetTickRate;

        global_os.ToggleCursor   = ToggleCursor;
        global_os.ToggleRawMouse = ToggleRawMouse;

        global_os.RefreshScreen       = OpenGLRefreshScreen;
        global_os.LoadOpenGLProcedure = LoadOpenGLProcedure;

        global_os.CreateThread  = _CreateThread;
        global_os.JoinThread    = JoinThread;
        global_os.DetachThread  = DetachThread;
        global_os.DestroyThread = DestroyThread;

        global_os.OutputErrorMessage = OutputErrorMessage;
        global_os.WriteToConsole     = WriteToConsole;
    }

    // NOTE(rjf): OpenGL initialization
    {
        global_device_context = GetDC(window_handle);
        if (!InitOpenGL(global_device_context)) {
            OutputErrorMessage("Fatal Error", "OpenGL initialization failure");
            goto quit;
        }
    }

    // NOTE(geni): Load app code
    win32_app_code.PermanentLoad(&global_os);
    win32_app_code.HotLoad(&global_os, 0);

    ShowWindow(window_handle, global_show_command);
    UpdateWindow(window_handle);

    while (!global_os.quit) {
        TimerBeginFrame(&global_win32_timer);
        Xtal_MArenaClear(&global_os.frame_arena);

        // NOTE(geni): Event loop
        global_os.event_count = 0;
        ProcessEvents();
        {
            b32 last_fullscreen = global_os.fullscreen;

            win32_app_code.Update(global_os.frametime);
            while (global_win32_timer.ticks_to_do > 0) {
                win32_app_code.Tick();
                --global_win32_timer.ticks_to_do;
            }
            win32_app_code.Draw(global_os.frametime);

            if (last_fullscreen != global_os.fullscreen) {
                ToggleFullscreen(global_window_handle);
            }

            if (global_os.frametime > 0) {
                EndFrame(global_os.frametime);
            }
        }
#if XTAL_HOT_RELOAD
        // NOTE(geni): This function DOESN'T call the game's update function; it updates file write times and reloads if they don't match.
        AppCodeUpdate(&win32_app_code);
#endif

        TimerEndFrame(&global_win32_timer);
    }

    ShowWindow(global_window_handle, SW_HIDE);

quit:
    win32_app_code.HotUnload();
    AppCodeUnload(&win32_app_code);
    CleanUpOpenGL(global_device_context);
    ReleaseDC(global_window_handle, global_device_context);

    return 0;
}

i32 WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR lp_cmd_line, i32 n_show_cmd) {
    Unreferenced(lp_cmd_line);
    Unreferenced(prev_instance);

    global_instance_handle = instance;
    global_show_command    = n_show_cmd;

#if XTAL_DEBUG
    // NOTE(geni): Open debug console
    {
        global_conout_handle = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
        SoftAssert(global_conout_handle != INVALID_HANDLE_VALUE);
        SetStdHandle(STD_OUTPUT_HANDLE, global_conout_handle);
        SetStdHandle(STD_ERROR_HANDLE, global_conout_handle);
    }
#endif

    // NOTE(geni): Initialize platform arenas
    {
        xtal_os = &global_os;

        global_os.Reserve  = Reserve;
        global_os.Release  = Release;
        global_os.Commit   = Commit;
        global_os.Decommit = Decommit;

        global_os.HeapAlloc   = W32_HeapAlloc;
        global_os.HeapRealloc = W32_HeapRealloc;
        global_os.HeapFree    = W32_HeapFree;

        global_os.permanent_arena = Xtal_MArenaNew();
        global_os.frame_arena     = Xtal_MArenaNew();
        global_os_arena           = Xtal_MArenaNew();
    }

    TimerInit(&global_win32_timer);

    // NOTE(geni): Get paths
    {
        // NOTE(geni): Get executable path in a really stupid way thanks to this horrible fucking API
        Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
        String16        executable_path;
        DWORD           executable_path_capacity = MAX_PATH;
        while (1) {
            executable_path = (String16){
                .s    = Xtal_MArenaPushTypeN(scratch.arena, u16, executable_path_capacity),
                .size = executable_path_capacity - 1,
            };
            DWORD size_of_executable_path = GetModuleFileNameW(0, executable_path.s, executable_path_capacity);
            if (!GetLastError()) {
                executable_path.size = size_of_executable_path;
                break;
            }
            executable_path_capacity *= 2;
        }
        String16 executable_directory_substr = S16_SubstrUntilLastOccurrenceOf(executable_path, L'\\');
        global_executable_directory          = S16_PushCopy(&global_os_arena, executable_directory_substr);
        Xtal_MArenaTempEnd(scratch);

        global_app_dll_path      = S16_PushF(&global_os_arena, L"\\\\?\\%.*s" GAME_FILENAME ".dll", S16VArg(global_executable_directory));
        global_temp_app_dll_path = S16_PushF(&global_os_arena, L"\\\\?\\%.*s" GAME_FILENAME "_temp.dll", S16VArg(global_executable_directory));
        global_working_directory = S16_Push(&global_os_arena, GetCurrentDirectoryW(0, 0));
        GetCurrentDirectoryW((DWORD) global_working_directory.size + 1, global_working_directory.s);
    }

    WNDCLASSEXW window_class = {
        .cbSize        = sizeof window_class,
        .style         = CS_CLASSDC,
        .lpfnWndProc   = &WinProc,
        .hInstance     = global_instance_handle,
        .lpszClassName = L"XtalWindowClass",
        .hCursor       = LoadImageW(0, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED),
        .hIcon         = LoadImageW(global_instance_handle, MAKEINTRESOURCEW(XTAL_WIN32_ICON_32X32), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED),
        .hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH),
    };
    if (!RegisterClassExW(&window_class)) {
        OutputErrorMessage("Fatal Error", "Window class registration failure");
        goto quit;
    }

    WindowParams params = {
        .style       = WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        .ex_style    = WS_EX_ACCEPTFILES,
        .pos         = {       CW_USEDEFAULT,         CW_USEDEFAULT},
        .size        = {DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT},
        .window_name = WINDOW_TITLE_W,
        .class_name  = window_class.lpszClassName,
    };
    HWND window_handle   = CreateProperWindow(&params);
    global_window_handle = window_handle;
    HANDLE main_thread   = CreateThread(NULL, 0, MainThread, window_handle, 0, &global_main_thread_id);

    while (!global_os.quit) {
        MSG msg;
        GetMessageW(&msg, 0, 0, 0);
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    WaitForSingleObject(main_thread, 1000);
    CloseHandle(main_thread);

quit:
    return 0;
}

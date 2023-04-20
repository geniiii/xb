// TODO(geni): Function names all over the codebase are a mess
//             (some methods are XNew, others are XCreate (I think), some are CreateX, some are MakeX, etc..)

#define XTAL_PLATFORM_LAYER 1

//~ cglm

#include "ext/cglm/cglm.h"
#include "ext/cglm/struct.h"

//~ Windows headers

#define UNICODE
// NOTE(geni): 0x0601 = Windows 7
#define WINVER       0x0601
#define _WIN32_WINNT 0x0601
#define COM_NO_WINDOWS_H
#define RPC_NO_WINDOWS_H
#define VC_EXTRALEAN
#define NOATOM
#define NOHELP
#define NOPROFILER
#define NOGDICAPMASKS
#define NOATOM
#define NOSERVICE
#define NOMETAFILE
#define NOMINMAX
#define NOKANJI
#define NODRAWTEXT
#define NOCOLOR
#define NOSYSMETRICS
#define NOMENUS
#define NOKEYSTATES
#define NOWH
#define NOCOMM
#define NOMCX
#define NOIME
#define NODEFERWINDOWPOS
#define NONLS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <hidusage.h>
#include <objbase.h>
#include <shlobj.h>
#include <shellapi.h>
#include <Shlwapi.h>
#include <timeapi.h>
#undef OpenFile
#undef DeleteFile
#undef CopyFile

//~ CRT

#include <stdint.h>
#include <wchar.h>

//~ OpenGL

#include "win32_opengl_types.h"
#include "ext/wglext.h"

//~ xtal-win32 (mostly) headers

#define Xtal_MemoryCopy                 CopyMemory
#define Xtal_MemoryMove                 MoveMemory
#define Xtal_MemorySet(dest, fill, len) FillMemory(dest, len, fill)
#include "program_options.h"
#include "language_layer.h"
#include "math.c"
#include "memory.h"
#include "string8.h"
#include "string16.h"
#include "util/pool.h"
#include "os.h"

//~ stb_sprintf

#define STB_SPRINTF_STATIC
#define STB_SPRINTF_IMPLEMENTATION
#include "ext/stb_sprintf.h"

//~ xtal-win32 implementations

#include "log.c"
#include "language_layer.c"
#include "memory.c"
#include "chars.c"
#include "unicode.c"
#include "cstrings.c"
#include "string8.c"
#include "string16.c"
#include "util/pool.c"
#include "util/linked_list.c"
#include "os.c"

#include "win32_main.c"

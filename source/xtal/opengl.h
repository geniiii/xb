#if XTAL_BUILD_WIN32
#define APIENTRY __stdcall
#include "ext/glcorearb.h"
#else
#error "OpenGL includes for platform not supported."
#endif

#define GLProc(name, type) global PFNGL##type##PROC gl##name;
#include "gfx/gl4/opengl_proc_list.inc"
#undef GLProc

internal void Xtal_LoadAllOpenGLProcedures(void) {
#define GLProc(name, type) gl##name = (PFNGL##type##PROC) xtal_os->LoadOpenGLProcedure("gl" #name);
#include "gfx/gl4/opengl_proc_list.inc"
#undef GLProc
}

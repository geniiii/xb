// NOTE(geni): Request high performance from NVIDIA Optimus and AMD PowerXpress/DSG/Enduro
//             Sources: http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
//                      https://gpuopen.com/learn/amdpowerxpressrequesthighperformance/
__declspec(dllexport) DWORD NvOptimusEnablement                  = 1;
__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;

global HGLRC global_opengl_render_context;

internal void* LoadOpenGLProcedure(const char* name) {
    void* p = (void*) wglGetProcAddress(name);
    if (p == NULL || p == (void*) 0x1 || p == (void*) 0x2 || p == (void*) 0x3 || p == (void*) -1) {
        Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
        LogWarning("Failed to load procedure %s using wglGetProcAddress (%S), trying to directly load...", name, FormatErrorCode(scratch.arena, GetLastError()));

        HMODULE module = LoadLibraryA("opengl32.dll");
        p              = (void*) GetProcAddress(module, name);
        if (p == NULL) {
            LogWarning("Failed to load procedure %s (%S)", name, FormatErrorCode(scratch.arena, GetLastError()));
        }
        Xtal_MArenaTempEnd(scratch);
    }

    return p;
}

global PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB;
global PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
global PFNWGLSWAPINTERVALEXTPROC         wglSwapIntervalEXT;

internal void LoadWGLFunctions(void) {
    wglChoosePixelFormatARB    = (PFNWGLCHOOSEPIXELFORMATARBPROC) LoadOpenGLProcedure("wglChoosePixelFormatARB");
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) LoadOpenGLProcedure("wglCreateContextAttribsARB");
    wglSwapIntervalEXT         = (PFNWGLSWAPINTERVALEXTPROC) LoadOpenGLProcedure("wglSwapIntervalEXT");
}

internal b32 InitOpenGL(HDC device_context) {
    PIXELFORMATDESCRIPTOR pfd = {
        .nSize        = sizeof pfd,
        .nVersion     = 1,
        .dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType   = PFD_TYPE_RGBA,
        .cColorBits   = 32,
        .cDepthBits   = 24,
        .cStencilBits = 8,
        .iLayerType   = PFD_MAIN_PLANE,
    };

    i32 pixel_format = ChoosePixelFormat(device_context, &pfd);
    if (!pixel_format) {
        OutputErrorMessage("Fatal Error", "Could not initialize WGL");
        return 0;
    }

    SetPixelFormat(device_context, pixel_format, &pfd);
    HGLRC gl_dummy_render_context = wglCreateContext(device_context);
    wglMakeCurrent(device_context, gl_dummy_render_context);

    LoadWGLFunctions();

    // NOTE(rjf): Setup real pixel format
    {
        // clang-format off
	i32 pf_attribs_i[] = {
	    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
	    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
	    WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
	    WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
	    WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
	    WGL_DEPTH_BITS_ARB,     24,

	    // TODO(geni): The game should decide whether or not it wants MSAA
	    WGL_SAMPLE_BUFFERS_ARB, GL_FALSE,
	    WGL_SAMPLES_ARB, 0,
	    0,
	};
        // clang-format on

        UINT num_formats = 0;
        wglChoosePixelFormatARB(device_context, pf_attribs_i, 0, 1, &pixel_format, &num_formats);

        if (!pixel_format) {
            OutputErrorMessage("Fatal Error", "Could not initialize WGL");
            return 0;
        }
    }

    // clang-format off
    const i32 context_attribs[] = {
	WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
	WGL_CONTEXT_MINOR_VERSION_ARB, 2,
	WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#if XTAL_DEBUG
	WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
#else
        WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
#endif
	0,
    };
    // clang-format on

    global_opengl_render_context = wglCreateContextAttribsARB(device_context, gl_dummy_render_context, context_attribs);
    if (global_opengl_render_context) {
        wglMakeCurrent(device_context, 0);
        wglDeleteContext(gl_dummy_render_context);
        wglMakeCurrent(device_context, global_opengl_render_context);
        wglSwapIntervalEXT(1);
        return 1;
    }

    return 0;
}

internal void CleanUpOpenGL(HDC device_context) {
    wglMakeCurrent(device_context, 0);
    wglDeleteContext(global_opengl_render_context);
}

internal void OpenGLRefreshScreen(void) {
    SwapBuffers(global_device_context);
}

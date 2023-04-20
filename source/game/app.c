//~ Entrypoints

#if XTAL_DEBUG
void __stdcall GLMessageCallback(GLenum        source,
                                 GLenum        type,
                                 GLuint        id,
                                 GLenum        severity,
                                 GLsizei       length,
                                 const GLchar* message,
                                 const void*   user_param) {
    Unreferenced(id);
    Unreferenced(length);
    Unreferenced(user_param);

    const char* format = "OpenGL: type = 0x%x, source = 0x%x, severity = 0x%x, message = %s\n";
    if (type == GL_DEBUG_TYPE_ERROR) {
        LogError(format, type, source, severity, message);
    } else {
        Log(format, type, source, severity, message);
    }
}
#endif

XTAL_APP_PERMANENT_LOAD {
    xtal_os = os;

    Log("Hello");

    game = GameContextNew(&xtal_os->permanent_arena);

    NK_CreateContext();
}

XTAL_APP_HOT_LOAD {
    xtal_os = os;
    Xtal_LoadAllOpenGLProcedures();

#if XTAL_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_FALSE);
    glDebugMessageCallback(GLMessageCallback, 0);
#endif

    game = (GameContext*) xtal_os->permanent_arena.base;

    // NOTE(geni): Set up renderer
    {
        renderer.arena = Xtal_MArenaNew();

        for (XGL_SType i = 0; i < XGL_SType_Count; ++i) {
            renderer.shaders[i] = XGL_ShaderNew(i);
        }

        // NOTE(geni): Quads
        {
            local_persist f32 quad_vertices[] = {
                0.0f,
                1.0f,
                0.0f,
                1.0f,
                0.0f,
                0.0f,
                0.0f,
                0.0f,
                1.0f,
                1.0f,
                1.0f,
                1.0f,

                1.0f,
                0.0f,
                1.0f,
                0.0f,
                0.0f,
                0.0f,
                0.0f,
                0.0f,
                1.0f,
                1.0f,
                1.0f,
                1.0f,
            };

            renderer.quad_vbo = XGL_VBONew();
            XGL_VBOStorage(renderer.quad_vbo, &quad_vertices, sizeof quad_vertices);

            renderer.blank_vao = XGL_VAONew();
            renderer.quad_vao  = XGL_VAONew();
            XGL_VAOAttachVBO(renderer.quad_vao, renderer.quad_vbo, 4 * sizeof(f32));
            // NOTE(geni): Position
            XGL_VAOAttr(renderer.quad_vao, 0, 2, GL_FLOAT, 0);
            // NOTE(geni): UVs
            XGL_VAOAttr(renderer.quad_vao, 1, 2, GL_FLOAT, sizeof(f32) * 2);
        }
    }

    NK_Init();

    state_hotload_callbacks[game->state](NULL, reloaded);
}

XTAL_APP_HOT_UNLOAD {
    state_hotunload_callbacks[game->state]();

    for (XGL_SType i = 0; i < XGL_SType_Count; ++i) {
        XGL_ShaderDestroy(renderer.shaders[i]);
    }

    XGL_VAODestroy(renderer.blank_vao);
    XGL_VAODestroy(renderer.quad_vao);
    XGL_VBODestroy(renderer.quad_vbo);

    NK_Destroy();

    Xtal_MArenaRelease(&renderer.arena);
}

XTAL_APP_UPDATE {
    if (mouse.left_pressed) {
        mouse.left_pressed = 0;
    }
    if (mouse.right_pressed) {
        mouse.right_pressed = 0;
    }

    if (game->state_changed) {
        game->state_changed = 0;
        return;
    }

    state_update_callbacks[game->state](delta);
}

XTAL_APP_TICK {
    if (game->state_changed) {
        game->state_changed = 0;
        return;
    }

    state_tick_callbacks[game->state]();
}

XTAL_APP_DRAW {
    if (game->state_changed) {
        game->state_changed = 0;
        return;
    }

    state_draw_callbacks[game->state](delta);

    xtal_os->RefreshScreen();
}

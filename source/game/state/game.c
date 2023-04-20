#define LOGGING_ENABLED 0

#if LOGGING_ENABLED
global Xtal_OS_File log_file;
#endif

internal void Game_HotLoad(Xtal_OSState* _os, b32 reloaded) {
    Unreferenced(_os);
    Unreferenced(reloaded);

    ResetInputs();

    if (!reloaded) {
        game->hide_ui = 0;
        InitUI(&game->ui);

        game->gb.arena = Xtal_MArenaNew();
        GB_Init(&game->gb);
        if (xtal_os->FileExists(S8Lit("rom.gb"))) {
            GB_LoadROM(&game->gb, S8Lit("rom.gb"));
        } else {
            game->gb.rom_loaded = 0;
        }
    } else {
        PushMessage(1.0f, "Reloaded");
    }

    renderer.gb_fb   = XGL_TexNew((ivec2s){FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT}, XGL_TexFormat_R8B8G8A8, XGL_TexGPUFormat_RGBA, XGL_TexFiltering_Nearest, XGL_TexWrap_ClampToEdge);
    renderer.gb_vram = XGL_TexNew((ivec2s){VRAM_FRAMEBUFFER_WIDTH, VRAM_FRAMEBUFFER_HEIGHT}, XGL_TexFormat_R8B8G8A8, XGL_TexGPUFormat_RGBA, XGL_TexFiltering_Nearest, XGL_TexWrap_ClampToEdge);

    glViewport(0, 0, xtal_os->window_size.width, xtal_os->window_size.height);
    renderer.ortho_projection = glms_ortho(0, (f32) xtal_os->window_size.width, (f32) xtal_os->window_size.height, 0, -1.0f, 1.0f);
#if LOGGING_ENABLED
    log_file = xtal_os->OpenFile(S8Lit("log.txt"), Xtal_OS_FilePermission_Write, Xtal_OS_FileOpen_CreateAlways);
#endif
}

internal void Game_HotUnload(void) {
    XGL_TexDestroy(renderer.gb_fb);
    XGL_TexDestroy(renderer.gb_vram);
#if LOGGING_ENABLED
    xtal_os->CloseFile(log_file);
#endif
}

internal inline const OpcodeInfo* GB_FetchOpcode(Gameboy* gb) {
    u8 op = GB_ReadU8NoCycle(gb, gb->cpu.regs.pc++);

    GB_CycleT(gb, 2);
    b32 int_fired = gb->cpu.hw_regs.ime && (gb->cpu.hw_regs.int_flag & gb->cpu.hw_regs.int_enable & 0x1F);
    GB_CycleT(gb, 2);

    // NOTE(geni): Interrupt handler
    // TODO(geni): Needs documentation
    if (int_fired) {
        --gb->cpu.regs.pc;
        GB_Cycle1M(gb);

        gb->cpu.hw_regs.ime = 0;
        GB_Cycle1M(gb);

        --gb->cpu.regs.sp;
        GB_WriteU8(gb, gb->cpu.regs.sp, gb->cpu.regs.pc >> 8);
        --gb->cpu.regs.sp;

        GB_CycleT(gb, 2);
        u8 int_id;
        int_fired = GB_GetFirstInterrupt(gb, &int_id);
        GB_CycleT(gb, 2);

        GB_WriteU8(gb, gb->cpu.regs.sp, gb->cpu.regs.pc & 0xFF);

        if (int_fired) {
            gb->cpu.regs.pc = 0x40 + int_id * 8;
            ClearBit(gb->cpu.hw_regs.int_flag, int_id);
        } else {
            gb->cpu.regs.pc = 0;
        }
        op = GB_ReadU8NoCycle(gb, gb->cpu.regs.pc++);
    }

    // TODO(geni): Measure cost of "vtable", consider using switch case if necessary
    const OpcodeInfo* op_info = &opcodes[op];
    if (op == 0xCB) {
        op_info = &cb_opcodes[GB_ReadU8(gb, gb->cpu.regs.pc++)];
    }

    return op_info;
}

internal void GB_Tick(Gameboy* gb) {
    if (!gb->rom_loaded) {
        return;
    }

    if (gb->input != gb->last_input) {
        gb->last_input = gb->input;
    }

#if LOGGING_ENABLED
    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
#endif
    // TODO(geni): Currently, we pretend the Gameboy runs at 60Hz, but it's actually around 59.727500569606Hz
    while (gb->cpu.cycles < 70224) {
#if LOGGING_ENABLED
        String8 log = S8_PushF(scratch.arena,
                               "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X\n",
                               gb->cpu.regs.a, gb->cpu.regs.flags, gb->cpu.regs.b, gb->cpu.regs.c, gb->cpu.regs.d, gb->cpu.regs.e, gb->cpu.regs.h, gb->cpu.regs.l,
                               gb->cpu.regs.sp, gb->cpu.regs.pc);
        xtal_os->AppendToFile(log_file, log.data, log.size);
#endif

        if (gb->cpu.hw_regs.ime_toggled) {
            gb->cpu.hw_regs.ime         = 1;
            gb->cpu.hw_regs.ime_toggled = 0;
        }

        if (gb->cpu.halt) {
            GB_Cycle1M(gb);
            if (gb->cpu.hw_regs.int_flag) {
                // TODO(geni): Need to confirm this
                // GB_Cycle1M(gb);
                gb->cpu.halt = 0;
            } else {
                continue;
            }
        }

        const OpcodeInfo* op_info = GB_FetchOpcode(gb);
        if (gb->cpu.halt_bug) {
            gb->cpu.halt_bug = 0;
            --gb->cpu.regs.pc;
        }
        op_info->func(gb);
    }
    gb->cpu.cycles -= 70224;
#if LOGGING_ENABLED
    Xtal_MArenaTempEnd(scratch);
#endif

    for (u32 i = Input_Right; i <= Input_Start; ++i) {
        ToggleBitConditional(game->gb.input, i - Input_Right, !game->inputs[i].down);
    }
}

internal void Game_Update(f64 delta) {
    // TODO(geni): I'm not sure atomics are necessary...
    if (Xtal_AtomicI32CompareExchange(&game->gb.pending_reload, 0, 1)) {
        // TODO(geni): This is.. suboptimal
        Xtal_MArena temp       = game->gb.arena;
        game->gb.arena         = game->gb.pending_arena;
        game->gb.pending_arena = temp;
        Xtal_MArenaClear(&game->gb.pending_arena);

        GB_Init(&game->gb);
        GB_LoadROM(&game->gb, game->gb.pending_rom_path);
    }

    b32 update_nuklear = !game->hide_ui || game->paused;
    if (update_nuklear) {
        nk_input_begin(&game->ui.nk_ctx);
    }
    for (Xtal_OSEvent* event = &xtal_os->events[0]; event < &xtal_os->events[0] + xtal_os->event_count; ++event) {
        // NOTE(geni): Shared events
        switch (event->type) {
            case Xtal_OSEventType_KeyPress:
            case Xtal_OSEventType_KeyRelease: {
                UpdateInput(event);
            } break;

            case Xtal_OSEventType_WindowFocusLost:
            case Xtal_OSEventType_WindowFocusGained: {
                ResetInputs();
#if !XTAL_DEBUG
                // NOTE(geni): Pause if focus lost, unpause if gained
                game->paused = event->type == Xtal_OSEventType_WindowFocusLost;
#endif
            } break;

            case Xtal_OSEventType_WindowResized: {
                UpdateTransformMatrices();
            } break;
        }

        // NOTE(geni): Update Nuklear
        if (update_nuklear) {
            NK_HandleEvent(event);
            continue;
        }

        // NOTE(geni): Ingame-only events
        switch (event->type) {
            case Xtal_OSEventType_MousePress: {
                if (event->mouse_button == MouseButton_Left) {
                    mouse.left_held    = 1;
                    mouse.left_pressed = 1;
                } else if (event->mouse_button == MouseButton_Right) {
                    mouse.right_held    = 1;
                    mouse.right_pressed = 1;
                }
            } break;
            case Xtal_OSEventType_MouseRelease:
            case Xtal_OSEventType_MouseLeave: {
                if (event->mouse_button == MouseButton_Left) {
                    mouse.left_held    = 0;
                    mouse.left_pressed = 0;
                } else if (event->mouse_button == MouseButton_Right) {
                    mouse.right_held    = 0;
                    mouse.right_pressed = 0;
                }
            } break;
        }
    }
    if (update_nuklear) {
        nk_input_end(&game->ui.nk_ctx);
    }

    if (game->inputs[Input_Pause].pressed) {
        game->paused = !game->paused;
    }
    if (game->inputs[Input_ToggleDebugUI].pressed) {
        game->hide_ui = !game->hide_ui;
    }
    if (game->inputs[Input_ToggleFullscreen].pressed) {
        xtal_os->fullscreen = !xtal_os->fullscreen;
    }
    if (game->inputs[Input_OpenROM].pressed) {
        GB_CreateOpenROMDialog();
    }

    UpdateMessages((f32) delta);
    UpdateUI(&game->ui);

    ResetInputs();
}

internal void Game_Tick(void) {
    if (game->paused) {
        return;
    }

    GB_Tick(&game->gb);
}

internal void Game_Draw(f64 delta) {
    Unreferenced(delta);

    glClearNamedFramebufferfv(0, GL_COLOR, 0, (GLfloat[4]){0});

    NK_Draw(NK_ANTI_ALIASING_ON);
}

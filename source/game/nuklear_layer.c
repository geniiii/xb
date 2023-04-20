//~ Globals

global void* _nk_vertices;
global void* _nk_elements;

#define NK_MAX_VERTEX_MEMORY  Kilobytes(512)
#define NK_MAX_ELEMENT_MEMORY Kilobytes(128)

//~ Extra widgets/wrappers

internal void NK_LabelS8(struct nk_context* ctx, String8 str, nk_flags align) {
    nk_text(ctx, S8AsCStrIVArg(str), align);
}
internal void NK_LabelColoredS8(struct nk_context* ctx, String8 str, nk_flags align, struct nk_color color) {
    nk_text_colored(ctx, S8AsCStrIVArg(str), align, color);
}
internal b32 NK_ButtonLabelS8(struct nk_context* ctx, String8 str) {
    return nk_button_text(ctx, S8AsCStrIVArg(str));
}
internal b32 NK_ButtonLabelStyledS8(struct nk_context* ctx, String8 str, struct nk_style_button* style) {
    return nk_button_text_styled(ctx, style, S8AsCStrIVArg(str));
}
internal void NK_ColoredRect(struct nk_context* ctx, u32 color) {
    struct nk_command_buffer* canvas;
    canvas = nk_window_get_canvas(ctx);

    struct nk_rect               space;
    enum nk_widget_layout_states state;
    state = nk_widget(&space, ctx);
    if (!state) {
        return;
    }

    nk_fill_rect(canvas, space, 0, nk_rgba_u32(color));
}
internal void NK_HorizontalSeparator(struct nk_context* ctx) {
    struct nk_rect               space;
    enum nk_widget_layout_states state  = nk_widget(&space, ctx);
    struct nk_command_buffer*    canvas = nk_window_get_canvas(ctx);
    if (!state) {
        return;
    }
    nk_fill_rect(canvas, space, 0, ctx->style.window.border_color);
}
internal void NK_DrawColor(struct nk_context* ctx, u32 color) {
    struct nk_command_buffer* canvas;
    canvas = nk_window_get_canvas(ctx);

    struct nk_rect               space;
    enum nk_widget_layout_states state;
    state = nk_widget(&space, ctx);
    if (!state) {
        return;
    }

    nk_fill_rect(canvas, space, 0, nk_rgba_u32(color));
}

//~ Allocator

// TODO(geni): Modify Nuklear to use arenas

// NOTE(geni): Leaks a bunch of memory, I think.. Not that big of a deal
internal void* NK_Alloc(nk_handle user_data, void* old, nk_size size) {
    Unreferenced(user_data);
    Unreferenced(old);
    return Xtal_MArenaPush(&game->ui.nk_internal.arena, size);
}
internal void NK_Free(nk_handle user_data, void* old) {
    Unreferenced(user_data);
    Unreferenced(old);
}
global struct nk_allocator _nk_allocator = {
    .alloc = &NK_Alloc,
    .free  = &NK_Free,
};

// NOTE(geni): The atlas does a bunch of (de)allocations, so we'll just use the heap for it (for now)
internal void* NK_AtlasAlloc(nk_handle user_data, void* old, nk_size size) {
    Unreferenced(user_data);
    Unreferenced(old);
    return xtal_os->HeapAlloc(size);
}
internal void NK_AtlasFree(nk_handle user_data, void* old) {
    Unreferenced(user_data);
    xtal_os->HeapFree(old);
}
global struct nk_allocator _nk_atlas_allocator = {
    .alloc = &NK_AtlasAlloc,
    .free  = &NK_AtlasFree,
};

//~ Code

// NOTE(geni): GPU synchronization
internal void NK_CreateFence(void) {
    if (game->ui.nk_internal.device.fence != 0) {
        glDeleteSync(game->ui.nk_internal.device.fence);
    }
    game->ui.nk_internal.device.fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

internal void NK_Wait(void) {
    if (game->ui.nk_internal.device.fence == 0) {
        return;
    }

    while (1) {
        GLenum result = glClientWaitSync(game->ui.nk_internal.device.fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
        if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
            return;
        }
    }
}

// NOTE(geni): Fonts
internal void NK_FontStashBegin(struct nk_font_atlas** atlas) {
    game->ui.nk_internal.atlas_arena = Xtal_MArenaNew();
    nk_font_atlas_init(&game->ui.nk_internal.atlas, &_nk_atlas_allocator);
    nk_font_atlas_begin(&game->ui.nk_internal.atlas);
    *atlas = &game->ui.nk_internal.atlas;
}

internal void NK_FontStashEnd(void) {
    const void* image;
    ivec2s      size;
    image = nk_font_atlas_bake(&game->ui.nk_internal.atlas, (i32*) &size.width, (i32*) &size.height, NK_FONT_ATLAS_RGBA32);

    if (game->ui.nk_internal.device.font.handle == 0) {
        game->ui.nk_internal.device.font = XGL_TexNewWithData(size, XGL_TexFormat_R8B8G8A8, XGL_TexGPUFormat_RGBA, XGL_TexFiltering_Nearest, XGL_TexWrap_ClampToEdge, image);
    } else {
        XGL_TexUpdate(game->ui.nk_internal.device.font, image);
    }
    nk_font_atlas_end(&game->ui.nk_internal.atlas, nk_handle_id((int) game->ui.nk_internal.device.font.handle), &game->ui.nk_internal.device.null);
    nk_font_atlas_cleanup(&game->ui.nk_internal.atlas);
    Xtal_MArenaRelease(&game->ui.nk_internal.atlas_arena);
}

// NOTE(geni): Clipboard
internal void NK_ClipboardPaste(nk_handle usr, struct nk_text_edit* edit) {
    Unreferenced(usr);

    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
    String8         clipboard;
    b32             result = xtal_os->GetClipboard(scratch.arena, &clipboard);
    if (!result) {
        goto end;
    }
    nk_textedit_paste(edit, clipboard.cstr, (int) clipboard.size);
end:
    Xtal_MArenaTempEnd(scratch);
}
internal void NK_ClipboardCopy(nk_handle usr, const char* contents, int size) {
    Unreferenced(usr);
    xtal_os->SetClipboard((String8){.cstr = (char*) contents, .size = size});
}

internal void NK_CreateContext(void) {
    NK_Context* internal_ctx = &game->ui.nk_internal;

    internal_ctx->arena = Xtal_MArenaNew();

    nk_init(&game->ui.nk_ctx, &_nk_allocator, NULL);
    game->ui.nk_ctx.clip.copy  = NK_ClipboardCopy;
    game->ui.nk_ctx.clip.paste = NK_ClipboardPaste;
}

internal void NK_Init(void) {
    NK_Context* internal_ctx = &game->ui.nk_internal;
    nk_buffer_init(&internal_ctx->device.cmds, &_nk_allocator, Kilobytes(8));
    {
        internal_ctx->device.vbo = XGL_VBONew();
        internal_ctx->device.ebo = XGL_EBONew();

        XGL_VBOStorageFlags(internal_ctx->device.vbo, NULL, NK_MAX_VERTEX_MEMORY, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
        XGL_EBOStorageFlags(internal_ctx->device.ebo, NULL, NK_MAX_ELEMENT_MEMORY, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

        internal_ctx->device.vao = XGL_VAONew();
        XGL_VAOAttachVBO(internal_ctx->device.vao, internal_ctx->device.vbo, sizeof(NK_Vertex));
        XGL_VAOAttachEBO(internal_ctx->device.vao, internal_ctx->device.ebo);
        XGL_VAOAttr(internal_ctx->device.vao, 0, 2, GL_FLOAT, offsetof(NK_Vertex, position));
        XGL_VAOAttr(internal_ctx->device.vao, 1, 2, GL_FLOAT, offsetof(NK_Vertex, uv));
        XGL_VAOAttrNormalized(internal_ctx->device.vao, 2, 4, GL_UNSIGNED_BYTE, offsetof(NK_Vertex, color));

        _nk_vertices = XGL_VBOMapRange(internal_ctx->device.vbo, 0, NK_MAX_VERTEX_MEMORY, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
        _nk_elements = XGL_EBOMapRange(internal_ctx->device.ebo, 0, NK_MAX_ELEMENT_MEMORY, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    }

    struct nk_font_atlas* atlas;
    NK_FontStashBegin(&atlas);
    // NOTE(geni): Add custom fonts here
    NK_FontStashEnd();
    if (atlas->default_font) {
        nk_style_set_font(&game->ui.nk_ctx, &atlas->default_font->handle);
    }
}

internal void NK_Destroy(void) {
    // TODO(geni): Release textures, shaders, etc
    NK_Context* internal_ctx = &game->ui.nk_internal;

    NK_Wait();

    XGL_TexDestroy(internal_ctx->device.font);
    internal_ctx->device.font.handle = 0;

    XGL_VBOUnmap(internal_ctx->device.vbo);
    XGL_EBOUnmap(internal_ctx->device.ebo);

    XGL_VBODestroy(internal_ctx->device.vbo);
    XGL_EBODestroy(internal_ctx->device.ebo);

    if (internal_ctx->device.fence != 0) {
        glDeleteSync(internal_ctx->device.fence);
    }
    internal_ctx->device.fence = 0;

    XGL_VAODestroy(internal_ctx->device.vao);
}

internal void NK_Draw(enum nk_anti_aliasing aa) {
    ivec2s window_size = xtal_os->window_size;
    // TODO(geni): HiDPI support
    struct nk_vec2 scale = {1.0f, 1.0f};

    glViewport(0, 0, window_size.width, window_size.height);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);

    XGL_ShaderBind(renderer.shaders[XGL_SType_Nuklear]);
    XGL_VAOBind(game->ui.nk_internal.device.vao);
    XGL_ShaderUniformMat4(renderer.shaders[XGL_SType_Nuklear], XGL_SType_Nuklear_U_projection, &renderer.ortho_projection);
    {
        const struct nk_draw_command* cmd;
        nk_size                       offset = 0;
        struct nk_buffer              vbuf;
        struct nk_buffer              ebuf;

        NK_Wait();
        {
            local_persist const struct nk_draw_vertex_layout_element vertex_layout[] = {
                {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, offsetof(NK_Vertex, position)},
                {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, offsetof(NK_Vertex, uv)},
                {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, offsetof(NK_Vertex, color)},
                {NK_VERTEX_LAYOUT_END},
            };

            struct nk_convert_config config = {
                .vertex_layout        = vertex_layout,
                .vertex_size          = sizeof(NK_Vertex),
                .vertex_alignment     = alignof(NK_Vertex),
                .tex_null             = game->ui.nk_internal.device.null,
                .circle_segment_count = 22,
                .curve_segment_count  = 22,
                .arc_segment_count    = 22,
                .global_alpha         = 1.0f,
                .shape_AA             = aa,
                .line_AA              = aa,
            };

            nk_buffer_init_fixed(&vbuf, _nk_vertices, NK_MAX_VERTEX_MEMORY);
            nk_buffer_init_fixed(&ebuf, _nk_elements, NK_MAX_ELEMENT_MEMORY);
            nk_convert(&game->ui.nk_ctx, &game->ui.nk_internal.device.cmds, &vbuf, &ebuf, &config);
        }

        nk_draw_foreach(cmd, &game->ui.nk_ctx, &game->ui.nk_internal.device.cmds) {
            if (cmd->elem_count == 0) {
                continue;
            }

            glBindTexture(GL_TEXTURE_2D, cmd->texture.id);
            glScissor((GLint) (cmd->clip_rect.x * scale.x),
                      (GLint) ((window_size.height - (cmd->clip_rect.y + cmd->clip_rect.h)) * scale.y),
                      (GLsizei) (cmd->clip_rect.w * scale.x),
                      (GLsizei) (cmd->clip_rect.h * scale.y));
            glDrawElements(GL_TRIANGLES, (GLint) cmd->elem_count, GL_UNSIGNED_SHORT, (const void*) offset);
            offset += cmd->elem_count * sizeof(nk_draw_index);
        }
        nk_clear(&game->ui.nk_ctx);
        nk_buffer_clear(&game->ui.nk_internal.device.cmds);
    }

    XGL_ShaderUnbind();
    XGL_VAOUnbind();
    XGL_EBOUnbind();
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);

    NK_CreateFence();
}

internal i32 NK_HandleEvent(Xtal_OSEvent* event) {
    struct nk_context* ctx = &game->ui.nk_ctx;

    if (event->type == Xtal_OSEventType_KeyRelease || event->type == Xtal_OSEventType_KeyPress) {
        nk_bool down = event->type == Xtal_OSEventType_KeyPress;

        b32 ctrl_down = event->modifiers & KeyModifier_Ctrl;

        Key key = event->key;
        switch (key) {
            case Key_Shift: {
                nk_input_key(ctx, NK_KEY_SHIFT, down);
            } break;
            case Key_Delete: {
                nk_input_key(ctx, NK_KEY_DEL, down);
            } break;
            case Key_Enter: {
                nk_input_key(ctx, NK_KEY_ENTER, down);
            } break;
            case Key_Tab: {
                nk_input_key(ctx, NK_KEY_TAB, down);
            } break;
            case Key_Backspace: {
                nk_input_key(ctx, NK_KEY_BACKSPACE, down);
            } break;
            case Key_Home: {
                nk_input_key(ctx, NK_KEY_TEXT_START, down);
                nk_input_key(ctx, NK_KEY_SCROLL_START, down);
            } break;
            case Key_End: {
                nk_input_key(ctx, NK_KEY_TEXT_END, down);
                nk_input_key(ctx, NK_KEY_SCROLL_END, down);
            }
            case Key_PageDown: {
                nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
            }
            case Key_PageUp: {
                nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
            } break;
            case Key_Up: {
                nk_input_key(ctx, NK_KEY_UP, down);
            } break;
            case Key_Down: {
                nk_input_key(ctx, NK_KEY_DOWN, down);
            } break;
            case Key_Left: {
                if (ctrl_down) {
                    nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
                } else {
                    nk_input_key(ctx, NK_KEY_LEFT, down);
                }
            } break;
            case Key_Right: {
                if (ctrl_down) {
                    nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
                } else {
                    nk_input_key(ctx, NK_KEY_RIGHT, down);
                }
            } break;
        }

        if (ctrl_down) {
            switch (key) {
                case Key_Z: {
                    nk_input_key(ctx, NK_KEY_TEXT_UNDO, down);
                } break;
                case Key_R: {
                    nk_input_key(ctx, NK_KEY_TEXT_REDO, down);
                } break;
                case Key_C: {
                    nk_input_key(ctx, NK_KEY_COPY, down);
                } break;
                case Key_V: {
                    nk_input_key(ctx, NK_KEY_PASTE, down);
                } break;
                case Key_X: {
                    nk_input_key(ctx, NK_KEY_CUT, down);
                } break;
                case Key_B: {
                    nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down);
                } break;
                case Key_E: {
                    nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down);
                } break;
            }
        }

        return 1;
    } else if (event->type == Xtal_OSEventType_MousePress || event->type == Xtal_OSEventType_MouseRelease) {
        nk_bool down = event->type == Xtal_OSEventType_MousePress;

        int x = (int) xtal_os->mouse_position.x;
        int y = (int) xtal_os->mouse_position.y;

        if (event->key == MouseButton_Left) {
            // TODO(geni): Double click
            nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
        } else if (event->key == MouseButton_Right) {
            nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
        } else if (event->key == MouseButton_Middle) {
            nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
        }

        return 1;
    } else if (event->type == Xtal_OSEventType_MouseLeave) {
        nk_input_button(ctx, NK_BUTTON_LEFT, 0, 0, 0);
        nk_input_button(ctx, NK_BUTTON_RIGHT, 0, 0, 0);
    } else if (event->type == Xtal_OSEventType_MouseMove) {
        if (ctx->input.mouse.grabbed) {
            nk_input_motion(ctx, (int) (ctx->input.mouse.prev.x + event->delta.x), (int) (ctx->input.mouse.prev.y + event->delta.y));
        } else {
            nk_input_motion(ctx, (int) xtal_os->mouse_position.x, (int) xtal_os->mouse_position.y);
        }
        return 1;
    } else if (event->type == Xtal_OSEventType_CharacterInput) {
        if (event->character.data[0] < 32) {
            return 0;
        }

        nk_glyph glyph;
        Xtal_MemoryCopy(glyph, &event->character.data[0], NK_UTF_SIZE);
        nk_input_glyph(ctx, glyph);
        return 1;
    } else if (event->type == Xtal_OSEventType_MouseScroll) {
        nk_input_scroll(ctx, nk_vec2((float) event->delta.x, (float) event->delta.y));
        return 1;
    }

    return 0;
}

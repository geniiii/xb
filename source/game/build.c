#include <stdint.h>
#include <stdalign.h>
#include <string.h>

#include "xtal/ext/pcg.c"

#define CGLM_STATIC
#include "xtal/ext/cglm/cglm.h"
#include "xtal/ext/cglm/struct.h"

#include "xtal/ext/xxh/xxhash.c"

//~ xtal headers

#include "xtal/language_layer.h"
#include "xtal/math.c"
#include "xtal/memory.h"
#include "xtal/string8.h"
#include "xtal/string16.h"
#include "xtal/util/pool.h"
#include "xtal/os.h"
#include "xtal/opengl.h"
#include "xtal/util/serialize.h"

#include "xtal/gfx/gl4/types.h"
#include "xtal/gfx/gl4/texture.h"

//~ stb_sprintf

#define STB_SPRINTF_STATIC
#define STB_SPRINTF_IMPLEMENTATION
#include "xtal/ext/stb_sprintf.h"

//~ stb_rect_pack

#define STBRP_STATIC
#if !XTAL_DEBUG
#define STBRP_ASSERT(a) ((void) (a))
#endif
#define STB_RECT_PACK_IMPLEMENTATION
#include "xtal/ext/stb_rect_pack.h"

//~ stb_truetype

#define STBTT_STATIC
#if !XTAL_DEBUG
#define STBTT_ASSERT(a) ((void) (a))
#endif
#define STB_TRUETYPE_IMPLEMENTATION
#include "xtal/ext/stb_truetype.h"

//~ Nuklear

#define NK_PRIVATE
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_VSNPRINTF stbsp_vsnprintf
#define NK_MEMSET    Xtal_MemorySet
#define NK_MEMCPY    Xtal_MemoryCopy
#define NK_SIN       Sin
#define NK_COS       Cos
#if !XTAL_DEBUG
#define NK_ASSERT(a) ((void) (a))
#endif
#define NK_IMPLEMENTATION
#include "xtal/ext/nuklear.h"

//~ xtal implementations

#include "xtal/log.c"
#include "xtal/language_layer.c"
#include "xtal/memory.c"
#include "xtal/chars.c"
#include "xtal/cstrings.c"
#include "xtal/unicode.c"
#include "xtal/string8.c"
#include "xtal/os.c"

#include "xtal/util/bmp.c"
#include "xtal/util/rand.c"
#include "xtal/util/pool.c"
#include "xtal/util/dynamic_array.c"
#include "xtal/util/hashmap.c"
#include "xtal/util/ini_scanner.c"
#include "xtal/util/ini.c"
#include "xtal/util/ini_ast.c"

#include "xtal/gfx/gl4/shader.c"
#include "xtal/gfx/gl4/texture.c"
#include "xtal/gfx/gl4/vbo.c"
#include "xtal/gfx/gl4/vao.c"
#include "xtal/gfx/gl4/fbo.c"

#include "xtal/util/serialize.c"

//~ Game

#include "xtal/program_options.h"

#include "version.h"
#include "color.h"
#include "messages.h"
#include "nuklear_layer.h"
#include "input.c"
#include "states.h"
#include "renderer.h"
#include "gb/rom.h"
#include "gb/oam.h"
#include "gb/mbc.h"
#include "gb/ppu.h"
#include "gb.h"
#include "gb/memory.h"
#include "gb/timing.h"
#include "gb/opcodes.h"
#include "gb/interrupts.h"
#include "ui.h"
#include "game_context.h"

#include "messages.c"
#include "nuklear_layer.c"
#include "gb/mmu.c"
#include "gb/mbc.c"
#include "gb/memory.c"
#include "gb/interrupts.c"
#include "gb/ppu.c"
#include "gb/timing.c"
#include "gb.c"
#include "gb/opcodes.c"
#include "game_context.c"
#include "renderer.c"

#include "events.c"
#include "ui/panel.h"
#include "ui/panels/menubar.c"
#include "ui/panels/framebuffer.c"
#include "ui/panels/registers.c"
#include "ui/panels/speed.c"
#include "ui/panels/rom_info.c"
#include "ui/panels/mbc_registers.c"
#include "ui.c"
#include "state/game.c"
#include "states.c"
#include "app.c"

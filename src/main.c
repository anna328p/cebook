#include <tice.h>
#include <fileioc.h>
#include <graphx.h>
#include <debug.h>

#include "mcufont-decoder/mcufont.h"

#define MAX_LINES 15

struct state_t {
    uint16_t width;
    uint16_t height;
    uint16_t y;
    const struct mf_font_s *font;
    uint24_t charnum;
    uint8_t current_line;
};


typedef struct state_t state_t;

/* Callback to write to a memory buffer. */
static void pixel_callback(int16_t x, int16_t y, uint8_t count, uint8_t alpha, void *state) {
    while (count--) gfx_vbuffer[y][x++] = alpha >> 4;
}

/* Callback to render characters. */
static uint8_t character_callback(int16_t x, int16_t y, mf_char character, void *state) {
    state_t *s = (state_t*)state;
    s->charnum++;
    return mf_render_character(s->font, x, y, character, pixel_callback, state);
}

/* Callback to render lines. */
static bool line_callback(const char *line, uint16_t count, void *state) {
    state_t *s = (state_t*)state;

    if (s->current_line > MAX_LINES) return 0;

    dbg_sprintf(dbgout, "line %d\n", s->current_line);

    mf_render_justified(s->font, 10, s->y, s->width - 20,
            line, count, character_callback, state);
    s->current_line++;
    s->y += s->font->line_height;

    gfx_BlitBuffer();

    return true;
}

void fill_palette(void) {
    memset(gfx_palette, 0, 256);

    gfx_palette[0]  = gfx_RGBTo1555(243, 157, 101);
    gfx_palette[1]  = gfx_RGBTo1555(228, 144, 89);
    gfx_palette[2]  = gfx_RGBTo1555(214, 132, 77);
    gfx_palette[3]  = gfx_RGBTo1555(199, 119, 65);
    gfx_palette[4]  = gfx_RGBTo1555(185, 107, 54);
    gfx_palette[5]  = gfx_RGBTo1555(171, 95, 42);
    gfx_palette[6]  = gfx_RGBTo1555(157, 83, 31);
    gfx_palette[7]  = gfx_RGBTo1555(143, 71, 19);
    gfx_palette[8]  = gfx_RGBTo1555(129, 60, 6);
    gfx_palette[9]  = gfx_RGBTo1555(115, 49, 0);
    gfx_palette[10] = gfx_RGBTo1555(102, 38, 0);
    gfx_palette[11] = gfx_RGBTo1555(89, 26, 0);
    gfx_palette[12] = gfx_RGBTo1555(78, 14, 0);
    gfx_palette[13] = gfx_RGBTo1555(67, 2, 0);
    gfx_palette[14] = gfx_RGBTo1555(58, 0, 0);
    gfx_palette[15] = gfx_RGBTo1555(0, 0, 0);
}

#define CHUNK_SIZE 1024


/* Main function, called first */
int main(void) {
    struct mf_font_s const *font;
    struct mf_scaledfont_s scaledfont;
    struct state_t state;

    char const *text = "Welcome to CEbook!\n\nPress [MODE] to open the menu.";

    ti_var_t cur_book;
    char *book_text;
    int num_read = 0;
    
    ti_CloseAll();

    book_text = calloc(CHUNK_SIZE + 1, 1);
    cur_book = ti_Open("Alice", "r");

    dbg_sprintf(dbgout, "slot size %d pos %d\n",
            ti_GetSize(cur_book), ti_Tell(cur_book));

    num_read = ti_Read(book_text, CHUNK_SIZE, 1, cur_book); /* = 64 */
    dbg_sprintf(dbgout, "read %d chunks of %d bytes (%d)\n",
            num_read, CHUNK_SIZE, num_read * CHUNK_SIZE);

    dbg_sprintf(dbgout, book_text);

    gfx_Begin();
    gfx_SetDrawBuffer();

    fill_palette();

    gfx_FillScreen(0);
    gfx_BlitBuffer();

    gfx_SetColor(15);

    font = mf_find_font(mf_get_font_list()->font->short_name);

    state.width = 320;
    state.height = 240;
    state.y = 5;
    state.font = font;

    state.current_line = 0;
    state.charnum = 0;
    mf_wordwrap(font, 300, book_text + state.charnum, line_callback, &state);
    gfx_BlitBuffer();

    /* Waits for a keypress */
    while (!os_GetCSC());

    gfx_FillScreen(0);

    state.y = 5;
    state.current_line = 0;
    mf_wordwrap(font, 300, book_text + state.charnum, line_callback, &state);
    gfx_BlitBuffer();

    /* Waits for a keypress */
    while (!os_GetCSC());

    gfx_End();

    free(book_text);

    /* Return 0 for success */
    return 0;
}

/* vim: set ts=4 sw=4 et: */

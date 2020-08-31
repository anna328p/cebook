#include <tice.h>
#include <graphx.h>
#include <debug.h>

#include "mcufont-decoder/mcufont.h"

struct state_t {
    uint16_t width;
    uint16_t height;
    uint16_t y;
    const struct mf_font_s *font;
};


typedef struct state_t state_t;

/* Callback to write to a memory buffer. */
static void pixel_callback(int16_t x, int16_t y, uint8_t count, uint8_t alpha, void *state) {
    uint32_t pos;
    uint8_t *pixels = malloc(count);
    uint8_t cur_pix;
    uint8_t next_val;


    if (y < 0 || y >= 240) return;
    if (x < 0 || x + count >= 320) return;


    while (count--) {
        cur_pix = gfx_GetPixel(x, y);
        next_val = cur_pix + (alpha >> 4);
        if (next_val > 15) next_val = 15;
        gfx_SetColor(next_val);
        gfx_SetPixel(x, y);

        x++;
    }
}

/* Callback to render characters. */
static uint8_t character_callback(int16_t x, int16_t y, mf_char character,
        void *state)
{
    state_t *s = (state_t*)state;
    return mf_render_character(s->font, x, y, character, pixel_callback, state);
}

/* Callback to render lines. */
static bool line_callback(const char *line, uint16_t count, void *state)
{
    state_t *s = (state_t*)state;

    mf_render_justified(s->font, 10, s->y,
            s->width - 20,
            line, count, character_callback, state);
    s->y += s->font->line_height;
    return true;
}

/* Callback to just count the lines.
 * Used to decide the image height */
bool count_lines(const char *line, uint16_t count, void *state)
{
    int *linecount = (int*)state;
    (*linecount)++;
    return true;
}

void fill_palette(void) {
    memset(gfx_palette, 0, 256);

    dbg_sprintf(dbgout, "palette %d\n", gfx_RGBTo1555(112, 112, 112));
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

/* Main function, called first */
int main(void) {
    const struct mf_font_s *font;
    struct mf_scaledfont_s scaledfont;
    struct state_t state;

    const char *text = "I'd just like to interject for a moment. What you're referring to as TI-84, is in fact, Zilog eZ80 Toolchain/TI-84 Plus CE, or as I've recently taken to calling it, utter garbage";

    gfx_Begin();
    gfx_SetDrawBuffer();

    fill_palette();

    gfx_FillScreen(0);
    gfx_BlitBuffer();

    gfx_SetColor(15);

    font = mf_find_font(mf_get_font_list()->font->short_name);

    state.width = 320;
    state.height = 240;
    state.y = 2;
    state.font = font;

    mf_wordwrap(font, 320, text, line_callback, &state);
    gfx_BlitBuffer();

    /* Waits for a keypress */
    while (!os_GetCSC());

    gfx_End();

    /* Return 0 for success */
    return 0;
}

/* vim: set ts=4 sw=4 et: */

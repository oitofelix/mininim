/*
  video.c -- video module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mininim.h"

bool force_full_redraw;
ALLEGRO_DISPLAY *display;
ALLEGRO_BITMAP *uscreen, *iscreen;
ALLEGRO_TIMER *video_timer;
uint64_t bottom_text_timer;
int screen_flags;
int potion_flags;
/* bool hgc; */
ALLEGRO_BITMAP *cutscene_screen;
ALLEGRO_BITMAP *effect_buffer;
ALLEGRO_BITMAP *black_screen;
struct video_effect video_effect = {.type = VIDEO_NO_EFFECT};
ALLEGRO_FONT *builtin_font;
int display_width, display_height;
ALLEGRO_BITMAP *logo_icon;
int effect_counter;
void (*load_callback) (void);
int display_mode = -1;
bool about_screen;
ALLEGRO_BITMAP *oitofelix_face;
bool is_video_initialized = false;

static struct palette_cache {
  ALLEGRO_BITMAP *ib, *ob;
  palette pal;
  double time;
} *palette_cache;
static size_t palette_cache_nmemb;
static size_t palette_cache_size;
ssize_t palette_cache_size_limit = 12 * 1024 * 1024;

struct drawn_rectangle drawn_rectangle_stack[DRAWN_RECTANGLE_STACK_NMEMB_MAX];
size_t drawn_rectangle_stack_nmemb;

static struct clipping_rectangle clipping_rectangle_stack[CLIPPING_RECTANGLE_STACK_NMEMB_MAX];
static size_t clipping_rectangle_stack_nmemb;
bool ignore_clipping_rectangle_intersection;

void
init_video (void)
{
  if (! al_init_image_addon ())
    error (-1, 0, "%s (void): failed to initialize image addon",
            __func__);

  al_set_new_display_flags (al_get_new_display_flags ()
                            | (display_mode < 0 ? ALLEGRO_WINDOWED
                               : ALLEGRO_FULLSCREEN)
                            | ALLEGRO_RESIZABLE
                            | ALLEGRO_GENERATE_EXPOSE_EVENTS
                            | ALLEGRO_GTK_TOPLEVEL
                            | ALLEGRO_PROGRAMMABLE_PIPELINE);

  display_width = display_width ? display_width : DISPLAY_WIDTH;
  display_height = display_height ? display_height : DISPLAY_HEIGHT;

  if (display_mode >= 0) {
    ALLEGRO_DISPLAY_MODE d;
    al_get_display_mode (display_mode, &d);
    display_width = d.width;
    display_height = d.height;
    al_set_new_display_refresh_rate (d.refresh_rate);
    al_set_new_display_flags (al_get_new_display_flags ()
                              & ~ALLEGRO_FULLSCREEN_WINDOW);
  }

  al_set_new_display_option (ALLEGRO_SINGLE_BUFFER, 1, ALLEGRO_SUGGEST);

  display = al_create_display (display_width, display_height);
  if (! display)
    error (-1, 0, "%s (void): failed to initialize display", __func__);

  al_register_event_source (event_queue,
                            al_get_display_event_source (display));

  al_set_target_backbuffer (display);

  al_set_window_title (display, WINDOW_TITLE);
  logo_icon = load_bitmap (LOGO_ICON);
  if (logo_icon) al_set_display_icon (display, logo_icon);

  cutscene = true;

  main_menu_enabled = false;
  main_menu ();
  if (! is_fullscreen ()) show_menu ();

  if (global_mr.fit_w == 0 && global_mr.fit_h == 0) {
    global_mr.fit_w = 1;
    global_mr.fit_h = 1;
  }
  mr_set_dim (&global_mr, global_mr.fit_w, global_mr.fit_h);

  cutscene_screen = create_bitmap (CUTSCENE_WIDTH, CUTSCENE_HEIGHT);
  effect_buffer = create_bitmap (CUTSCENE_WIDTH, CUTSCENE_HEIGHT);
  black_screen = create_bitmap (CUTSCENE_WIDTH, CUTSCENE_HEIGHT);
  uscreen = create_bitmap (CUTSCENE_WIDTH, CUTSCENE_HEIGHT);
  iscreen = create_bitmap (display_width, display_height);
  clear_bitmap (uscreen, TRANSPARENT_COLOR);

  video_timer = al_create_timer (1.0 / EFFECT_HZ);

  al_register_event_source (event_queue,
                            al_get_timer_event_source (video_timer));

  al_init_font_addon ();
  builtin_font = al_create_builtin_font ();
  if (! builtin_font)
    error (0, 0, "%s (void): cannot create builtin font", __func__);

  if (! al_init_primitives_addon ())
    error (0, 0, "%s (void): failed to initialize primitives addon",
           __func__);

#if MACOSX_PORT
  /* workaround to make Mac OS X render the title screen cutscene
     properly */
  al_acknowledge_resize (display);
#endif

  /* workaround bug in which first access to clipboard fails */
  char *clipboard_text = al_get_clipboard_text (display);
  al_free (clipboard_text);

  is_video_initialized = true;
}

void
finalize_video (void)
{
  al_destroy_bitmap (logo_icon);
  al_destroy_bitmap (uscreen);
  al_destroy_bitmap (effect_buffer);
  al_destroy_bitmap (black_screen);
  al_destroy_font (builtin_font);
  al_destroy_timer (video_timer);
  al_destroy_display (display);
  al_shutdown_image_addon ();
  al_shutdown_font_addon ();
  al_shutdown_primitives_addon ();
}

int
memory_bitmap_flags (void)
{
  return
    (al_get_new_bitmap_flags ()
     & ~ALLEGRO_VIDEO_BITMAP
     & ~ALLEGRO_CONVERT_BITMAP)
    | ALLEGRO_MEMORY_BITMAP
    | ALLEGRO_MIN_LINEAR
    | ALLEGRO_MAG_LINEAR
    | ALLEGRO_MIPMAP;
}

int
video_bitmap_flags (void)
{
  return
    al_get_new_bitmap_flags ()
    & ~ALLEGRO_VIDEO_BITMAP
    & ~ALLEGRO_MEMORY_BITMAP
    & ~ALLEGRO_MIN_LINEAR
    & ~ALLEGRO_MAG_LINEAR
    & ~ALLEGRO_MIPMAP;
}

ALLEGRO_BITMAP *
create_memory_bitmap (int w, int h)
{
  int flags = al_get_new_bitmap_flags ();
  al_set_new_bitmap_flags (memory_bitmap_flags ());
  ALLEGRO_BITMAP *bitmap = al_create_bitmap (w, h);
  al_set_new_bitmap_flags (flags);
  return bitmap;
}

ALLEGRO_BITMAP *
create_bitmap (int w, int h)
{
  al_set_target_backbuffer (display);
  int flags = al_get_new_bitmap_flags ();
  al_set_new_bitmap_flags (video_bitmap_flags ());
  ALLEGRO_BITMAP *bitmap = al_create_bitmap (w, h);
  al_set_new_bitmap_flags (flags);
  /* validate_bitmap_for_windows (bitmap); */
  return bitmap;
}

ALLEGRO_BITMAP *
load_memory_bitmap (char *filename)
{
  int flags = al_get_new_bitmap_flags ();
  al_set_new_bitmap_flags (memory_bitmap_flags ());

  ALLEGRO_BITMAP *bitmap = (ALLEGRO_BITMAP *)
    load_resource (filename, (load_resource_f) al_load_bitmap, true);

  if (! bitmap)
    fprintf (stderr, "cannot load bitmap file '%s'\n", filename);

  al_set_new_bitmap_flags (flags);

  return bitmap;
}

ALLEGRO_BITMAP *
clone_memory_bitmap (ALLEGRO_BITMAP *bitmap)
{
  if (! bitmap) return NULL;
  int flags = al_get_new_bitmap_flags ();
  al_set_new_bitmap_flags (memory_bitmap_flags ());
  ALLEGRO_BITMAP *new_bitmap = al_clone_bitmap (bitmap);
  al_set_new_bitmap_flags (flags);
  return new_bitmap;
}

ALLEGRO_BITMAP *
load_scaled_memory_bitmap (char *filename, int w, int h, int flags)
{
  ALLEGRO_BITMAP *bitmap = load_memory_bitmap (filename);
  if (! bitmap) return NULL;
  ALLEGRO_BITMAP *scaled_bitmap =
    clone_scaled_memory_bitmap (bitmap, w, h, flags);
  al_destroy_bitmap (bitmap);
  return scaled_bitmap;
}

ALLEGRO_BITMAP *
clone_scaled_memory_bitmap (ALLEGRO_BITMAP *bitmap, int w, int h, int flags)
{
  ALLEGRO_BITMAP *scaled_bitmap = create_memory_bitmap (w, h);
  al_set_target_bitmap (scaled_bitmap);
#if WINDOWS_PORT
  /* White is the transparent mask for menu icons in Windows XP. */
  al_clear_to_color (IsWindowsVistaOrGreater ()
		     ? TRANSPARENT_COLOR
		     : WHITE);
#else
  al_clear_to_color (TRANSPARENT_COLOR);
#endif
  al_draw_scaled_bitmap (bitmap, 0, 0,
                         get_bitmap_width (bitmap),
                         get_bitmap_height (bitmap),
                         0, 0, w, h, flags);
  return scaled_bitmap;
}

ALLEGRO_BITMAP *
clone_scaled_bitmap (ALLEGRO_BITMAP *bitmap, int w, int h, int flags)
{
  ALLEGRO_BITMAP *scaled_bitmap = create_bitmap (w, h);
  al_set_target_bitmap (scaled_bitmap);
  al_clear_to_color (TRANSPARENT_COLOR);
  al_draw_scaled_bitmap (bitmap, 0, 0,
                         get_bitmap_width (bitmap),
                         get_bitmap_height (bitmap),
                         0, 0, w, h, flags);
  return scaled_bitmap;
}

ALLEGRO_BITMAP *
load_bitmap (const char *filename)
{
  int flags = al_get_new_bitmap_flags ();
  al_set_new_bitmap_flags (video_bitmap_flags ());

  al_set_target_backbuffer (display);

  ALLEGRO_BITMAP *bitmap = (ALLEGRO_BITMAP *)
    load_resource (filename, (load_resource_f) al_load_bitmap, true);

  al_set_new_bitmap_flags (flags);

  if (! bitmap)
    fprintf (stderr, "cannot load bitmap file '%s'\n", filename);

  /* validate_bitmap_for_windows (bitmap); */

  if (load_callback) load_callback ();

  return bitmap;
}

ALLEGRO_BITMAP *
trim_bitmap (ALLEGRO_BITMAP *b, ALLEGRO_COLOR t)
{
  int x0, y0, x1, y1, xl, yl, xr, yr;
  int w0 = get_bitmap_width (b);
  int h0 = get_bitmap_height (b);
  xl = w0;
  yl = h0;
  xr = 0;
  yr = 0;

  al_lock_bitmap (b, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
  for (y0 = 0; y0 < h0; y0++)
    for (x0 = 0; x0 < w0; x0++) {
      ALLEGRO_COLOR c = al_get_pixel (b, x0, y0);
      if (! color_eq (c, t)) {
        xl = min_int (xl, x0);
        yl = min_int (yl, y0);
        break;
      }
    }

  for (y1 = h0 - 1; y1 >= 0; y1--)
    for (x1 = w0 - 1; x1 >= 0; x1--) {
      ALLEGRO_COLOR c = al_get_pixel (b, x1, y1);
      if (! color_eq (c, t)) {
        xr  = max_int (xr, x1);
        yr  = max_int (yr, y1);
        break;
      }
    }
  al_unlock_bitmap (b);

  if (xl > xr) xl = xr = 0;
  if (yl > yr) yl = yr = 0;

  int w = xr - xl + 1;
  int h = yr - yl + 1;

  /* fprintf (stderr, "%i,%i,%i,%i\n", xl, yl, xr, yr); */
  /* fprintf (stderr, "%i,%i:%ix%i\n", xl, yl, w, h); */

  return al_create_sub_bitmap (b, xl, yl, w, h);
}

ALLEGRO_FONT *
load_font (const char *filename)
{
  int flags = al_get_new_bitmap_flags ();
  al_set_new_bitmap_flags (video_bitmap_flags ());

  al_set_target_backbuffer (display);

  ALLEGRO_FONT *font = (ALLEGRO_FONT *)
    load_resource (filename, (load_resource_f) al_load_bitmap_font, true);

  al_set_new_bitmap_flags (flags);

  if (! font)
    fprintf (stderr, "error: cannot load bitmap font file '%s'\n", filename);

  if (load_callback) load_callback ();

  return font;
}

static ALLEGRO_SHADER *target_shader;
static ALLEGRO_SHADER_TYPE target_shader_type;

static ALLEGRO_SHADER *
attach_shader_source_file_sub (const char *filename)
{
  if (! al_filename_exists (filename)) return NULL;
  else return al_attach_shader_source_file
         (target_shader, target_shader_type, filename)
         ? target_shader : NULL;
}

bool
attach_shader_source_file (ALLEGRO_SHADER *shader, const char *filename)
{
  target_shader = shader;
  target_shader_type = str_end_matches (filename, ".frag")
    ? ALLEGRO_PIXEL_SHADER : ALLEGRO_VERTEX_SHADER;

  ALLEGRO_SHADER *s = (ALLEGRO_SHADER *)
    load_resource (filename, (load_resource_f)
                   attach_shader_source_file_sub, true);

  return s ? true : false;
}

const char *
get_shader_platform (ALLEGRO_SHADER *s)
{
  if (s) {
    return (al_get_shader_platform (s) == ALLEGRO_SHADER_GLSL)
      ? "GLSL" : "HLSL";
  } else return "NONE";
}

/* Work around a bug in Windows, where bitmaps are loaded as
   black/transparent images. */

void
validate_bitmap_for_windows (ALLEGRO_BITMAP *bitmap)
{
#if WINDOWS_PORT
  if (! bitmap) return;
  al_lock_bitmap (bitmap,
		  ALLEGRO_PIXEL_FORMAT_ANY,
		  ALLEGRO_LOCK_READONLY);
  al_unlock_bitmap (bitmap);
#endif
}

ALLEGRO_BITMAP *
clone_bitmap (ALLEGRO_BITMAP *bitmap)
{
  int flags = al_get_new_bitmap_flags ();
  al_set_new_bitmap_flags (video_bitmap_flags ());
  al_set_target_backbuffer (display);
  ALLEGRO_BITMAP *new_bitmap = al_clone_bitmap (bitmap);
  al_set_new_bitmap_flags (flags);
  /* validate_bitmap_for_windows (new_bitmap); */
  return new_bitmap;
}

void
clear_bitmap (ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR color)
{
  al_set_target_bitmap (bitmap);
  al_clear_to_color (color);
}

void
convert_bitmap (ALLEGRO_BITMAP *bitmap, int flags)
{
  int old_flags = al_get_new_bitmap_flags ();
  al_set_new_bitmap_flags (flags);
  al_set_target_backbuffer (display);
  al_convert_bitmap (bitmap);
  al_set_new_bitmap_flags (old_flags);
}

ALLEGRO_BITMAP *
get_cached_palette (ALLEGRO_BITMAP *bitmap, palette p)
{
  if (! p) return NULL;

  struct palette_cache pc, *rpc;
  pc.ib = bitmap;
  pc.pal = p;

  rpc = bsearch (&pc, palette_cache, palette_cache_nmemb, sizeof (pc),
                 compare_palette_caches);

  if (rpc) return rpc->ob;
  else return NULL;
}

int
compare_palette_caches (const void *pc0, const void *pc1)
{
  struct palette_cache *p0 = (struct palette_cache *) pc0;
  struct palette_cache *p1 = (struct palette_cache *) pc1;

  if (p0->ib < p1->ib) return -1;
  else if (p0->ib > p1->ib) return 1;
  else if ((void *) p0->pal < (void *) p1->pal) return -1;
  else if ((void *) p0->pal > (void *) p1->pal) return 1;
  else return 0;
}

int
compare_palette_caches_by_time (const void *pc0, const void *pc1)
{
  struct palette_cache *p0 = (struct palette_cache *) pc0;
  struct palette_cache *p1 = (struct palette_cache *) pc1;

  if (p0->time < p1->time) return -1;
  else if (p0->time > p1->time) return 1;
  else return 0;
}

void
remove_palette_cache (lua_State *L, ssize_t i)
{
  palette_cache_size -= al_get_pixel_size (ALLEGRO_PIXEL_FORMAT_ANY)
    * get_bitmap_width (palette_cache[i].ob)
    * get_bitmap_height (palette_cache[i].ob);

  L_get_registry_by_ptr (L, palette_cache[i].ob);
  if (lua_isnil (L, -1)) {
    /* C bitmap */
    lua_pop (L, 1);
    al_destroy_bitmap (palette_cache[i].ob);
  } else {
    /* Lua bitmap */
    lua_pop (L, 1);
    lua_pushnil (L);
    L_set_registry_by_ptr (L, palette_cache[i].ob);
  }

  palette_cache =
    remove_from_array (palette_cache, &palette_cache_nmemb, i, 1,
                       sizeof (*palette_cache));
}

void
palette_cache_gc (lua_State *L)
{
  ssize_t i;
  for (i = 0; i < palette_cache_nmemb; i++) {
    L_get_weak_registry_by_ptr (L, palette_cache[i].ob);
    if (lua_isnil (L, -1)) {
      lua_pop (L, 1);
      remove_palette_cache (L, i--);
      continue;
    } else lua_pop (L, 1);

    L_get_weak_registry_by_ptr (L, palette_cache[i].pal);
    if (lua_isnil (L, -1)) {
      lua_pop (L, 1);
      remove_palette_cache (L, i--);
      continue;
    } else lua_pop (L, 1);
  }
}

void
enforce_palette_cache_limit (lua_State *L, ALLEGRO_BITMAP *b)
{
  if (palette_cache_size_limit < 0 || palette_cache_nmemb == 0) return;

  size_t additional_size = al_get_pixel_size (ALLEGRO_PIXEL_FORMAT_ANY)
    * get_bitmap_width (b) * get_bitmap_height (b);

  if (palette_cache_size + additional_size > palette_cache_size_limit) {

    palette_cache_gc (L);

    if (palette_cache_size + additional_size <= palette_cache_size_limit)
      return;

    qsort (palette_cache, palette_cache_nmemb, sizeof (*palette_cache),
           compare_palette_caches_by_time);

    while (palette_cache_size + additional_size > palette_cache_size_limit
           && palette_cache_nmemb > 0) remove_palette_cache (L, 0);

    qsort (palette_cache, palette_cache_nmemb, sizeof (*palette_cache),
           compare_palette_caches);
  }
}

ALLEGRO_BITMAP *
apply_palette (ALLEGRO_BITMAP *bitmap, palette p)
{
  return apply_palette_k (bitmap, p, p, NULL);
}

ALLEGRO_BITMAP *
apply_palette_k (ALLEGRO_BITMAP *bitmap, palette p, const void *k,
                 void *data)
{
  if (! bitmap) return NULL;

  lua_State *L;
  if (p == L_palette && data) L = data;
  else L = main_L;

  if (palette_cache_size_limit && k) {
    ALLEGRO_BITMAP *cached = get_cached_palette (bitmap, k);
    if (cached) return cached;

    enforce_palette_cache_limit (L, bitmap);
  }

  /* BEGIN: Palette caching guardian */
  if (k) {
    static uint64_t last_cycle = 0;
    static int consecutive_cycles = 0;

    if (last_cycle == anim_cycle - 1)
      consecutive_cycles++;
    else
      consecutive_cycles = 0;

    if (consecutive_cycles >= 120) {
      char *hl = hline ('-');
      fprintf (stderr,
               "%s\nWARNING: Palettes are being cached for each cycle.\n"
               "This is probably a result of mutable cache keys.\n"
               "This severely slows down MININIM and needlessly consumes"
               " memory.\n"
               "Please, consider fixing the %s video mode.\n",
               hl, video_mode);
      al_free (hl);
      consecutive_cycles = 0;
    }

    last_cycle = anim_cycle;
  } else {
    static uint64_t last_cycle = 0;
    static int consecutive_cycles = 0;

    if (last_cycle == anim_cycle - 1)
      consecutive_cycles++;
    else
      consecutive_cycles = 0;

    if (consecutive_cycles >= 120) {
      char *hl = hline ('-');
      fprintf (stderr,
               "%s\nWARNING: Palettes are being applied "
               "for each cycle without caching.\n"
               "This severely slows down MININIM.\n"
               "Please, consider fixing the %s video mode.\n",
               hl, video_mode);
      al_free (hl);
      consecutive_cycles = 0;
    }

    last_cycle = anim_cycle;
  }
  /* END: Palette caching guardian */

  /* fprintf (stderr, "%" PRIu64 ": paletted again!!!\n", anim_cycle); */

  /* Apply palette */
  al_lock_bitmap (bitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
  int w = get_bitmap_width (bitmap);
  int h = get_bitmap_height (bitmap);
  ALLEGRO_BITMAP *rbitmap = create_bitmap (w, h);
  al_lock_bitmap (rbitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
  al_set_target_bitmap (rbitmap);
  for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++)
      al_put_pixel (x, y, p (al_get_pixel (bitmap, x, y), data));
  al_unlock_bitmap (rbitmap);
  al_unlock_bitmap (bitmap);

  if (palette_cache_size_limit && k) {
    /* In case it's Lua bitmap and palette */
    if (p == L_palette) {
      /* put palette into weak registry for GC tracking */
      lua_pushvalue (L, -1);
      L_set_weak_registry_by_ptr (L, (void *) k);

      /* put output bitmap into registry to prevent GC collection */
      L_pushbitmap (L, rbitmap);
      L_set_registry_by_ptr (L, rbitmap);
    }

    struct palette_cache pc;
    pc.ib = bitmap;
    pc.pal = k;
    pc.ob = rbitmap;
    pc.time = al_get_time ();

    palette_cache_size += al_get_pixel_size (ALLEGRO_PIXEL_FORMAT_ANY)
      * w * h;

    palette_cache =
      add_to_array (&pc, 1, palette_cache, &palette_cache_nmemb,
                    palette_cache_nmemb, sizeof (pc));

    qsort (palette_cache, palette_cache_nmemb, sizeof (pc),
           compare_palette_caches);
  }

  return rbitmap;
}

bool
bitmap_heq (ALLEGRO_BITMAP *b0, ALLEGRO_BITMAP *b1)
{
  al_lock_bitmap (b0, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
  al_lock_bitmap (b1, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);

  int b0_w = get_bitmap_width (b0);
  int b0_h = get_bitmap_height (b0);
  int b0_m = min_int (b0_w, b0_h);

  int b1_w = get_bitmap_width (b1);
  int b1_h = get_bitmap_height (b1);
  int b1_m = min_int (b1_w, b1_h);

  int l = min_int (b0_m, b1_m);

  int i;
  for (i = 0; i < l; i++) {
    ALLEGRO_COLOR c0 = al_get_pixel (b0, i, i);
    ALLEGRO_COLOR c1 = al_get_pixel (b1, i, i);
    if (! color_eq (c0, c1)) {
      al_unlock_bitmap (b0);
      al_unlock_bitmap (b1);
      return false;
    }
  }

  al_unlock_bitmap (b0);
  al_unlock_bitmap (b1);
  return true;
}

bool
color_eq (ALLEGRO_COLOR c0, ALLEGRO_COLOR c1)
{
  unsigned char r0, g0, b0, a0, r1, g1, b1, a1;
  al_unmap_rgba (c0, &r0, &g0, &b0, &a0);
  al_unmap_rgba (c1, &r1, &g1, &b1, &a1);
  return r0 == r1 && g0 == g1 && b0 == b1 && a0 == a1;
}

void
convert_mask_to_alpha (ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR mask_color)
{
  al_convert_mask_to_alpha (bitmap, mask_color);
}

int
get_bitmap_width (ALLEGRO_BITMAP *bitmap)
{
  return bitmap ? al_get_bitmap_width (bitmap) : 0;
}

int
get_bitmap_height (ALLEGRO_BITMAP *bitmap)
{
  return bitmap ? al_get_bitmap_height (bitmap) : 0;
}

void
draw_bitmap (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
             float dx, float dy, int flags)
{
  draw_bitmap_region (from, to, 0, 0,
                      get_bitmap_width (from),
                      get_bitmap_height (from),
                      dx, dy, flags);
}

void
draw_bitmap_region (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                    float sx, float sy, float sw, float sh,
                    float dx, float dy, int flags)
{
  if (! from || ! is_video_rendering ())
    return;

  merge_drawn_rectangle (to, dx, dy, sw, sh);
  al_set_target_bitmap (to);

  int cx, cy, cw, ch;
  al_get_clipping_rectangle (&cx, &cy, &cw, &ch);

  if (cw <= 0 || ch <= 0) return;

  al_draw_bitmap_region (from, sx, sy, sw, sh, dx, dy, flags);
}

void
draw_rectangle (ALLEGRO_BITMAP *to, float x1, float y1,
                float x2, float y2, ALLEGRO_COLOR color,
                float thickness)
{
  if (! is_video_rendering ())
    return;
  al_set_target_bitmap (to);
  al_draw_rectangle (x1 + 1, y1, x2 + 1, y2, color, thickness);
}

void
draw_filled_rectangle (ALLEGRO_BITMAP *to, float x1, float y1,
                       float x2, float y2, ALLEGRO_COLOR color)
{
  if (! is_video_rendering ())
    return;
  al_set_target_bitmap (to);
  al_draw_filled_rectangle (x1, y1, x2 + 1, y2 + 1, color);
}

void
draw_text (ALLEGRO_BITMAP *bitmap, ALLEGRO_FONT *font,
           ALLEGRO_COLOR c, float x, float y, int flags,
           char const *text)
{
  al_set_target_bitmap (bitmap);
  al_draw_text (font, c, x, y, flags | ALLEGRO_ALIGN_INTEGER, text);
}

bool
draw_bottom_text (ALLEGRO_BITMAP *bitmap, char *text, int priority)
{
  if (! is_video_rendering ())
    return false;

  static char *current_text = NULL;
  static int cur_priority = INT_MIN;
  static int text_offset = 0;
  static int dir = 0;
  static uint64_t cycle = 0;

  if (bitmap == NULL && priority < cur_priority
      && bottom_text_timer < BOTTOM_TEXT_DURATION)
    return false;

  if (text) {
    if (current_text) {
      if (strcmp (text, current_text)) {
        text_offset = 0;
        dir = 0;
        cycle = 0;
      }
      al_free (current_text);
    }
    current_text = xasprintf ("%s", text);
    bottom_text_timer = 1;
    cur_priority = priority;
  } else if (bottom_text_timer > BOTTOM_TEXT_DURATION
             || ! bitmap) {
    bottom_text_timer = 0;
    cur_priority = INT_MIN;
  } else if (bottom_text_timer) {
    if (strlen (current_text) > BOTTOM_TEXT_MAX_LENGTH) {
      if (cycle++ % 2) {
        if (dir % 2) {
          if (strlen (current_text + text_offset) > BOTTOM_TEXT_MAX_LENGTH)
            text_offset++;
          else
            dir = (dir + 1) % 2;
        } else {
          if (text_offset > 0)
            text_offset--;
          else
            dir = (dir + 1) % 2;
        }
      }
    }

    char str[BOTTOM_TEXT_MAX_LENGTH + 1];
    strncpy (str, current_text + text_offset, BOTTOM_TEXT_MAX_LENGTH);
    str[BOTTOM_TEXT_MAX_LENGTH] = '\0';

    push_reset_clipping_rectangle (bitmap);

    bool r = draw_object_part (bitmap, "TEXT", str, NULL);

    if (! r) {
      int x = 0;
      int y = REAL_HEIGHT - al_get_font_line_height (builtin_font);
      int w = REAL_WIDTH - 1;
      int h = REAL_HEIGHT - 1;
      draw_filled_rectangle (bitmap, x, y, w, h, BLACK);
      draw_text (bitmap, builtin_font, WHITE, REAL_WIDTH / 2.0, y + 1,
                 ALLEGRO_ALIGN_CENTRE, str);
    }

    pop_clipping_rectangle ();

  }

  return true;
}

void
save_bitmap (char *filename, ALLEGRO_BITMAP *bitmap)
{
  if (! al_save_bitmap (filename, bitmap))
    error (0, 0, "%s: cannot save bitmap file '%s'",
           __func__, filename);
}

ALLEGRO_COLOR
hgc_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, al_map_rgb (85, 255, 255)))
    return al_map_rgb (170, 170, 170);
  if (color_eq (c, al_map_rgb (255, 85, 255)))
    return al_map_rgb (85, 85, 85);
  return c;
}

void
draw_mr_select_rect (struct mr *mr, int x, int y, ALLEGRO_COLOR color,
                     int flags)
{
  if (0 > x || x >= mr->w || 0 > y || y >= mr->h)
    return;

  int rx = x, ry = y;
  if (flags & ALLEGRO_FLIP_HORIZONTAL)
    rx = (mr->w - 1) - x;
  if (flags & ALLEGRO_FLIP_VERTICAL)
    ry = (mr->h - 1) - y;

  int w = al_get_display_width (display);
  int h = al_get_display_height (display);
  int tw, th; mr_get_resolution (mr, &tw, &th);
  tw = OW (tw);
  th = OH (th);

  ALLEGRO_BITMAP *screen = mr->cell[rx][ry].screen;
  int sw = get_bitmap_width (screen);
  int sh = get_bitmap_height (screen);
  float dx = ((OW (ORIGINAL_WIDTH) * rx) * w) / (float) tw;
  float dy = ((OH (ROOM_HEIGHT) * ry) * h) / (float) th;
  float dw = (sw * w) / (float) tw;
  float dh = (sh * h) / (float) th;
  al_draw_rectangle (dx, dy, dx + dw, dy + dh, color, 2);
}

void
draw_room_select_rect (struct mr *mr, int room, ALLEGRO_COLOR color,
                       int flags)
{
  int x, y;
  for (y = mr->h - 1; y >= 0; y--)
    for (x = 0; x < mr->w; x++)
      if (mr_coord_room (mr, x, y) == room)
        draw_mr_select_rect (mr, x, y, color, flags);
}

void
flip_display (struct mr *mr, ALLEGRO_BITMAP *bitmap)
{
  int w = al_get_display_width (display);
  int h = al_get_display_height (display);

  int flags;

  if (! is_video_rendering ()
      || about_screen
      || load_callback) flags = 0;
  else flags = screen_flags | potion_flags;

  if (bitmap) {
    int bw = get_bitmap_width (bitmap);
    int bh = get_bitmap_height (bitmap);
    al_set_target_backbuffer (display);
    al_draw_scaled_bitmap
      (bitmap, 0, 0, bw, bh, 0, 0, w, h, flags);
  } else {
    if (has_mr_view_changed (mr)
        && ! no_room_drawing) {
      mr_draw (mr);
      force_full_redraw = true;
    }

    int iw = get_bitmap_width (iscreen);
    int ih = get_bitmap_height (iscreen);

    if (iw != w || ih != h) {
      al_destroy_bitmap (iscreen);
      iscreen = create_bitmap (w, h);
      /* iscreen = clone_bitmap (al_get_backbuffer (display)); */
    }

    al_set_target_bitmap (iscreen);

    int x, y;
    int tw, th;
    mr_get_resolution (mr, &tw, &th);
    tw = OW (tw);
    th = OH (th);

    for (y = mr->h - 1; y >= 0; y--)
      for (x = 0; x < mr->w; x++) {
        ALLEGRO_BITMAP *screen =
          (mr_coord_room (mr, x, y) || no_room_drawing)
          ? mr->cell[x][y].screen : mr->cell[x][y].cache;
        int sw = get_bitmap_width (screen);
        int sh = get_bitmap_height (screen);
        float dx = ((OW (ORIGINAL_WIDTH) * x) * w) / (float) tw;
        float dy = ((OH (ROOM_HEIGHT) * y) * h) / (float) th;
        float dw = (sw * w) / (float) tw;
        float dh = (sh * h) / (float) th;

        if (mr_coord_room (mr, x, y)
            || mr->last.display_width != w
            || mr->last.display_height != h
            || force_full_redraw)
          al_draw_scaled_bitmap
            (screen, 0, 0, sw, sh, dx, dy, dw, dh, 0);
      }

    al_set_target_backbuffer (display);
    al_draw_bitmap (iscreen, 0, 0, flags);

    if (edit != EDIT_NONE && (mr->w > 1 || mr->h > 1)) {
      for (int y = mr->h - 1; y >= 0; y--)
        for (int x = 0; x < mr->w; x++)
          if (is_room_in_sel_ring
              (&global_sel_ring, mr_coord_room (mr, x, y)))
            draw_room_select_rect (mr, mr_coord_room (mr, x, y), YELLOW, flags);

      if (selection_locked && is_valid_pos (&selection_pos)) {
        struct pos p; npos (&selection_pos, &p);
        draw_room_select_rect (mr, p.room, RED, flags);
      }

      if (is_valid_pos (&mouse_pos)) {
        struct pos p; npos (&mouse_pos, &p);
        draw_room_select_rect (mr, p.room, BLUE, flags);
      }
    }

    if ((mr->w > 1 || mr->h > 1) && mr->room_select > 0)
      draw_room_select_rect (mr, mr->room_select, MAGENTA, flags);

    if (mr->room != mr->last.room
        || mr->x != mr->last.x
        || mr->y != mr->last.y
        || mr->w != mr->last.w
        || mr->h != mr->last.h)
      mr->select_cycles = SELECT_CYCLES;

    if ((mr->w > 1 || mr->h > 1)
        && mr->select_cycles > 0) {
      draw_mr_select_rect (mr, mr->x, mr->y, GREEN, flags);
      if (! pause_anim) mr->select_cycles--;
    }
  }

  if (! about_screen && ! load_callback) {
    int uw = get_bitmap_width (uscreen);
    int uh = get_bitmap_height (uscreen);

    al_draw_scaled_bitmap (uscreen, 0, 0, uw, uh, 0, 0, w, h, 0);
  }

  al_flip_display ();

  force_full_redraw = false;
}

int
bool2bitmap_flags (bool v, bool h)
{
  return (v ? ALLEGRO_FLIP_VERTICAL : 0)
    | (h ? ALLEGRO_FLIP_HORIZONTAL : 0);
}

void
draw_fade (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to, float factor)
{
  factor = factor < 0 ? 0 : factor;
  clear_bitmap (black_screen, al_map_rgba_f (0, 0, 0, factor));
  draw_bitmap (from, to, 0, 0, 0);
  draw_bitmap (black_screen, to, 0, 0, 0);
}

void
draw_roll_right (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                 int total, int i)
{
  int w = get_bitmap_width (from);
  int h = get_bitmap_height (from);
  float slice =  w / total;
  draw_bitmap_region (from, to, 0, 0, i * slice, h, 0, 0, 0);
}

void
draw_shutter (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
              int total, int i)
{
  int sw = get_bitmap_width (from);
  int sh = get_bitmap_height (from);

  int sy;
  for (sy = 0; sy < sh; sy += total)
    draw_bitmap_region (from, to, 0, sy, sw, i, 0, sy, 0);
}

void
draw_pattern (ALLEGRO_BITMAP *bitmap, int ox, int oy, int w, int h,
              ALLEGRO_COLOR color_0, ALLEGRO_COLOR color_1)
{
  int x, y;
  al_set_target_bitmap (bitmap);
  al_lock_bitmap (bitmap, ALLEGRO_PIXEL_FORMAT_ANY,
                  ALLEGRO_LOCK_READWRITE);
  for (y = oy; y < oy + h; y++)
    for (x = ox; x < ox + w; x++)
      al_put_pixel (x, y, (x % 2 != y % 2) ? color_0 : color_1);
  al_unlock_bitmap (bitmap);
}

void
start_video_effect (enum video_effect_type type, int duration)
{
  video_effect.type = type;
  video_effect.duration = duration;
  clear_bitmap (effect_buffer, BLACK);
  clear_bitmap (black_screen, BLACK);
  draw_bitmap (cutscene_screen, effect_buffer, 0, 0, 0);
  al_start_timer (video_timer);
}

void
stop_video_effect (void)
{
  if (! al_get_timer_started (video_timer)) return;
  video_effect.type = VIDEO_NO_EFFECT;
  al_stop_timer (video_timer);
  al_set_timer_count (video_timer, 0);
  drop_all_events_from_source
    (event_queue, al_get_timer_event_source (video_timer));
  clear_bitmap (cutscene_screen, BLACK);
  draw_bitmap (effect_buffer, cutscene_screen, 0, 0, 0);
  effect_counter = 0;
}

bool
is_video_effect_started (void)
{
  return al_get_timer_started (video_timer);
}

void
show (void)
{
  if (! is_video_initialized)
    return;

  if (load_callback)
    {
      show_logo ("Loading...",
		 current_resource_filename,
		 NULL);
      return;
    }
  else if (is_dedicatedly_replaying ())
    {
      show_logo_replaying ();
      return;
    }
  else if (! command_line_replay
	   && ! is_video_rendering ())
    {
      char *text1 = is_audio_rendering () ? "AUDIO" : "NONE";
      show_logo ("RENDERING", text1, NULL);
      return;
    }
  else if (about_screen)
    {
      show_logo ("http://oitofelix.github.io/mininim/",
		 "http://forum.princed.org/", oitofelix_face);
      return;
    }

  switch (video_effect.type) {
  case VIDEO_NO_EFFECT:
    flip_display (&global_mr, cutscene ? cutscene_screen : NULL); return;
  case VIDEO_OFF: return;
  default: break;
  }

  if (! pause_anim) {
    if (++effect_counter >= video_effect.duration + 2) {
      effect_counter = 0;
      stop_video_effect ();
      return;
    }

    switch (video_effect.type) {
    case VIDEO_FLICKERING:
      if (effect_counter % 2 && effect_counter < video_effect.duration) {
        clear_bitmap (effect_buffer, video_effect.color);
        convert_mask_to_alpha (cutscene_screen, BLACK);
      } else clear_bitmap (effect_buffer, BLACK);
      draw_bitmap (cutscene_screen, effect_buffer, 0, 0, 0);
      break;
    case VIDEO_FADE_IN:
      /* switch (vm) { */
      /* case CGA: case EGA: */
      /*   draw_shutter (cutscene_screen, effect_buffer, video_effect.duration / 4, effect_counter); */
      /*   if (effect_counter >= video_effect.duration / 4) */
      /*     effect_counter += video_effect.duration; */
      /*   break; */
      /* case VGA: */
        draw_fade (cutscene_screen, effect_buffer, 1 - (float) effect_counter
                   / (float) video_effect.duration);
      /*   break; */
      /* } */
      break;
    case VIDEO_FADE_OUT:
      /* switch (vm) { */
      /* case CGA: case EGA: */
      /*   draw_shutter (black_screen, effect_buffer, video_effect.duration / 4, */
      /*                 effect_counter); */
      /*   if (effect_counter >= video_effect.duration / 4) */
      /*     effect_counter += video_effect.duration; */
      /*   break; */
      /* case VGA: */
        draw_fade (cutscene_screen, effect_buffer, (float) effect_counter
                   / (float) video_effect.duration);
        /* break; */
      /* } */
      if (effect_counter + 1 >= video_effect.duration) clear_bitmap (effect_buffer, BLACK);
      break;
    case VIDEO_ROLL_RIGHT:
      draw_roll_right (cutscene_screen, effect_buffer, video_effect.duration, effect_counter);
      break;
    default: assert (false); break;
    }
  }

  flip_display (&global_mr, effect_buffer);
}

bool
is_fullscreen (void)
{
  return (al_get_display_flags (display) & ALLEGRO_FULLSCREEN_WINDOW)
    || (al_get_display_flags (display) & ALLEGRO_FULLSCREEN);
}

void
process_display_events (void)
{
  ALLEGRO_EVENT event;
  while (al_get_next_event (event_queue, &event))
    switch (event.type) {
    case ALLEGRO_EVENT_DISPLAY_EXPOSE:
      show ();
      break;
    case ALLEGRO_EVENT_DISPLAY_RESIZE:
      al_acknowledge_resize (display);
      show ();
      break;
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
      exit (EXIT_SUCCESS);
      break;
    }
}

bool
intersection_rectangle (int x0, int y0, int w0, int h0,
                        int x1, int y1, int w1, int h1,
                        int *xrp, int *yrp, int *wrp, int *hrp)
{
  if (w0 <= 0 || h0 <= 0 || w1 <= 0 || h1 <= 0) {
    *xrp = 0;
    *yrp = 0;
    *wrp = 0;
    *hrp = 0;
    return false;
  }

  int xr = max_int (x0, x1);
  int yr = max_int (y0, y1);

  int x0b = x0 + w0 - 1;
  int y0b = y0 + h0 - 1;
  int x1b = x1 + w1 - 1;
  int y1b = y1 + h1 - 1;

  int xrb = min_int (x0b, x1b);
  int yrb = min_int (y0b, y1b);

  int wr = xrb - xr + 1;
  int hr = yrb - yr + 1;

  *xrp = xr;
  *yrp = yr;
  *wrp = wr > 0 ? wr : 0;
  *hrp = hr > 0 ? hr : 0;

  return wr > 0 && hr > 0;
}

bool
union_rectangle (int x0, int y0, int w0, int h0,
                 int x1, int y1, int w1, int h1,
                 int *xrp, int *yrp, int *wrp, int *hrp)
{
  if ((w0 <= 0 || h0 <= 0) && (w1 > 0 && h1 > 0)) {
    *xrp = x1;
    *yrp = y1;
    *wrp = w1;
    *hrp = h1;
    return true;
  } else if ((w1 <= 0 || h1 <= 0) && (w0 > 0 && h0 > 0)) {
    *xrp = x0;
    *yrp = y0;
    *wrp = w0;
    *hrp = h0;
    return true;
  } else if ((w0 <= 0 || h0 <= 0) && (w1 <= 0 && h1 <= 0)) {
    *xrp = 0;
    *yrp = 0;
    *wrp = 0;
    *hrp = 0;
    return false;
  }

  int xr = min_int (x0, x1);
  int yr = min_int (y0, y1);

  int x0b = x0 + w0 - 1;
  int y0b = y0 + h0 - 1;
  int x1b = x1 + w1 - 1;
  int y1b = y1 + h1 - 1;

  int xrb = max_int (x0b, x1b);
  int yrb = max_int (y0b, y1b);

  int wr = xrb - xr + 1;
  int hr = yrb - yr + 1;

  *xrp = xr;
  *yrp = yr;
  *wrp = wr > 0 ? wr : 0;
  *hrp = hr > 0 ? hr : 0;

  return wr > 0 && hr > 0;
}

struct drawn_rectangle *
push_drawn_rectangle (ALLEGRO_BITMAP *bitmap)
{
  assert (drawn_rectangle_stack_nmemb < DRAWN_RECTANGLE_STACK_NMEMB_MAX);

  /* push onto stack */
  size_t i = drawn_rectangle_stack_nmemb;
  drawn_rectangle_stack[i].bitmap = bitmap;
  drawn_rectangle_stack[i].x = 0;
  drawn_rectangle_stack[i].y = 0;
  drawn_rectangle_stack[i].w = 0;
  drawn_rectangle_stack[i].h = 0;
  drawn_rectangle_stack_nmemb++;

  return &drawn_rectangle_stack[i];
}

struct drawn_rectangle *
get_drawn_rectangle (ALLEGRO_BITMAP *bitmap)
{
  int i;
  for (i = drawn_rectangle_stack_nmemb - 1;
       i >= 0 && drawn_rectangle_stack[i].bitmap != bitmap; i--);
  if (i < 0) return NULL;
  return &drawn_rectangle_stack[i];
}

struct drawn_rectangle *
merge_drawn_rectangle (ALLEGRO_BITMAP *bitmap, int x, int y, int w, int h)
{
  struct drawn_rectangle *dr = get_drawn_rectangle (bitmap);
  if (! dr) return NULL;
  union_rectangle (dr->x, dr->y, dr->w, dr->h,
                   x, y, w, h,
                   &dr->x, &dr->y, &dr->w, &dr->h);
  return dr;
}

void
adjust_drawn_rectangle (ALLEGRO_BITMAP *bitmap, int dx, int dy, int dw, int dh)
{
  struct drawn_rectangle *dr = get_drawn_rectangle (bitmap);
  if (! dr) return;
  dr->x += dx;
  dr->y += dy;
  dr->w += dw;
  dr->h += dh;
}

struct drawn_rectangle *
pop_drawn_rectangle (void)
{
  assert (drawn_rectangle_stack_nmemb > 0);
  drawn_rectangle_stack_nmemb--;
  return &drawn_rectangle_stack[drawn_rectangle_stack_nmemb];
}

void
push_clipping_rectangle (ALLEGRO_BITMAP *bitmap, int x, int y, int w, int h)
{
  assert (clipping_rectangle_stack_nmemb < CLIPPING_RECTANGLE_STACK_NMEMB_MAX);

  al_set_target_bitmap (bitmap);

  /* save current */
  int cx, cy, cw, ch;
  al_get_clipping_rectangle (&cx, &cy, &cw, &ch);
  size_t i = clipping_rectangle_stack_nmemb;
  clipping_rectangle_stack[i].bitmap = bitmap;
  clipping_rectangle_stack[i].x = cx;
  clipping_rectangle_stack[i].y = cy;
  clipping_rectangle_stack[i].w = cw;
  clipping_rectangle_stack[i].h = ch;
  clipping_rectangle_stack_nmemb++;

  /* intersection */
  if (! ignore_clipping_rectangle_intersection)
    intersection_rectangle (x, y, w, h,
                            cx, cy, cw, ch,
                            &x, &y, &w, &h);

  /* set new */
  al_set_clipping_rectangle (x, y, w, h);
}

void
push_reset_clipping_rectangle (ALLEGRO_BITMAP *bitmap)
{
  int w = bitmap ? get_bitmap_width (bitmap) : 0;
  int h = bitmap ? get_bitmap_height (bitmap) : 0;
  push_clipping_rectangle (bitmap, 0, 0, w, h);
}

bool
merge_clipping_rectangle (ALLEGRO_BITMAP *bitmap, int x, int y, int w, int h)
{
  assert (clipping_rectangle_stack_nmemb > 0);

  int i;
  for (i = clipping_rectangle_stack_nmemb - 1;
       i >= 0 && clipping_rectangle_stack[i].bitmap != bitmap; i--);
  if (i < 0) return false;

  al_set_target_bitmap (clipping_rectangle_stack[i].bitmap);

  /* get current */
  int cx, cy, cw, ch;
  al_get_clipping_rectangle (&cx, &cy, &cw, &ch);

  /* union */
  int xr, yr, wr, hr;
  union_rectangle (x, y, w, h,
                   cx, cy, cw, ch,
                   &xr, &yr, &wr, &hr);

  /* intersection */
  if (! ignore_clipping_rectangle_intersection) {
    int xs = clipping_rectangle_stack[i].x;
    int ys = clipping_rectangle_stack[i].y;
    int ws = clipping_rectangle_stack[i].w;
    int hs = clipping_rectangle_stack[i].h;
    intersection_rectangle (xr, yr, wr, hr,
                            xs, ys, ws, hs,
                            &xr, &yr, &wr, &hr);
  }

  /* set new */
  al_set_clipping_rectangle (xr, yr, wr, hr);
  return true;
}

void
pop_clipping_rectangle (void)
{
  assert (clipping_rectangle_stack_nmemb > 0);

  size_t i = clipping_rectangle_stack_nmemb - 1;
  ALLEGRO_BITMAP *bitmap = clipping_rectangle_stack[i].bitmap;
  int x = clipping_rectangle_stack[i].x;
  int y = clipping_rectangle_stack[i].y;
  int w = clipping_rectangle_stack[i].w;
  int h = clipping_rectangle_stack[i].h;
  al_set_target_bitmap (bitmap);
  al_set_clipping_rectangle (x, y, w, h);
  clipping_rectangle_stack_nmemb--;
}

void
draw_logo (ALLEGRO_BITMAP *bitmap,
	   const char *text0,
	   const char *text1,
           ALLEGRO_BITMAP *icon)
{
  int x = 145;
  int y = 40;
  int w = get_bitmap_width (logo_icon);
  int h = get_bitmap_height (logo_icon);

  push_reset_clipping_rectangle (bitmap);

  clear_bitmap (bitmap, BLACK);

  draw_filled_rectangle (bitmap, x - 1, y - 1, x + w, y + h, WHITE);
  draw_bitmap (logo_icon, bitmap, x, y, 0);

  if (text0)
    draw_text (bitmap, builtin_font,
               WHITE, CUTSCENE_WIDTH / 2.0, CUTSCENE_HEIGHT / 2.0,
               ALLEGRO_ALIGN_CENTRE, text0);

  if (text1)
    draw_text (bitmap, builtin_font,
               WHITE, CUTSCENE_WIDTH / 2.0,
               CUTSCENE_HEIGHT / 2.0 + 16, ALLEGRO_ALIGN_CENTRE,
               text1);

  if (icon)
    draw_bitmap (icon, bitmap, (CUTSCENE_WIDTH / 2.0)
                 - (get_bitmap_width (icon) / 2.0) - 2, 136, 0);

  al_draw_filled_rectangle (0, CUTSCENE_HEIGHT - 8,
                            CUTSCENE_WIDTH, CUTSCENE_HEIGHT,
                            BLUE);
  draw_text (bitmap, builtin_font,
             WHITE, CUTSCENE_WIDTH / 2.0,
             CUTSCENE_HEIGHT - 7,
             ALLEGRO_ALIGN_CENTRE,
             "MININIM " VERSION);

  pop_clipping_rectangle ();
}

void
show_logo_replaying (void)
{
  char *text0, *text1;
  int progress; update_replay_progress (&progress);
  text0 = xasprintf ("Level: %02u", global_level.n);
  text1 = xasprintf ("Replaying: %3u%%", progress);

  show_logo (text0, text1, NULL);

  al_free (text0);
  al_free (text1);
}

void
show_logo (const char *text0, const char *text1, ALLEGRO_BITMAP *icon)
{
  enum rendering rendering_backup = rendering;
  rendering |= VIDEO_RENDERING;
  draw_logo (cutscene_screen, text0, text1, icon);
  rendering = rendering_backup;
  flip_display (&global_mr, cutscene_screen);
}

void
load_oitofelix_face (void)
{
  oitofelix_face = load_bitmap (OITOFELIX_FACE);
}

void
unload_oitofelix_face (void)
{
  al_destroy_bitmap (oitofelix_face);
}

lua_Number
IW (lua_Number w)
{
  return (w * ORIGINAL_WIDTH) / REAL_WIDTH;
}

lua_Number
IH (lua_Number h)
{
  return (h * ORIGINAL_HEIGHT) / REAL_HEIGHT;
}

lua_Number
OW (lua_Number w)
{
  return (w * REAL_WIDTH) / ORIGINAL_WIDTH;
}

lua_Number
OH (lua_Number h)
{
  return (h * REAL_HEIGHT) / ORIGINAL_HEIGHT;
}

bool
is_video_rendering ()
{
  return rendering & VIDEO_RENDERING;
}

void
video_rendering (bool enable)
{
  if (enable)
    rendering |= VIDEO_RENDERING;
  else
    rendering &= ~VIDEO_RENDERING;
}

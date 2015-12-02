/*
  video.c -- video module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

#include <stdio.h>
#include <error.h>

#include "video.h"
#include "event.h"
#include "timer.h"

ALLEGRO_DISPLAY *display;
ALLEGRO_BITMAP *screen;
ALLEGRO_BITMAP *flick_bg_0, *flick_bg_1;
ALLEGRO_TIMER *video_timer;
static ALLEGRO_BITMAP *effect_buffer;
static ALLEGRO_BITMAP *memory_bitmap;
struct video_effect video_effect = {.type = VIDEO_NO_EFFECT};
static ALLEGRO_FONT *builtin_font;

void
init_video (void)
{
  if (! al_init_image_addon ())
    error (-1, 0, "%s (void): failed to initialize image addon",
           __func__);

  al_set_new_display_flags (ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE |
                            ALLEGRO_GENERATE_EXPOSE_EVENTS);

  display = al_create_display (DISPLAY_WIDTH, DISPLAY_HEIGHT);
  if (! display) error (-1, 0, "%s (void): failed to initialize display", __func__);

  screen = create_bitmap (ORIGINAL_WIDTH, ORIGINAL_HEIGHT);
  effect_buffer = create_bitmap (ORIGINAL_WIDTH, ORIGINAL_HEIGHT);

  int flags = al_get_new_bitmap_flags ();
  al_add_new_bitmap_flag (ALLEGRO_MEMORY_BITMAP);
  memory_bitmap = create_bitmap (ORIGINAL_WIDTH, ORIGINAL_HEIGHT);
  al_set_new_bitmap_flags (flags);

  video_timer = create_timer (1.0 / EFFECT_HZ);

  al_init_font_addon ();
  builtin_font = al_create_builtin_font ();
  if (! builtin_font)
    error (-1, 0, "%s (void): cannot create builtin font", __func__);

  if (! al_init_primitives_addon ())
    error (-1, 0, "%s (void): failed to initialize primitives addon",
           __func__);
}

void
finalize_video (void)
{
  al_destroy_bitmap (screen);
  al_destroy_bitmap (effect_buffer);
  al_destroy_bitmap (memory_bitmap);
  al_destroy_font (builtin_font);
  al_destroy_timer (video_timer);
  al_destroy_display (display);
  al_shutdown_image_addon ();
  al_shutdown_font_addon ();
  al_shutdown_primitives_addon ();
}

int
max (int a, int b)
{
  return (a > b) ? a : b;
}

int
min (int a, int b)
{
  return (a < b) ? a : b;
}

ALLEGRO_EVENT_SOURCE *
get_display_event_source (ALLEGRO_DISPLAY *display)
{
  ALLEGRO_EVENT_SOURCE *event_source = al_get_display_event_source (display);
  if (! event_source)
    error (-1, 0, "%s: failed to get display event source (%p)",
           __func__,  display);
  return event_source;
}

ALLEGRO_BITMAP *
create_bitmap (int w, int h)
{
  ALLEGRO_BITMAP *bitmap = al_create_bitmap (w, h);
  if (! bitmap) error (-1, 0, "%s (%i, %i): cannot create bitmap", __func__, w, h);
  return bitmap;
}

ALLEGRO_BITMAP *
clone_bitmap (ALLEGRO_BITMAP *bitmap)
{
  ALLEGRO_BITMAP *new_bitmap = al_clone_bitmap (bitmap);
  if (! new_bitmap) error (-1, 0, "%s (%p): cannot clone bitmap", __func__, bitmap);
  return new_bitmap;
}

void
clear_bitmap (ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR color)
{
  al_set_target_bitmap (bitmap);
  al_clear_to_color (color);
}

ALLEGRO_BITMAP *
apply_palette (ALLEGRO_BITMAP *bitmap, palette p)
{
  int x, y;
  ALLEGRO_BITMAP *rbitmap = clone_bitmap (bitmap);
  int w = al_get_bitmap_width (bitmap);
  int h = al_get_bitmap_height (bitmap);
  al_lock_bitmap (rbitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
  al_set_target_bitmap (rbitmap);
  for (y = 0; y < h; y++)
    for (x = 0; x < w; x++)
      al_put_pixel (x, y, p (al_get_pixel (rbitmap, x, y)));
  al_unlock_bitmap (rbitmap);
  return rbitmap;
}

bool
color_eq (ALLEGRO_COLOR c0, ALLEGRO_COLOR c1)
{
  unsigned char r0, g0, b0, r1, g1, b1;
  al_unmap_rgb (c0, &r0, &g0, &b0);
  al_unmap_rgb (c1, &r1, &g1, &b1);
  return r0 == r1 && g0 == g1 && b0 == b1;
}

void
draw_bitmap (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to, float dx, float dy, int flags)
{
  al_set_target_bitmap (to);
  al_draw_bitmap (from, dx, dy, flags);
}

void
draw_bitmap_region (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to, float sx, float sy,
                    float sw, float sh, float dx, float dy, int flags)
{
  al_set_target_bitmap (to);
  al_draw_bitmap_region (from, sx, sy, sw, sh, dx, dy, flags);
}

void
draw_text (ALLEGRO_BITMAP *bitmap, char const *text, float x, float y, int flags)
{
  al_set_target_bitmap (bitmap);
  al_draw_text (builtin_font, WHITE, x, y, flags, text);
}

void
draw_bottom_text (char const *text)
{
  al_draw_filled_rectangle (0, ORIGINAL_HEIGHT - 8,
                            ORIGINAL_WIDTH, ORIGINAL_HEIGHT,
                            al_map_rgba (0, 0, 0, 192));
  draw_text (screen, text,
             ORIGINAL_WIDTH / 2.0, ORIGINAL_HEIGHT - 7,
             ALLEGRO_ALIGN_CENTRE);
}

ALLEGRO_BITMAP *
load_bitmap (char *filename)
{
  ALLEGRO_BITMAP *bitmap = al_load_bitmap (filename);
  if (! bitmap)
    error (-1, 0, "%s: cannot load bitmap file '%s'", __func__, filename);
  return bitmap;
}

static void
flip_display (ALLEGRO_BITMAP *bitmap)
{
  int w = al_get_display_width (display);
  int h = al_get_display_height (display);

  al_set_target_backbuffer (display);
  al_draw_scaled_bitmap (bitmap, 0, 0, ORIGINAL_WIDTH, ORIGINAL_HEIGHT,
                         0, 0, w, h, 0);
  al_flip_display ();
}

void
acknowledge_resize (void)
{
  if (! al_acknowledge_resize (display))
    error (-1, 0, "%s: cannot acknowledge display resize (%p)", __func__, display);
}

void
draw_fade (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to, float factor)
{
  al_set_target_bitmap (to);
  al_draw_tinted_bitmap
    (from, al_map_rgba_f (factor, factor, factor, 1.0), 0, 0, 0);
}

void
draw_roll_right (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                 int total, int i)
{
  int w = al_get_bitmap_width (from);
  int h = al_get_bitmap_height (from);

  float slice =  w / total;

  draw_bitmap_region (from, to, (i - 1) * slice, 0, i * slice, h,
                      (i - 1) * slice, 0, 0);
}

void
draw_pattern (ALLEGRO_BITMAP *bitmap, int ox, int oy, int w, int h,
              ALLEGRO_COLOR color_0, ALLEGRO_COLOR color_1)
{
  int x, y;
  draw_bitmap (bitmap, memory_bitmap, 0, 0, 0);
  al_set_target_bitmap (memory_bitmap);
  al_lock_bitmap (memory_bitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
  for (y = oy; y < oy + h; y++)
    for (x = ox; x < ox + w; x++)
      al_put_pixel (x, y, (x % 2 != y % 2) ? color_0 : color_1);
  al_unlock_bitmap (memory_bitmap);
  draw_bitmap (memory_bitmap, bitmap, 0, 0, 0);
}

void
start_video_effect (enum video_effect_type type, int duration)
{
  video_effect.type = type;
  video_effect.duration = duration;
  clear_bitmap (effect_buffer, BLACK);
  draw_bitmap (screen, effect_buffer, 0, 0, 0);
  al_start_timer (video_timer);
}

bool
is_video_effect_started (void)
{
  return al_get_timer_started (video_timer);
}

void
show (void)
{
  static int i = 0;

  switch (video_effect.type) {
  case VIDEO_NO_EFFECT: flip_display (screen); return;
  case VIDEO_OFF: return;
  default: break;
  }

  if (++i >= video_effect.duration) {
    i = 0;
    video_effect.type = VIDEO_NO_EFFECT;
    clear_bitmap (screen, BLACK);
    draw_bitmap (effect_buffer, screen, 0, 0, 0);
    al_stop_timer (video_timer);
    return;
  }

  switch (video_effect.type) {
  case VIDEO_FLICKERING:
    if (i % 2) clear_bitmap (effect_buffer, video_effect.color);
    else clear_bitmap (effect_buffer, BLACK);
    al_convert_mask_to_alpha (screen, BLACK);
    draw_bitmap (screen, effect_buffer, 0, 0, 0);
    break;
  case VIDEO_FADE_IN:
    draw_fade (screen, effect_buffer, (float) i / (float) video_effect.duration);
    break;
  case VIDEO_FADE_OUT:
    draw_fade (screen, effect_buffer, 1 - (float) i / (float) video_effect.duration);
    if (i + 1 >= video_effect.duration) clear_bitmap (effect_buffer, BLACK);
    break;
  case VIDEO_ROLL_RIGHT:
    draw_roll_right (screen, effect_buffer, video_effect.duration, i);
    break;
  default:
    error (-1, 0, "%s (void): unknown video effect type (%i)",
           __func__, video_effect.type);
  }

  flip_display (effect_buffer);
}

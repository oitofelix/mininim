/*
  video.h -- video module;

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

#ifndef MININIM_VIDEO_H
#define MININIM_VIDEO_H

/* functions */
void init_video (void);
void finalize_video (void);
ALLEGRO_EVENT_SOURCE *get_display_event_source (ALLEGRO_DISPLAY *display);
void destroy_bitmap (ALLEGRO_BITMAP *bitmap);
ALLEGRO_BITMAP *create_bitmap (int w, int h);
ALLEGRO_BITMAP *clone_bitmap (ALLEGRO_BITMAP *bitmap);
ALLEGRO_BITMAP *load_bitmap (char *filename);
void validate_bitmap_for_mingw (ALLEGRO_BITMAP *bitmap);
void save_bitmap (char *filename, ALLEGRO_BITMAP *bitmap);
bool color_eq (ALLEGRO_COLOR c0, ALLEGRO_COLOR c1);
void set_target_bitmap (ALLEGRO_BITMAP *bitmap);
void set_target_backbuffer (ALLEGRO_DISPLAY *display);
void clear_bitmap (ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR color);
void draw_bitmap (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                  float dx, float dy, int flags);
void draw_bitmap_region (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                         float sx, float sy, float sw, float sh,
                         float dx, float dy, int flags);
void draw_filled_rectangle (ALLEGRO_BITMAP *to, float x1, float y1,
                            float x2, float y2, ALLEGRO_COLOR color);
void draw_rectangle (ALLEGRO_BITMAP *to, float x1, float y1,
                     float x2, float y2, ALLEGRO_COLOR color,
                     float thickness);
void draw_text (ALLEGRO_BITMAP *bitmap, char const *text, float x, float y, int flags);
void draw_bottom_text (ALLEGRO_BITMAP *bitmap, char *text, int priority);
void acknowledge_resize (void);
void draw_fade (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to, float factor);
void draw_roll_right (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                      int total, int current);
void draw_shutter (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                   int total, int i);
void draw_pattern (ALLEGRO_BITMAP *bitmap, int ox, int oy, int w, int h,
                   ALLEGRO_COLOR color_0, ALLEGRO_COLOR color_1);
void start_video_effect (enum video_effect_type type, int duration);
void stop_video_effect (void);
bool is_video_effect_started (void);
void show (void);
void flip_display (ALLEGRO_BITMAP *bitmap);
bool is_fullscreen (void);
void process_display_events (void);

/* palette */
int compare_palette_caches (const void *pc0, const void *pc1);
ALLEGRO_BITMAP *apply_palette (ALLEGRO_BITMAP *bitmap, palette p);
ALLEGRO_BITMAP *get_cached_palette (ALLEGRO_BITMAP *bitmap, palette p);
ALLEGRO_COLOR hgc_palette (ALLEGRO_COLOR c);

/* variables */
extern bool force_full_redraw;
extern ALLEGRO_DISPLAY *display;
extern ALLEGRO_BITMAP *uscreen, *iscreen;
extern ALLEGRO_BITMAP *effect_buffer;
extern ALLEGRO_BITMAP *black_screen;
extern ALLEGRO_TIMER *video_timer;
extern struct video_effect video_effect;
extern int screen_flags;
extern bool hgc;
extern int display_width, display_height;
extern ALLEGRO_BITMAP *icon;
extern ALLEGRO_TIMER *bottom_text_timer;
extern bool is_display_focused;
extern int effect_counter;
extern void (*load_callback) (void);

#endif	/* MININIM_VIDEO_H */

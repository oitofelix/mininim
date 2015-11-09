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

#ifndef FREEPOP_GRAPHICS_H
#define FREEPOP_GRAPHICS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "event.h"
#include "engine/pos.h"

/* types */
enum video_effect {
  VIDEO_NO_EFFECT, VIDEO_OFF, VIDEO_FADE_IN,
  VIDEO_FADE_OUT, VIDEO_ROLL_RIGHT, VIDEO_FLICKERING,
};

/* functions */
void init_video (void);
void finalize_video (void);
int max (int a, int b);
int min (int a, int b);
ALLEGRO_EVENT_SOURCE *get_display_event_source (ALLEGRO_DISPLAY *display);
ALLEGRO_BITMAP *create_bitmap (int w, int h);
ALLEGRO_BITMAP *clone_bitmap (ALLEGRO_BITMAP *bitmap);
ALLEGRO_BITMAP *load_bitmap (char *filename);
void clear_bitmap (ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR color);
void draw_bitmap (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                  float dx, float dy, int flags);
void draw_bitmap_region (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                         float sx, float sy, float sw, float sh,
                         float dx, float dy, int flags);
void draw_text (ALLEGRO_BITMAP *bitmap, char const *text, float x, float y, int flags);
void acknowledge_resize (void);
void draw_fade (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to, float factor);
void draw_roll_right (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                      int total, int current);
void draw_pattern (ALLEGRO_BITMAP *bitmap, int ox, int oy, int w, int h,
                   ALLEGRO_COLOR color_0, ALLEGRO_COLOR color_1);
void start_video_effect (enum video_effect type, int duration);
bool is_video_effect_started (void);
void show (void);


/* variables */
extern ALLEGRO_DISPLAY *display;
extern ALLEGRO_BITMAP *screen;
extern ALLEGRO_TIMER *video_timer;
extern ALLEGRO_BITMAP *flick_bg_0, *flick_bg_1;

#endif	/* FREEPOP_VIDEO_H */

/*
  room.h -- room module;

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

#ifndef MININIM_ROOM_H
#define MININIM_ROOM_H

/* variables */
extern bool no_recursive_links_continuity;

/* functions */
void load_room (void);
void unload_room (void);

void draw_bitmapc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                   struct coord *c, int flags);
void draw_bitmap_regionc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                          float sx, float sy, float sw, float sh,
                          struct coord *c, int flags);

struct rect *new_rect (struct rect *r, int room, int x, int y, int w, int h);
void draw_filled_rect (ALLEGRO_BITMAP *to, struct rect *r,
                       ALLEGRO_COLOR color);
void clear_rect_to_color (ALLEGRO_BITMAP *to, struct rect *r,
                          ALLEGRO_COLOR color);

void draw_room (ALLEGRO_BITMAP *bitmap, int room,
                enum em em, enum vm vm);
void draw_conbg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm, bool redraw);
void draw_confg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm, bool redraw);
void draw_confg_no_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm, bool redraw);
void draw_confg_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm);
void draw_confg_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm, bool redraw);
void draw_confg_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                       enum em em, enum vm vm, bool redraw);
void draw_confg_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm, bool redraw);
void draw_room_anim_fg (ALLEGRO_BITMAP *bitmap,
                        enum em em, enum vm vm, struct anim *a);
void draw_confg_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    enum em em, enum vm vm, struct frame *f);
void draw_room_frame_fg (ALLEGRO_BITMAP *bitmap, enum em em,
                         enum vm vm, struct frame *f);
/* void draw_room_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, */
/*                    enum em em, enum vm vm, struct frame *f); */

ALLEGRO_BITMAP *apply_hue_palette (ALLEGRO_BITMAP *bitmap);
ALLEGRO_COLOR apply_hue_color (ALLEGRO_COLOR c);
ALLEGRO_COLOR selection_palette (ALLEGRO_COLOR c);
void draw_no_floor_selection (ALLEGRO_BITMAP *bitmap, struct pos *p);

#endif	/* MININIM_ROOM_H */

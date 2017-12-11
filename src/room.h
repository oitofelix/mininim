/*
  room.h -- room module;

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

#ifndef MININIM_ROOM_H
#define MININIM_ROOM_H

/* variables */
extern bool no_recursive_links_continuity;
extern ALLEGRO_BITMAP *room0;
extern bool tile_caching;
extern int room_view;

/* functions */
void draw_bitmapc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                   struct coord *c, int flags);
void draw_bitmap_regionc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                          lua_Number sx, lua_Number sy,
                          lua_Number sw, lua_Number sh,
                          struct coord *c, int flags);
void draw_bitmap_region_o (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                           lua_Number sx, lua_Number sy,
                           lua_Number sw, lua_Number sh,
                           lua_Number dx, lua_Number dy, int flags);

void draw_filled_rect (ALLEGRO_BITMAP *to, struct rect *r,
                       ALLEGRO_COLOR color);
void clear_rect_to_color (ALLEGRO_BITMAP *to, struct rect *r,
                          ALLEGRO_COLOR color);

void redraw_drawn_rectangle (struct drawn_rectangle *dr, struct pos *p);

void draw_room (ALLEGRO_BITMAP *bitmap, int room);
void draw_tile (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_tile_bg (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_tile_fg (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_room_actor_front (ALLEGRO_BITMAP *bitmap, struct actor *a);
void draw_tile_fg_front (ALLEGRO_BITMAP *bitmap, struct pos *p,
                         struct frame *f);
void draw_room_frame_front (ALLEGRO_BITMAP *bitmap, struct frame *f);
/* void draw_room_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, */
/*                    struct frame *f); */
ALLEGRO_BITMAP *get_tile_bitmap (struct tile *tile_ref,
                                 union tile_state *ts,
                                 float scale, enum tile_part part);

ALLEGRO_BITMAP *apply_hue_palette (ALLEGRO_BITMAP *bitmap);
ALLEGRO_COLOR apply_hue_color (ALLEGRO_COLOR c);
ALLEGRO_COLOR selection_palette (ALLEGRO_COLOR c);

void update_room0_cache (void);
void update_cache (struct mr *mr);
void update_cache_pos (struct mr *mr, struct pos *p);
void register_changed_pos (struct pos *p);
struct pos *get_changed_pos (struct pos *p);
struct pos *get_changed_pos_by_room (int room);
void optimize_changed_pos (void);
void remove_changed_pos (struct pos *pos);
void register_changed_room (int room);
bool has_room_changed (int room);

#endif	/* MININIM_ROOM_H */

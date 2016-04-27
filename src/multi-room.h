/*
  multi-room.h -- multi-room module;

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

#ifndef MININIM_MULTI_ROOM_H
#define MININIM_MULTI_ROOM_H

/* variables */
extern ALLEGRO_BITMAP *room0;
extern ALLEGRO_COLOR room0_wall_color[3][4][11];
extern bool con_caching;
extern struct multi_room mr;
extern int mr_room, mr_x, mr_y;
extern bool mr_view_changed;

/* functions */
bool is_room_visible (int room);
bool is_kid_visible (void);
bool has_mr_view_changed (void);
bool set_multi_room (int w, int h);
void mr_get_resolution (int *w, int *h);
void mr_map_rooms (void);
bool mr_topmost_cell (int *rx, int *ry);
bool mr_leftmost_cell (int *rx, int *ry);
bool mr_bottommost_cell (int *rx, int *ry);
bool mr_rightmost_cell (int *rx, int *ry);
void mr_center_room (int room);
void mr_select_trans (enum dir d);
void mr_view_trans (enum dir d);
void mr_view_page_trans (enum dir d);
void draw_multi_rooms (void);
void nmr_coord (int x, int y, int *rx, int *ry);
bool mr_coord (int room0, enum dir dir, int *rx, int *ry);
bool ui_set_multi_room (int dw, int dh);
void update_room0_cache (enum em em, enum vm vm);
void update_cache (enum em em, enum vm vm);
void update_cache_pos (struct pos *p, enum em em, enum vm vm);
void register_changed_pos (struct pos *p);
struct pos *get_changed_pos (struct pos *p);
void mr_update_last_settings (void);

#endif	/* MININIM_MULTI_ROOM_H */

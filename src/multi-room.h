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
extern int room_view;
extern int mr_room, mr_x, mr_y;

/* visibility functions */
bool is_pos_visible (struct pos *p);
bool is_frame_visible_at_room (struct frame *f, int room);
bool is_frame_visible (struct frame *f);
bool is_room_visible (int room);

/* functions */
bool has_mr_view_changed (void);
void redim_multi_room (int w, int h);
void create_multi_room_bitmaps (void);
void set_multi_room (int w, int h);
void mr_get_resolution (int *w, int *h);
bool mr_topmost_cell (int *rx, int *ry);
bool mr_leftmost_cell (int *rx, int *ry);
bool mr_bottommost_cell (int *rx, int *ry);
bool mr_rightmost_cell (int *rx, int *ry);
struct mr_origin *mr_save_origin (struct mr_origin *o);
void mr_restore_origin (struct mr_origin *o);
void mr_map_room (int r, int x, int y);
void mr_set_origin (int room, int x, int y);
void mr_center_room (int room);
void mr_focus_room (int room);
void mr_focus_cell (int x, int y);
void mr_focus_mouse (void);
void mr_select_trans (enum dir d);
void mr_view_trans (enum dir d);
void mr_view_page_trans (enum dir d);
void draw_multi_rooms (void);
void nmr_coord (int x, int y, int *rx, int *ry);
bool mr_coord (int room0, enum dir dir, int *rx, int *ry);
bool ui_set_multi_room (int dw, int dh);
void update_room0_cache (enum em em, enum vm vm);
void update_cache (enum em em, enum vm vm);
void update_cache_pos (struct pos *p, enum changed_pos_reason reason,
                       enum em em, enum vm vm);
int c_changed_pos (struct changed_pos *cp0, struct changed_pos *cp1);
void register_changed_pos (struct pos *p, enum changed_pos_reason reason);
struct changed_pos *get_changed_pos (struct changed_pos *cp);
void register_changed_room (int room);
bool has_room_changed (int room);
void mr_update_last_settings (void);
void multi_room_fit_stretch (void);
void multi_room_fit_ratio (void);
void apply_mr_fit_mode (void);
bool mr_room_list_has_room (struct mr_room_list *l, int room);
struct mr_room_list *mr_get_room_list (struct mr_room_list *l);
void mr_destroy_room_list (struct mr_room_list *l);
int mr_count_uniq_rooms (void);
void mr_stabilize_origin (struct mr_origin *o, enum dir d);

#endif	/* MININIM_MULTI_ROOM_H */

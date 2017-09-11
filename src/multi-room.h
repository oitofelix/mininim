/*
  multi-room.h -- multi-room module;

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

#ifndef MININIM_MULTI_ROOM_H
#define MININIM_MULTI_ROOM_H

/* variables */
extern ALLEGRO_BITMAP *room0;
extern bool tile_caching;
extern struct mr global_mr;
extern int room_view;
extern int mr_room, mr_x, mr_y;
extern struct rect_sel global_rect_sel;

/* visibility functions */
bool is_pos_visible (struct mr *mr, struct pos *p);
bool is_frame_visible_at_room (struct frame *f, int room);
bool is_frame_visible (struct mr *mr, struct frame *f);
bool is_room_visible (struct mr *mr, int room);

/* functions */
bool has_mr_view_changed (struct mr *mr);
void mr_redim (struct mr *mr, int w, int h);
void mr_create_bitmaps (struct mr *mr);
void mr_set_dim (struct mr *mr, int w, int h);
void mr_get_resolution (struct mr *mr, int *w, int *h);
bool mr_topmost_cell (struct mr *mr, int *rx, int *ry);
bool mr_leftmost_cell (struct mr *mr, int *rx, int *ry);
bool mr_bottommost_cell (struct mr *mr, int *rx, int *ry);
bool mr_rightmost_cell (struct mr *mr, int *rx, int *ry);
struct mr_origin *mr_save_origin (struct mr *mr, struct mr_origin *o);
bool mr_origin_eq (struct mr_origin *a, struct mr_origin *b);
void mr_restore_origin (struct mr *mr, struct mr_origin *o);
void mr_map_room (struct mr *mr, int r, int x, int y);
void mr_set_origin (struct mr *mr, int room, int x, int y);
void mr_center_room (struct mr *mr, int room);
void mr_simple_center_room (struct mr *mr, int room);
void mr_focus_room (struct mr *mr, int room);
void mr_focus_cell (struct mr *mr, int x, int y);
void mr_focus_mouse (struct mr *mr);
void mr_scroll_into_view (struct mr *mr, int room);
void mr_room_trans (struct mr *mr, enum dir d);
void mr_row_trans (struct mr *mr, enum dir d);
void mr_page_trans (struct mr *mr, enum dir d);
void draw_multi_rooms (struct mr *mr);
void nmr_coord (struct mr *mr, int x, int y, int *rx, int *ry);
bool mr_coord (struct mr *mr, int room0, enum dir dir, int *rx, int *ry);
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
void mr_update_last_settings (struct mr *mr);
void multi_room_fit_stretch (struct mr *mr);
void multi_room_fit_ratio (struct mr *mr);
void apply_mr_fit_mode (struct mr *mr);
bool mr_room_list_has_room (struct mr_room_list *l, int room);
struct mr_room_list *mr_get_room_list (struct mr *mr, struct mr_room_list *l);
void mr_destroy_room_list (struct mr_room_list *l);
int mr_count_uniq_rooms (struct mr *mr);
void mr_stabilize_origin (struct mr *mr, struct mr_origin *o);
void mr_busy (struct mr *mr);

/* rectangular selection */
struct rect_sel *new_rect_sel (struct mr *mr,
                               struct rect_sel *rs,
                               struct pos *a,
                               struct pos *b);
bool is_valid_rect_sel (struct rect_sel *rs);
void destroy_rect_sel (struct rect_sel *rs);
bool is_room_in_rect_sel (struct rect_sel *rs, int _room);
bool is_pos_in_rect_sel (struct rect_sel *rs, struct pos *p);

#endif	/* MININIM_MULTI_ROOM_H */

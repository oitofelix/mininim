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
extern struct mr global_mr;

/* destruction */
void destroy_mr (struct mr *mr);

/* dimension */
void mr_set_dim (struct mr *mr, int w, int h);
void apply_mr_fit_mode (struct mr *mr, enum mr_fit_mode mode);

/* query info */
void mr_get_resolution (struct mr *mr, int *w, int *h);
int mr_central_room (struct mr *mr);

/* view */
void mr_set_origin (struct mr *mr, int room, int x, int y,
                    struct room_linking *rlink, size_t room_nmemb);
void mr_set_room (struct mr *mr, int room, int x, int y);
void mr_best_view (struct mr *mr, int room);
void mr_center_room (struct mr *mr, int room);

void mr_focus_room (struct mr *mr, int room);
void mr_focus_mouse (struct mr *mr);
void mr_scroll_into_view (struct mr *mr, int room);


/* visibility functions */
bool is_pos_visible (struct mr *mr, struct pos *p);
bool is_frame_visible_at_room (struct frame *f, int room);
bool is_frame_visible (struct mr *mr, struct frame *f);
bool is_room_visible (struct mr *mr, int room);

/* functions */
bool has_mr_view_changed (struct mr *mr);
struct mr_origin *mr_save_origin (struct mr *mr, struct mr_origin *o);
bool mr_origin_eq (struct mr_origin *a, struct mr_origin *b);
void mr_restore_origin (struct mr *mr, struct mr_origin *o);
bool mr_is_view_within_bounds (struct mr *mr);
void mr_room_trans (struct mr *mr, enum dir d);
void mr_row_trans (struct mr *mr, enum dir d);
void mr_page_trans (struct mr *mr, enum dir d);
void mr_draw (struct mr *mr);

bool mr_is_coord_visible (struct mr *mr, int x, int y);
bool mr_room_coord (struct mr *mr, int room, int *rx, int *ry);
int mr_coord_room (struct mr *mr, int x, int y);

void mr_update_last_settings (struct mr *mr);
bool mr_room_list_has_room (struct mr_room_list *l, int room);
struct mr_room_list *mr_get_room_list (struct mr *mr, struct mr_room_list *l);
void mr_destroy_room_list (struct mr_room_list *l);
int mr_count_uniq_rooms (struct mr *mr);
void mr_busy (struct mr *mr);

#endif	/* MININIM_MULTI_ROOM_H */

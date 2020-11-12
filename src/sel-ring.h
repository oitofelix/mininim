/*
  sel-ring.h -- selection ring module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

#ifndef MININIM_SEL_RING_H
#define MININIM_SEL_RING_H

/* variables */
extern struct sel_ring global_sel_ring;

/* Rectangular Selection */
struct rect_sel *new_rect_sel
(struct mr *mr, struct rect_sel *rs, enum rect_sel_type type,
 struct pos *a, struct pos *b);
bool is_valid_rect_sel (struct rect_sel *rs);
void destroy_sel_set_tail (struct sel_set *s);
void destroy_rect_sel (struct rect_sel *rs);
bool is_room_in_rect_sel (struct rect_sel *rs, int _room);
bool is_pos_in_rect_sel (struct rect_sel *rs, struct pos *p);
bool is_pos_in_rect_sel_array (struct rect_sel *rs, size_t rs_nmemb,
                               struct pos *p);

/* Selection Set */
bool is_valid_sel_set (struct sel_set *s);
bool is_room_possibly_in_sel_set (struct sel_set *ss, int room);
bool is_pos_in_sel_set (struct sel_set *s, struct pos *p);
bool is_room_in_sel_set (struct sel_set *s, int _room);
bool is_empty_sel_set (struct sel_set *s);
void destroy_sel_set (struct sel_set *s);
bool is_rect_sel_subset_of_sel_set
(struct rect_sel *a, struct sel_set *s);
bool is_rect_sel_and_sel_set_inter_empty
(struct rect_sel *a, struct sel_set *s);
bool is_rect_sel_helpful_for_sel_set
(struct rect_sel *a, struct sel_set *s);
bool sel_set_can_undo (struct sel_set *s, int dir);
bool sel_set_undo_pass (struct sel_set *s, int dir);

/* Selection Ring */
bool is_valid_sel_ring (struct sel_ring *sr);
void destroy_sel_ring (struct sel_ring *sr);
bool is_room_in_sel_ring (struct sel_ring *sr, int room);
bool is_pos_in_sel_ring (struct sel_ring *sr, struct pos *p);
bool sel_ring_can_undo (struct sel_ring *sr, int dir);
bool sel_ring_undo_pass (struct sel_ring *sr, int dir);
bool add_rect_sel_to_sel_ring
(struct mr *mr, struct sel_ring *sr, enum rect_sel_type type,
 struct pos *a, struct pos *b);
void new_sel_ring_entry (struct sel_ring *sr);
void del_sel_ring_entry (struct sel_ring *sr);
bool sel_ring_can_go_next (struct sel_ring *sr, int dir);
bool sel_ring_go_next (struct sel_ring *sr, int dir);
size_t sel_ring_ss_c_nmemb (struct sel_ring *sr);
size_t sel_ring_ss_nmemb (struct sel_ring *sr);

#endif	/* MININIM_SEL_RING_H */

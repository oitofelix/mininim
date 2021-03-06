/*
  fix.h -- fix module;

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

#ifndef MININIM_FIX_H
#define MININIM_FIX_H

struct pos *fix_con (struct pos *p);

/* -------------------------------------- */

void make_links_locally_consistent (struct level *l, int prev_room, int current_room);
void fix_room_0 (struct level *l);
void fix_traversable_above_room_0 (struct level *l);

struct pos *fix_door_adjacent_to_wall_or_door (struct pos *p);
struct pos *fix_skeleton_or_spikes_floor_with_no_or_loose_floor_at_left (struct pos *p);
struct pos *fix_adjacent_itens (struct pos *p);
struct pos *fix_door_lacking_opener (struct pos *p);
struct pos *fix_opener_or_closer_lacking_door (struct pos *p);
struct pos *fix_confg_which_should_not_have_conbg (struct pos *p);
struct pos *fix_partial_big_pillar (struct pos *p);

bool is_there_event_handler (struct level *l, int e);



/* room linking */
void make_reciprocal_link (struct level *l, int room0, int room1, enum dir dir);
void make_link_locally_unique (struct level *l, int room, enum dir dir);
void make_link_globally_unique (struct level *l, int room, enum dir dir);
void make_semi_consistent_link (struct level *l, int room0, int room1, enum dir dir);
void make_link_adjacency_bound (struct level *l, int room, enum dir dir);
void exchange_rooms  (struct level *l, int room0, int room1);
void circular_linking (struct level *lv);

#endif	/* MININIM_FIX_H */

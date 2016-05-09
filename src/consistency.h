/*
  consistency.h -- consistency module;

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

#ifndef MININIM_CONSISTENCY_H
#define MININIM_CONSISTENCY_H

void fix_level (void);
void make_links_locally_consistent (int prev_room, int current_room);
void fix_legacy_room_above_zero_with_traversable_at_bottom (void);

void fix_single_walls_at_place_0 (struct pos *p);
void fix_inaccessible_enclosure (struct pos *p);
void fix_loose_enclosure (struct pos *p);
void fix_rigid_con_no_floor_top (struct pos *p);
void fix_door_adjacent_to_wall_or_door (struct pos *p);
void fix_broken_floor_lacking_no_floor_on_top (struct pos *p);
void fix_skeleton_or_spikes_floor_with_no_or_loose_floor_at_left (struct pos *p);
void fix_adjacent_itens (struct pos *p);
void fix_item_on_non_normal_floor (struct pos *p);
void fix_sword_at_right_of_wall_or_door (struct pos *p);
void fix_door_lacking_opener (struct pos *p);
void fix_opener_or_closer_lacking_door (struct pos *p);
void fix_confg_which_should_not_have_conbg (struct pos *p);
void fix_partial_big_pillar (struct pos *p);

void fix_enclosure (struct pos *p, enum dir dir);

bool is_there_event_handler (int e);
bool is_enclosure (struct pos *p, bool (*pred) (struct pos *p), enum dir dir);
bool is_inaccessible (struct pos *p);
bool is_loose (struct pos *p);



/* room linking */
void make_reciprocal_link (int room0, int room1, enum dir dir);
void make_link_locally_unique (int room, enum dir dir);
void make_link_globally_unique (int room, enum dir dir);
void make_semi_consistent_link (int room0, int room1, enum dir dir);
void make_link_adjacency_bound (int room, enum dir dir);
void exchange_rooms  (int room0, int room1);

#endif	/* MININIM_CONSISTENCY_H */

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


/* room linking */
void make_reciprocal_link (int room0, int room1, enum dir dir);
void make_link_locally_unique (int room, enum dir dir);
void make_link_globally_unique (int room, enum dir dir);
void make_semi_consistent_link (int room0, int room1, enum dir dir);
void make_link_adjacency_bound (int room, enum dir dir);
void exchange_rooms  (int room0, int room1);

#endif	/* MININIM_CONSISTENCY_H */

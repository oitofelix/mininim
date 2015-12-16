/*
  physics.h -- physics module;

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

#ifndef FREEPOP_PHYSICS_H
#define FREEPOP_PHYSICS_H

#include "prince.h"
#include "pos.h"

/* functions */
struct con *con (struct pos *p);
struct con *crel (struct pos *p, int floor, int place);
bool is_frame_visible (struct frame *f);
bool is_pos_visible (struct pos *p);
bool is_strictly_traversable (struct pos *p);


int dist_next_place (struct frame *f, coord_f cf, pos_f pf,
                     int margin, bool reverse);
bool is_on_con (struct frame *f, coord_f cf, pos_f pf,
                int margin, bool reverse, int min_dist, enum confg t);
int dist_con (struct frame *f, coord_f cf, pos_f pf,
              int margin, bool reverse, enum confg t);
bool
is_colliding (struct frame *f, struct frame_offset *fo, int dx,
              bool reverse, struct collision_info *ci);
int
dist_collision (struct frame *f, bool reverse,
                struct collision_info *ci);
int
dist_chopper (struct frame *f, bool reverse);



bool is_hangable_con (struct pos *p, enum dir d);
bool is_hangable_pos (struct pos *p, enum dir d);
bool can_hang (struct frame *f, bool reverse, struct pos *hang_pos);
struct pos *get_hanged_pos (struct pos *hang_pos, enum dir d, struct pos *p);
enum confg get_hanged_con (struct pos *hang_pos, enum dir d);
bool is_hang_pos_critical (struct pos *hang_pos);
bool is_hang_pos_free (struct pos *hang_pos, enum dir d);



void update_depressible_floor (struct anim *a, int dx0, int dx1);
void keep_depressible_floor (struct anim *a);
void clear_depressible_floor (struct anim *a);
void save_depressible_floor (struct anim *a);
void restore_depressible_floor (struct anim *a);
void press_depressible_floor (struct pos *p);

#endif	/* FREEPOP_PHYSICS_H */

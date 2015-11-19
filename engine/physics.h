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

/* functions */
struct con con (struct pos pos);
struct con crel (struct pos pos, int floor, int place);
void set_confg (struct pos p, enum confg fg);
bool is_visible (struct anim a);
bool is_pos_visible (struct pos p);

struct anim
adjust_anim (struct anim a, int margin, bool reverse);

int
dist_next_place (struct anim a,
                 struct coord (*coord_func) (struct anim a),
                 struct pos (*pos_func) (struct coord c),
                 int margin, bool reverse);
bool
to_next_place_edge (struct anim *a,
                    struct coord (*coord_func) (struct anim a),
                    struct pos (*pos_func) (struct coord c),
                    int margin, bool reverse);

bool
is_colliding (struct anim a,
              struct coord (*coord_func) (struct anim a),
              struct pos (*pos_func) (struct coord c),
              int margin, bool reverse, int min_dist);
bool
is_on_con (struct anim a,
           struct coord (*coord_func) (struct anim a),
           struct pos (*pos_func) (struct coord c),
           int margin, bool reverse, int min_dist, enum confg ct);

#endif	/* FREEPOP_PHYSICS_H */

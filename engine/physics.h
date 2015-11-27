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

/* variables */
extern struct pos hang_pos;
extern enum confg confg_collision;

/* functions */
struct con *con (struct pos pos);
struct con *crel (struct pos pos, int floor, int place);
bool is_visible (struct anim a);
bool is_pos_visible (struct pos p);



int
dist_next_place (struct anim a,
                 struct coord (*coord_func) (struct anim a),
                 struct pos (*pos_func) (struct coord c),
                 int margin, bool reverse);
void
to_next_place_edge (struct anim *a, ALLEGRO_BITMAP *frame,
                    struct coord (*coord_func) (struct anim a),
                    struct pos (*pos_func) (struct coord c),
                    int margin, bool reverse, int min_dist);

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

int
dist_collision (struct anim a,
                struct coord (*coord_func) (struct anim a),
                struct pos (*pos_func) (struct coord c),
                int margin, bool reverse);
int
dist_con (struct anim a,
          struct coord (*coord_func) (struct anim a),
          struct pos (*pos_func) (struct coord c),
          int margin, bool reverse, enum confg ct);

bool
to_collision_edge (struct anim *a, ALLEGRO_BITMAP *frame,
                   struct coord (*coord_func) (struct anim a),
                   struct pos (*pos_func) (struct coord c),
                   int margin, bool reverse, int min_dist);
bool
to_con_edge (struct anim *a, ALLEGRO_BITMAP *frame,
             struct coord (*coord_func) (struct anim a),
             struct pos (*pos_func) (struct coord c),
             int margin, bool reverse, int min_dist, enum confg ct);


bool is_hangable_pos (struct pos p, enum dir d);
bool can_hang (struct anim a);


void update_depressible_floor (struct anim *a, int dx0, int dx1);
void keep_depressible_floor (struct anim *a);
void clear_depressible_floor (struct anim *a);
void save_depressible_floor (struct anim *a);
void restore_depressible_floor (struct anim *a);
void press_depressible_floor (struct pos p);

#endif	/* FREEPOP_PHYSICS_H */

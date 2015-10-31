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

#include "anim.h"

#define ROOMS 25
#define FLOORS 3
#define PLACES 10

#define PLACE_WIDTH (ORIGINAL_WIDTH / PLACES)
#define PLACE_HEIGHT ((ORIGINAL_HEIGHT - 11) / FLOORS)

/* types */
struct level {
  enum {
    DUNGEON, PALACE,
  } type;

  enum obj {
      NO_FLOOR,
      NORMAL_FLOOR,
      NORMAL_FLOOR_BRICKS_1,
      NORMAL_FLOOR_TORCH,
      BROKEN_FLOOR,
      LOOSE_FLOOR,
      WALL,
      PILLAR,
  } obj[ROOMS][FLOORS][PLACES];

  unsigned int link[ROOMS][4];
};

struct pos {
  unsigned int room;
  int floor, place;
};

/* functions */
enum obj obj (struct pos pos);
enum obj obj_rel (struct pos pos, int floor, int place);
struct pos norm_pos (struct pos pos, bool floor_first);
struct pos norm_pos_floor (struct pos pos);
struct pos norm_pos_place (struct pos pos);
unsigned int prandom_pos (struct pos pos, unsigned int i,
                          unsigned int max);
struct pos pos_xy (unsigned int room, int x, int y);
struct pos pos (struct anim anim);
bool is_colliding (struct anim anim);
int dist_collision (struct anim anim);
int dist_fall (struct anim anim);
bool is_falling (struct anim anim);
void to_collision_edge (struct anim *anim);
void to_fall_edge (struct anim *anim);
void apply_physics (struct anim *a, ALLEGRO_BITMAP *frame,
                    int dx, int dy);


#endif	/* FREEPOP_PHYSICS_H */

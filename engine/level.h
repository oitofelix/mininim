/*
  level.h -- level module;

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

#ifndef FREEPOP_LEVEL_H
#define FREEPOP_LEVEL_H

#include "anim.h"

#define LEVEL_ROOMS 25
#define LEVEL_FLOORS 3
#define LEVEL_PLACES 10

struct level {
  enum {
    DUNGEON, PALACE,
  } type;

  enum level_object {
      NO_FLOOR,
      NORMAL_FLOOR,
      NORMAL_FLOOR_BRICKS_1,
      NORMAL_FLOOR_TORCH,
      BROKEN_FLOOR,
      LOOSE_FLOOR,
      WALL,
      PILLAR,
  } object[LEVEL_ROOMS][LEVEL_FLOORS][LEVEL_PLACES];

  unsigned int link[LEVEL_ROOMS][4];
};

struct level_pos {
  unsigned int room;
  int floor, place;
};

/* current level */
extern struct level *level;

/* random number generator seed */
extern uint32_t random_seed;

void play_level (struct level *level);
enum level_object level_obj (struct level_pos pos);
enum level_object level_obj_rel (struct level_pos pos, int floor, int place);
struct level_pos anim_pos (struct anim anim);
unsigned int obj_dist (struct anim);
struct level_pos norm_pos (struct level_pos pos, bool floor_first);
struct level_pos norm_pos_floor (struct level_pos pos);
struct level_pos norm_pos_place (struct level_pos pos);
unsigned int prandom_pos (struct level_pos pos, unsigned int i, unsigned int max);

#endif	/* FREEPOP_LEVEL_H */

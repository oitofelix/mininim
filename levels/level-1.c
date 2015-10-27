/*
  world.c -- level 1 module;

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

#include "kernel/video.h"
#include "engine/level.h"

static struct level level_1 = {
  .type = DUNGEON,

  .object = {
    [0] =
    {{WALL, NO_FLOOR, NO_FLOOR, NO_FLOOR, NO_FLOOR,
      NO_FLOOR, NO_FLOOR, NO_FLOOR, NO_FLOOR, WALL},
     {WALL, NO_FLOOR, NO_FLOOR, NO_FLOOR, NO_FLOOR,
      NO_FLOOR, NO_FLOOR, NO_FLOOR, NO_FLOOR, WALL},
     {NORMAL_FLOOR, NORMAL_FLOOR, NORMAL_FLOOR, NORMAL_FLOOR, NORMAL_FLOOR,
      NORMAL_FLOOR, NORMAL_FLOOR, NORMAL_FLOOR, NORMAL_FLOOR, NORMAL_FLOOR}},

    [1] =
    {{NO_FLOOR, NO_FLOOR, NO_FLOOR,
      NORMAL_FLOOR, NORMAL_FLOOR_BRICKS_1, NORMAL_FLOOR,
      NORMAL_FLOOR_BRICKS_1, NORMAL_FLOOR, WALL, WALL},
     {NORMAL_FLOOR_TORCH, NORMAL_FLOOR_TORCH, NORMAL_FLOOR_BRICKS_1, PILLAR, NO_FLOOR,
      WALL, WALL, WALL, WALL, WALL},
     {WALL, WALL, WALL, WALL, BROKEN_FLOOR,
      PILLAR, LOOSE_FLOOR, NORMAL_FLOOR_BRICKS_1, NORMAL_FLOOR, WALL}},
  },

  .link = {{0, 0, 0, 0},
           {0, 0, 0, 0},},
};

void
play_level_1 (void)
{
  play_level (&level_1);
}

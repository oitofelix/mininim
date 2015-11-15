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
#include "engine/physics.h"
#include "engine/level.h"

static struct level level_1 = {
  .type = DUNGEON,

  .construct = {
    [0] =
    {{{WALL}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR},
      {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {WALL}},
     {{WALL}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR},
      {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {WALL}},
     {{FLOOR}, {FLOOR}, {FLOOR}, {FLOOR}, {FLOOR},
      {FLOOR}, {FLOOR}, {FLOOR}, {FLOOR}, {FLOOR}}},

    [1] =
    {{{NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {FLOOR}, {FLOOR,BRICKS_01}, {FLOOR},
      {FLOOR,BRICKS_01}, {FLOOR}, {WALL}, {WALL}},
     {{FLOOR,TORCH}, {FLOOR,TORCH}, {FLOOR,BRICKS_01}, {PILLAR}, {NO_FLOOR},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}},
     {{WALL}, {WALL}, {WALL}, {WALL}, {BROKEN_FLOOR},
      {PILLAR}, {LOOSE_FLOOR}, {FLOOR,BRICKS_01}, {FLOOR}, {WALL}}},

    [2] =
    {{{NO_FLOOR,BRICKS_03}, {NO_FLOOR}, {WALL}, {WALL}, {WALL},
      {WALL}, {NO_FLOOR,BRICKS_03}, {NO_FLOOR}, {WALL}, {WALL}},
     {{FLOOR,TORCH}, {FLOOR}, {PILLAR}, {FLOOR,TORCH}, {FLOOR},
      {PILLAR}, {FLOOR}, {FLOOR,BRICKS_01}, {PILLAR}, {FLOOR}},
     {{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}}},

    [3] =
    {{{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}},
     {{PILLAR}, {FLOOR,BRICKS_01}, {FLOOR}, {FLOOR,TORCH}, {FLOOR,BRICKS_01},
      {FLOOR,TORCH}, {FLOOR}, {FLOOR,BRICKS_01}, {FLOOR,BRICKS_01}, {PILLAR}},
     {{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}}},

    [6] =
    {{{FLOOR,TORCH}, {FLOOR,BRICKS_01}, {OPENER_FLOOR,NO_BG,0}, {NO_FLOOR}, {PILLAR},
      {FLOOR}, {FLOOR,BRICKS_01}, {FLOOR,TORCH}, {FLOOR}, {PILLAR}},
     {{WALL}, {WALL}, {WALL}, {NO_FLOOR}, {WALL},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}},
     {{WALL}, {WALL}, {WALL}, {SPIKES_FLOOR}, {SPIKES_FLOOR},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}}},

    [7] =
    {{{NO_FLOOR}, {NO_FLOOR}, {OPENER_FLOOR,NO_BG,1}, {FLOOR,BRICKS_01}, {PILLAR},
      {LOOSE_FLOOR}, {LOOSE_FLOOR}, {PILLAR}, {FLOOR,TORCH}, {DOOR}},
     {{FLOOR,TORCH}, {FLOOR}, {PILLAR}, {FLOOR}, {PILLAR},
      {NO_FLOOR,BRICKS_03}, {NO_FLOOR}, {WALL}, {WALL}, {WALL}},
     {{WALL}, {WALL}, {WALL}, {LOOSE_FLOOR}, {PILLAR},
      {FLOOR}, {FLOOR}, {PILLAR}, {FLOOR}, {WALL}}},

    [8] =
    {{{NO_FLOOR,BRICKS_03}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR},
      {NO_FLOOR,BRICKS_03}, {OPENER_FLOOR,NO_BG,0}, {FLOOR,BRICKS_01}, {FLOOR}, {DOOR}},
     {{FLOOR,TORCH}, {FLOOR,TORCH}, {FLOOR}, {LOOSE_FLOOR}, {NO_FLOOR,BRICKS_03},
      {FLOOR}, {PILLAR}, {OPENER_FLOOR,NO_BG,1}, {FLOOR}, {WALL}},
     {{WALL}, {WALL}, {WALL}, {NO_FLOOR}, {NO_FLOOR},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}}},

    [9] =
    {{{FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {FLOOR},
      {NO_FLOOR}, {NO_FLOOR,BRICKS_04}, {NO_FLOOR}, {FLOOR}, {WALL}},
     {{PILLAR}, {FLOOR,BRICKS_01}, {FLOOR,TORCH}, {FLOOR}, {FLOOR},
      {FLOOR,TORCH}, {FLOOR}, {FLOOR,BRICKS_01}, {FLOOR}, {WALL}},
     {{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}}},
  },

  .link = {[0] = {0, 0, 0, 0},
           [1] = {0, 0, 0, 2},
           [2] = {6, 3, 1, 0},
           [3] = {2, 9, 0, 0},
           [6] = {8, 2, 0, 0},
           [7] = {0, 8, 0, 0},
           [8] = {7, 6, 0, 0},
           [9] = {3, 0, 0, 0},},

  .event = {
    [0] = {{8,0,9}, false},
    [1] = {{7,0,9}, false},
  }
};

void
play_level_1 (void)
{
  play_level (&level_1);
}

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
#include "engine/kid/kid.h"

static void next_level (int lv, struct pos *exit_door_pos);
static void start (void);
static struct level level_1;

void
play_level_1 (void)
{
  next_level (1, NULL);
  play_level (&level_1);
}

static void
start (void)
{
  create_kid (NULL);
  create_kid (&kid[0]);
}

static void
next_level (int number, struct pos *exit_door_pos)
{
  return;
}

static struct level level_1 = {
  .next_level = next_level,
  .start = start,
  .number = 1,

  .con = {
    [0] =
    {{{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}},
     {{WALL}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR},
      {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {WALL}},
     {{FLOOR}, {FLOOR}, {FLOOR}, {FLOOR}, {FLOOR},
      {FLOOR}, {FLOOR}, {FLOOR}, {FLOOR}, {FLOOR}}},

    [1] =
    {{{NO_FLOOR,BRICKS_01}, {NO_FLOOR,BRICKS_02}, {NO_FLOOR,BRICKS_03}, {FLOOR,BRICKS_04}, {FLOOR,BRICKS_01}, {FLOOR,WINDOW,{SWORD}},
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

    [5] =
    {{{PILLAR}, {FLOOR,TORCH}, {CLOSER_FLOOR,NO_BG,{3}}, {FLOOR,TORCH}, {OPENER_FLOOR,NO_BG,{2}},
      {DOOR}, {OPENER_FLOOR,NO_BG,{3}}, {NO_FLOOR,BRICKS_03}, {FLOOR}, {DOOR}},
     {{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
      {WALL}, {PILLAR}, {NO_FLOOR}, {WALL}, {WALL}},
     {{WALL}, {WALL}, {FLOOR,TORCH}, {FLOOR,NO_BG,{SMALL_LIFE_POTION}}, {FLOOR,BRICKS_01},
      {LOOSE_FLOOR}, {PILLAR}, {BROKEN_FLOOR}, {WALL}, {WALL}}},

    [6] =
    {{{FLOOR,TORCH}, {FLOOR,BRICKS_01}, {OPENER_FLOOR,NO_BG,{0}}, {NO_FLOOR}, {PILLAR},
      {FLOOR}, {FLOOR,BRICKS_01}, {FLOOR,TORCH}, {FLOOR}, {PILLAR}},
     {{WALL}, {WALL}, {WALL}, {NO_FLOOR}, {WALL},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}},
     {{WALL}, {WALL}, {WALL}, {SPIKES_FLOOR}, {SPIKES_FLOOR},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}}},

    [7] =
    {{{FLOOR}, {FLOOR}, {LOOSE_FLOOR}, {LOOSE_FLOOR,BRICKS_01},
      {OPENER_FLOOR,NO_BG,{0}}, {OPENER_FLOOR,NO_BG,{1}},
      {FLOOR}, {BROKEN_FLOOR}, {FLOOR,TORCH}, {DOOR}},
     {{FLOOR,TORCH}, {NO_FLOOR}, {CLOSER_FLOOR}, {LOOSE_FLOOR}, {FLOOR},
      {NO_FLOOR,BRICKS_03}, {NO_FLOOR}, {WALL}, {WALL}, {WALL}},
     {{WALL}, {FLOOR}, {DOOR}, {SKELETON_FLOOR}, {FLOOR},
      {FLOOR}, {FLOOR}, {PILLAR}, {FLOOR}, {WALL}}},

    [8] =
    {{{NO_FLOOR,BRICKS_03}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR},
      {NO_FLOOR,BRICKS_03}, {OPENER_FLOOR,NO_BG,{0}}, {FLOOR,BRICKS_01}, {FLOOR}, {DOOR}},
     {{FLOOR,TORCH}, {FLOOR,TORCH}, {FLOOR}, {LOOSE_FLOOR}, {NO_FLOOR,BRICKS_03},
      {FLOOR}, {PILLAR}, {OPENER_FLOOR,NO_BG,{1}}, {FLOOR}, {WALL}},
     {{WALL}, {WALL}, {WALL}, {NO_FLOOR}, {NO_FLOOR},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}}},

    [9] =
    {{{FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {NO_FLOOR}, {FLOOR},
      {NO_FLOOR}, {NO_FLOOR,BRICKS_04}, {NO_FLOOR}, {FLOOR}, {WALL}},
     {{PILLAR}, {FLOOR,BRICKS_01}, {FLOOR,TORCH}, {FLOOR}, {FLOOR},
      {FLOOR,TORCH}, {FLOOR}, {FLOOR,BRICKS_01}, {FLOOR}, {WALL}},
     {{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}}},

    [23] =
    {{{WALL}, {WALL}, {WALL}, {WALL}, {FLOOR},
      {FLOOR,NO_BG,{BIG_LIFE_POTION}}, {FLOOR,TORCH}, {OPENER_FLOOR}, {FLOOR}, {DOOR}},
     {{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}},
     {{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
      {WALL}, {WALL}, {WALL}, {WALL}, {WALL}}},
  },

  .link = {[0] = {0, 0, 0, 0},
           [1] = {5, 0, 0, 2},
           [2] = {6, 3, 1, 0},
           [3] = {2, 9, 0, 0},
           [5] = {0, 1, 0, 6},
           [6] = {8, 2, 5, 0},
           [7] = {0, 8, 23, 0},
           [8] = {7, 6, 0, 0},
           [9] = {3, 0, 0, 0},
           [23] = {23, 23, 0, 7}},

  .event = {
    [0] = {{8,0,9}, false},
    [1] = {{7,0,9}, false},
    [2] = {{5,0,5}, false},
    [3] = {{5,0,9}, false},
    [4] = {{23,1,4}, false},
  }
};

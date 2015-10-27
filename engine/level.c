/*
  level.c -- level module;

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

#include <error.h>

#include "kernel/video.h"
#include "kernel/random.h"

#include "anim.h"
#include "level.h"
#include "room.h"
#include "fire.h"
#include "kid.h"

/* functions */
static void load_level (void);
static void unload_level (void);
static void level_anim (void);

/* current level */
struct level *level;

enum level_object
level_obj (struct level_pos pos)
{
  pos = norm_pos (pos, true);
  return level->object[pos.room][pos.floor][pos.place];
}

enum level_object
level_obj_rel (struct level_pos pos, int floor, int place)
{
  pos.floor += floor;
  pos.place += place;
  return level_obj (pos);
}

struct level_pos
norm_pos (struct level_pos pos, bool floor_first)
{
  if (pos.room >= LEVEL_ROOMS)
    error (-1, 0, "%s: room out of range (%u)", __func__, pos.room);

  if (floor_first) {
    pos = norm_pos_floor (pos);
    pos = norm_pos_place (pos);
  } else {
    pos = norm_pos_place (pos);
    pos = norm_pos_floor (pos);
  }

  return pos;
}

struct level_pos
norm_pos_floor (struct level_pos pos)
{
  unsigned int room_offset;
  if (pos.floor < 0) {
    unsigned int q = (-pos.floor) / LEVEL_FLOORS;
    unsigned int r = (-pos.floor) % LEVEL_FLOORS;
    pos.floor = r ? LEVEL_FLOORS - r : 0;
    for (room_offset = r ? q + 1 : q; room_offset > 0; room_offset--)
      pos.room = level->link[pos.room][TOP];
  } else if (pos.floor >= LEVEL_FLOORS) {
    unsigned int q = pos.floor / LEVEL_FLOORS;
    unsigned int r = pos.floor % LEVEL_FLOORS;
    pos.floor = r;
    for (room_offset = q; room_offset > 0; room_offset--)
      pos.room = level->link[pos.room][BOTTOM];
  }
  return pos;
}

struct level_pos
norm_pos_place (struct level_pos pos)
{
  unsigned int room_offset;
  if (pos.place < 0) {
    unsigned int q = (-pos.place) / LEVEL_PLACES;
    unsigned int r = (-pos.place) % LEVEL_PLACES;
    pos.place = r ? LEVEL_PLACES - r : 0;
    for (room_offset = r ? q + 1 : q; room_offset > 0; room_offset--)
      pos.room = level->link[pos.room][LEFT];
  } else if (pos.place >= LEVEL_PLACES) {
    unsigned int q = pos.place / LEVEL_PLACES;
    unsigned int r = pos.place % LEVEL_PLACES;
    pos.floor = r;
    for (room_offset = q; room_offset > 0; room_offset--)
      pos.room = level->link[pos.room][RIGHT];
  }
  return pos;
}

unsigned int
prandom_pos (struct level_pos pos, unsigned int i, unsigned int max)
{
  return
    prandom_uniq (pos.room + pos.floor * LEVEL_PLACES + pos.place + i, max);
}

void
play_level (struct level *_level)
{
  level = _level;
  load_level ();
  play_anim (level_anim, 12);
  unload_level ();
}

static void
load_level (void)
{
  load_room ();
  load_fire ();
  load_kid ();
}

static void
unload_level (void)
{
  unload_room ();
  unload_fire ();
  unload_kid ();
}

static void
level_anim (void)
{
  clear_bitmap (screen, BLACK);
  draw_room (1);
  draw_fire (1);
  (*draw_kid) ();
}

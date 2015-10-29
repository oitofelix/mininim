/*
  physics.c -- physics module;

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
#include "kernel/random.h"
#include "physics.h"
#include "level.h"
#include "kid.h"

enum obj
obj (struct pos pos)
{
  pos = norm_pos (pos, true);
  return level->obj[pos.room][pos.floor][pos.place];
}

enum obj
obj_rel (struct pos pos, int floor, int place)
{
  pos.floor += floor;
  pos.place += place;
  return obj (pos);
}

struct pos
norm_pos (struct pos pos, bool floor_first)
{
  if (pos.room >= ROOMS)
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

struct pos
norm_pos_floor (struct pos pos)
{
  unsigned int room_offset;
  if (pos.floor < 0) {
    unsigned int q = (-pos.floor) / FLOORS;
    unsigned int r = (-pos.floor) % FLOORS;
    pos.floor = r ? FLOORS - r : 0;
    for (room_offset = r ? q + 1 : q; room_offset > 0; room_offset--)
      pos.room = level->link[pos.room][TOP];
  } else if (pos.floor >= FLOORS) {
    unsigned int q = pos.floor / FLOORS;
    unsigned int r = pos.floor % FLOORS;
    pos.floor = r;
    for (room_offset = q; room_offset > 0; room_offset--)
      pos.room = level->link[pos.room][BOTTOM];
  }
  return pos;
}

struct pos
norm_pos_place (struct pos pos)
{
  unsigned int room_offset;
  if (pos.place < 0) {
    unsigned int q = (-pos.place) / PLACES;
    unsigned int r = (-pos.place) % PLACES;
    pos.place = r ? PLACES - r : 0;
    for (room_offset = r ? q + 1 : q; room_offset > 0; room_offset--)
      pos.room = level->link[pos.room][LEFT];
  } else if (pos.place >= PLACES) {
    unsigned int q = pos.place / PLACES;
    unsigned int r = pos.place % PLACES;
    pos.floor = r;
    for (room_offset = q; room_offset > 0; room_offset--)
      pos.room = level->link[pos.room][RIGHT];
  }
  return pos;
}

unsigned int
prandom_pos (struct pos pos, unsigned int i, unsigned int max)
{
  return
    prandom_uniq (pos.room + pos.floor * PLACES + pos.place + i, max);
}

struct pos
pos_xy (unsigned int room, int x, int y)
{
  struct pos pos;

  unsigned int qx = x / PLACE_WIDTH;
  unsigned int rx = x % PLACE_WIDTH;
  unsigned int qy = y / PLACE_HEIGHT;
  unsigned int ry = y % PLACE_HEIGHT;

  pos.room = room;
  pos.place = (rx < 13) ? qx - 1 : qx;
  pos.floor = (ry < 3) ? qy - 1 : qy;

  if (x < 0) pos.place = -1;
  if (y < 0) pos.floor = -1;

  return pos;
}

struct pos
pos (struct anim anim)
{
  int w = al_get_bitmap_width (anim.frame);
  return pos_xy (anim.room, (anim.dir == LEFT) ? anim.x : anim.x + w - 1,
                 anim.y);
}

bool
is_colliding (struct anim anim)
{
  if (! anim.collision) return false;

  anim.x += (anim.dir == LEFT) ? 0 : -3;
  struct pos p = pos (anim);
  enum obj o = obj (p);
  if (o == WALL) {
    return true;
  } else return false;
}

unsigned int
dist_collision (struct anim anim)
{
  unsigned int i = 0;
  int inc = (anim.dir == LEFT) ? -1 : +1;
  while (! is_colliding (anim))
    if (i == PLACE_WIDTH) break;
    else {
      i++;
      anim.x += inc;
    }
  return i;
}

bool
is_falling (struct anim anim)
{
  if (! anim.fall) return false;

  if ((anim.frame == kid_jump_08 || anim.frame == kid_jump_09
       || anim.frame == kid_jump_10))
    return false;

  if (anim.frame == kid_fall_13 || anim.frame == kid_fall_14
      || anim.frame == kid_fall_15 || anim.frame == kid_fall_16)
    return true;

  anim.x += (anim.dir == LEFT) ? 4 : -5;
  struct pos p = pos (anim);
  enum obj o = obj (p);
  if (o == NO_FLOOR) {
    return true;
  } else return false;
}

unsigned int
dist_fall (struct anim anim)
{
  unsigned int i = 0;
  int inc = (anim.dir == LEFT) ? -1 : +1;
  while (! is_falling (anim))
    if (i == PLACE_WIDTH) break;
    else {
      i++;
      anim.x += inc;
    }
  return i;
}

void
to_edge (struct anim *anim)
{
  unsigned int dc = dist_collision (*anim);
  unsigned int df = dist_fall (*anim);

  anim->x += ((anim->dir == LEFT) ? -1 : +1) * ((dc < PLACE_WIDTH) ? dc - 1 : 0);
  anim->x += ((anim->dir == LEFT) ? -1 : +1) * ((df < PLACE_WIDTH) ? df - 1 : 0);
}

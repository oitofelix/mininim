/*
  floor.c -- floor module;

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

#include <stdio.h>
#include <error.h>
#include "kernel/video.h"
#include "kernel/audio.h"
#include "kernel/random.h"
#include "physics.h"
#include "kid.h"
#include "pillar.h"
#include "wall.h"
#include "level.h"
#include "room.h"
#include "loose-floor.h"
#include "opener-floor.h"
#include "spikes-floor.h"
#include "floor.h"

ALLEGRO_BITMAP *normal_floor_left, *normal_floor_right,
  *normal_floor_base,
  *broken_floor_left, *broken_floor_right, *broken_floor_front,
  *skeleton_floor_left, *skeleton_floor_right,
  *floor_corner_01, *floor_corner_02, *floor_corner_03;

void
load_vdungeon_floor (void)
{
  normal_floor_left = load_bitmap (VDUNGEON_NORMAL_FLOOR_LEFT);
  normal_floor_right = load_bitmap (VDUNGEON_NORMAL_FLOOR_RIGHT);
  normal_floor_base = load_bitmap (VDUNGEON_NORMAL_FLOOR_BASE);
  broken_floor_left = load_bitmap (VDUNGEON_BROKEN_FLOOR_LEFT);
  broken_floor_right = load_bitmap (VDUNGEON_BROKEN_FLOOR_RIGHT);
  broken_floor_front = load_bitmap (VDUNGEON_BROKEN_FLOOR_FRONT);
  skeleton_floor_left = load_bitmap (VDUNGEON_SKELETON_FLOOR_LEFT);
  skeleton_floor_right = load_bitmap (VDUNGEON_SKELETON_FLOOR_RIGHT);
  floor_corner_01 = load_bitmap (VDUNGEON_FLOOR_CORNER_01);
  floor_corner_02 = load_bitmap (VDUNGEON_FLOOR_CORNER_02);
  floor_corner_03 = load_bitmap (VDUNGEON_FLOOR_CORNER_03);
}

void
unload_floor (void)
{
  al_destroy_bitmap (normal_floor_left);
  al_destroy_bitmap (normal_floor_right);
  al_destroy_bitmap (normal_floor_base);
  al_destroy_bitmap (broken_floor_left);
  al_destroy_bitmap (broken_floor_right);
  al_destroy_bitmap (broken_floor_front);
  al_destroy_bitmap (skeleton_floor_left);
  al_destroy_bitmap (skeleton_floor_right);
  al_destroy_bitmap (floor_corner_01);
  al_destroy_bitmap (floor_corner_02);
  al_destroy_bitmap (floor_corner_03);
}

void
draw_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (normal_floor_base, bitmap, floor_base_coord (p), 0);
}

void
draw_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (normal_floor_left, bitmap, floor_left_coord (p), 0);
  draw_bitmapc (normal_floor_right, bitmap, floor_right_coord (p), 0);
  draw_con_left (bitmap, prel (p, 0, +1));
}

void
draw_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (normal_floor_left, bitmap, floor_left_coord (p), 0);
}

void
draw_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (normal_floor_right, bitmap, floor_right_coord (p), 0);
  draw_con_left (bitmap, prel (p, 0, +1));
}

void
draw_broken_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
}

void
draw_broken_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_broken_floor_base (bitmap, p);
  draw_bitmapc (broken_floor_left, bitmap, floor_left_coord (p), 0);
  draw_bitmapc (broken_floor_right, bitmap, broken_floor_right_coord (p), 0);
  draw_con_left (bitmap, prel (p, 0, +1));
}

void
draw_broken_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_broken_floor_base (bitmap, p);
  draw_bitmapc (broken_floor_left, bitmap, floor_left_coord (p), 0);
}

void
draw_broken_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_broken_floor_base (bitmap, p);
  draw_bitmapc (broken_floor_right, bitmap, broken_floor_right_coord (p), 0);
  draw_con_left (bitmap, prel (p, 0, +1));
}

void
draw_broken_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (broken_floor_front, bitmap,
                broken_floor_front_coord (p), 0);
  draw_broken_floor_base (bitmap, p);
}

void
draw_skeleton_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
}

void
draw_skeleton_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_skeleton_floor_base (bitmap, p);
  draw_bitmapc (skeleton_floor_left, bitmap, skeleton_floor_left_coord (p), 0);
  draw_bitmapc (skeleton_floor_right, bitmap, skeleton_floor_right_coord (p), 0);
  draw_con_left (bitmap, prel (p, 0, +1));
}

void
draw_skeleton_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_skeleton_floor_base (bitmap, p);
  draw_bitmapc (skeleton_floor_left, bitmap, skeleton_floor_left_coord (p), 0);
}

void
draw_skeleton_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_skeleton_floor_base (bitmap, p);
  draw_bitmapc (skeleton_floor_right, bitmap, skeleton_floor_right_coord (p), 0);
  draw_con_left (bitmap, prel (p, 0, +1));
}

void
draw_floor_corner_01 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct coord c = floor_corner_01_coord (p);
  if (con (p)->fg == CLOSER_FLOOR) c.y += 1;
  draw_bitmapc (floor_corner_01, bitmap, c, 0);
}

void
draw_floor_corner_02 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct coord c = floor_corner_02_coord (p);
  if (con (p)->fg == CLOSER_FLOOR) c.y += 1;
  draw_bitmapc (floor_corner_02, bitmap, c, 0);
}

void
draw_floor_corner_03 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct coord c = floor_corner_03_coord (p);
  if (con (p)->fg == CLOSER_FLOOR) c.y += 1;
  draw_bitmapc (floor_corner_03, bitmap, c, 0);
}

struct coord
floor_base_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * (p.floor + 1);
  c.room = p.room;
  return c;
}

struct coord
floor_left_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 50;
  c.room = p.room;
  return c;
}

struct coord
floor_right_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 50;
  c.room = p.room;
  return c;
}

struct coord
broken_floor_right_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 49;
  c.room = p.room;
  return c;
}

struct coord
broken_floor_front_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 54;
  c.room = p.room;
  return c;
}

struct coord
skeleton_floor_left_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 44;
  c.room = p.room;
  return c;
}

struct coord
skeleton_floor_right_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 44;
  c.room = p.room;
  return c;
}

struct coord
floor_corner_01_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 53;
  c.room = p.room;
  return c;
}

struct coord
floor_corner_02_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 53;
  c.room = p.room;
  return c;
}

struct coord
floor_corner_03_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 55;
  c.room = p.room;
  return c;
}

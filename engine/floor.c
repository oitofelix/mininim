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
#include "floor.h"

ALLEGRO_BITMAP *floor_normal_left, *floor_normal_right, *floor_normal_base,
  *floor_broken_left, *floor_broken_right, *floor_broken_front,
  *floor_corner_01, *floor_corner_02, *floor_corner_03,
  *floor_spikes_left, *floor_spikes_right;

void
load_vdungeon_floor (void)
{
  /* bitmaps */
  floor_normal_left = load_bitmap (VDUNGEON_FLOOR_NORMAL_LEFT);
  floor_normal_right = load_bitmap (VDUNGEON_FLOOR_NORMAL_RIGHT);
  floor_normal_base = load_bitmap (VDUNGEON_FLOOR_NORMAL_BASE);
  floor_broken_left = load_bitmap (VDUNGEON_FLOOR_BROKEN_LEFT);
  floor_broken_right = load_bitmap (VDUNGEON_FLOOR_BROKEN_RIGHT);
  floor_broken_front = load_bitmap (VDUNGEON_FLOOR_BROKEN_FRONT);
  floor_corner_01 = load_bitmap (VDUNGEON_FLOOR_CORNER_01);
  floor_corner_02 = load_bitmap (VDUNGEON_FLOOR_CORNER_02);
  floor_corner_03 = load_bitmap (VDUNGEON_FLOOR_CORNER_03);
  floor_spikes_left = load_bitmap (VDUNGEON_FLOOR_SPIKES_LEFT);
  floor_spikes_right = load_bitmap (VDUNGEON_FLOOR_SPIKES_RIGHT);
}

void
unload_floor (void)
{
  /* bitmaps */
  al_destroy_bitmap (floor_normal_left);
  al_destroy_bitmap (floor_normal_right);
  al_destroy_bitmap (floor_normal_base);
  al_destroy_bitmap (floor_broken_left);
  al_destroy_bitmap (floor_broken_right);
  al_destroy_bitmap (floor_broken_front);
  al_destroy_bitmap (floor_corner_01);
  al_destroy_bitmap (floor_corner_02);
  al_destroy_bitmap (floor_corner_03);
  al_destroy_bitmap (floor_spikes_left);
  al_destroy_bitmap (floor_spikes_right);
}

void
draw_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (floor_normal_base, bitmap, floor_base_coord (p), 0);
}

void
draw_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_normal_left, bitmap, floor_left_coord (p), 0);
  draw_bitmapc (floor_normal_right, bitmap, floor_right_coord (p), 0);
}

void
draw_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_normal_left, bitmap, floor_left_coord (p), 0);
}

void
draw_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_normal_right, bitmap, floor_right_coord (p), 0);
}

void
draw_broken_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_broken_left, bitmap, floor_left_coord (p), 0);
  draw_bitmapc (floor_broken_right, bitmap, floor_right_coord (p), 0);
}

void
draw_broken_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_broken_left, bitmap, floor_left_coord (p), 0);
}

void
draw_broken_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_broken_right, bitmap, floor_right_coord (p), 0);
}

void
draw_broken_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (floor_broken_front, bitmap,
                  broken_floor_front_coord (p), 0);
}

void
draw_spikes_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_spikes_left, bitmap, floor_left_coord (p), 0);
  draw_bitmapc (floor_spikes_right, bitmap, floor_right_coord (p), 0);
}

void
draw_spikes_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_spikes_left, bitmap, floor_left_coord (p), 0);
}

void
draw_spikes_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_spikes_right, bitmap, floor_right_coord (p), 0);
}

void
draw_floor_corner_01 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (floor_corner_01, bitmap, floor_corner_01_coord (p), 0);
}

void
draw_floor_corner_02 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (floor_corner_02, bitmap, floor_corner_02_coord (p), 0);
}

void
draw_floor_corner_03 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (floor_corner_03, bitmap, floor_corner_03_coord (p), 0);
}

void
draw_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (peq (p, prel (kids.ptl, 0, +1))
      && is_kid_fall ()) draw_floor_left (bitmap, p);
  else if (peq (kids.ptf, p)
           && is_kid_climb ()
           && kid.dir == RIGHT) {
    draw_floor_base (bitmap, p);
    if (kid.frame == kid_climb_03
        || kid.frame == kid_climb_09
        || kid.frame == kid_climb_10)
      draw_floor_corner_03 (bitmap, p);
    else if (kid.frame == kid_climb_04
             || kid.frame == kid_climb_06
             || kid.frame == kid_climb_07)
      draw_floor_corner_01 (bitmap, p);
    else if (kid.frame == kid_climb_08
             || kid.frame == kid_climb_05)
      draw_floor_corner_02 (bitmap, p);
  } else if ((peq (p, kidsf.ptl)
              || peq (p, kidsf.ptr)
              || peq (p, kidsf.pmt)
              || peq (p, kids.ptl)
              || peq (p, kids.ptr)
              || peq (p, kids.pmt))
             && (peq (p, prel (kidsf.pmbo, -1, 0))
                 || peq (p, prel (kids.pmbo, -1, 0)))
             && ! (is_kid_hang () && kid.dir == LEFT)
             && ! ((is_kid_climb ()
                    || is_kid_start_climb ())
                   && kid.dir == LEFT)) {
    draw_floor (bitmap, p);
    draw_construct_left (bitmap, prel (p, 0, +1));
  }
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
broken_floor_front_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 54;
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

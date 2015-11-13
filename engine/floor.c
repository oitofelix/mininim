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
#include "opener-floor.h"
#include "floor.h"

ALLEGRO_BITMAP *normal_floor_left, *normal_floor_right, *normal_floor_base,
  *broken_floor_left, *broken_floor_right, *broken_floor_front,
  *spikes_floor_left, *spikes_floor_right, *opener_floor_left, *opener_floor_base,
  *floor_corner_01, *floor_corner_02, *floor_corner_03;

void
load_vdungeon_floor (void)
{
  /* bitmaps */
  normal_floor_left = load_bitmap (VDUNGEON_NORMAL_FLOOR_LEFT);
  normal_floor_right = load_bitmap (VDUNGEON_NORMAL_FLOOR_RIGHT);
  normal_floor_base = load_bitmap (VDUNGEON_NORMAL_FLOOR_BASE);
  broken_floor_left = load_bitmap (VDUNGEON_BROKEN_FLOOR_LEFT);
  broken_floor_right = load_bitmap (VDUNGEON_BROKEN_FLOOR_RIGHT);
  broken_floor_front = load_bitmap (VDUNGEON_BROKEN_FLOOR_FRONT);
  floor_corner_01 = load_bitmap (VDUNGEON_FLOOR_CORNER_01);
  floor_corner_02 = load_bitmap (VDUNGEON_FLOOR_CORNER_02);
  floor_corner_03 = load_bitmap (VDUNGEON_FLOOR_CORNER_03);
  spikes_floor_left = load_bitmap (VDUNGEON_SPIKES_FLOOR_LEFT);
  spikes_floor_right = load_bitmap (VDUNGEON_SPIKES_FLOOR_RIGHT);
  opener_floor_left = load_bitmap (VDUNGEON_OPENER_FLOOR_LEFT);
  opener_floor_base = load_bitmap (VDUNGEON_OPENER_FLOOR_BASE);
}

void
unload_floor (void)
{
  /* bitmaps */
  al_destroy_bitmap (normal_floor_left);
  al_destroy_bitmap (normal_floor_right);
  al_destroy_bitmap (normal_floor_base);
  al_destroy_bitmap (broken_floor_left);
  al_destroy_bitmap (broken_floor_right);
  al_destroy_bitmap (broken_floor_front);
  al_destroy_bitmap (floor_corner_01);
  al_destroy_bitmap (floor_corner_02);
  al_destroy_bitmap (floor_corner_03);
  al_destroy_bitmap (spikes_floor_left);
  al_destroy_bitmap (spikes_floor_right);
  al_destroy_bitmap (opener_floor_left);
  al_destroy_bitmap (opener_floor_base);
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
}

void
draw_broken_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (broken_floor_left, bitmap, floor_left_coord (p), 0);
  draw_bitmapc (broken_floor_right, bitmap, floor_right_coord (p), 0);
}

void
draw_broken_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (broken_floor_left, bitmap, floor_left_coord (p), 0);
}

void
draw_broken_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (broken_floor_right, bitmap, floor_right_coord (p), 0);
}

void
draw_broken_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (broken_floor_front, bitmap,
                  broken_floor_front_coord (p), 0);
}

void
draw_spikes_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (spikes_floor_left, bitmap, floor_left_coord (p), 0);
  draw_bitmapc (spikes_floor_right, bitmap, floor_right_coord (p), 0);
}

void
draw_spikes_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (spikes_floor_left, bitmap, floor_left_coord (p), 0);
}

void
draw_spikes_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (spikes_floor_right, bitmap, floor_right_coord (p), 0);
}

void
draw_opener_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (opener_floor_base, bitmap, floor_base_coord (p), 0);
}

void
draw_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_opener_floor_base (bitmap, p);
  draw_bitmapc (opener_floor_left, bitmap, opener_floor_left_coord (p), 0);
  draw_bitmapc (normal_floor_right, bitmap, opener_floor_right_coord (p), 0);

  register_opener_floor (p);
}

void
draw_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_opener_floor_base (bitmap, p);
  draw_bitmapc (opener_floor_left, bitmap, opener_floor_left_coord (p), 0);
}

void
draw_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_opener_floor_base (bitmap, p);
  draw_bitmapc (normal_floor_right, bitmap, opener_floor_right_coord (p), 0);
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
      && is_kid_fall ())
    switch (construct (p).fg) {
    case FLOOR:
      draw_floor_left (bitmap, p);
      break;
    case LOOSE_FLOOR:
      draw_floor_left (bitmap, p);
      break;
    case OPENER_FLOOR:
      draw_opener_floor_left (bitmap, p);
      break;
    default:
      error (-1, 0, "%s: unknown floor type (%i)",
             __func__, construct (p).fg);
    }
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
    switch (construct (p).fg) {
    case FLOOR:
      draw_floor (bitmap, p);
      break;
    case LOOSE_FLOOR:
      draw_floor (bitmap, p);
      break;
    case OPENER_FLOOR:
      draw_opener_floor (bitmap, p);
      break;
    default:
      error (-1, 0, "%s: unknown floor type (%i)",
             __func__, construct (p).fg);
    }
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
opener_floor_left_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 50 - 1;
  c.room = p.room;
  return c;
}

struct coord
opener_floor_right_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 50 - 1;
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

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

#include <error.h>
#include "kernel/video.h"
#include "physics.h"
#include "kid.h"
#include "pillar.h"
#include "wall.h"
#include "floor.h"

ALLEGRO_BITMAP *floor_normal_left, *floor_normal_right, *floor_normal_base,
  *floor_broken_left, *floor_broken_right, *floor_broken_front;

void
load_vdungeon_floor (void)
{
  floor_normal_left = load_bitmap (VDUNGEON_FLOOR_NORMAL_LEFT);
  floor_normal_right = load_bitmap (VDUNGEON_FLOOR_NORMAL_RIGHT);
  floor_normal_base = load_bitmap (VDUNGEON_FLOOR_NORMAL_BASE);
  floor_broken_left = load_bitmap (VDUNGEON_FLOOR_BROKEN_LEFT);
  floor_broken_right = load_bitmap (VDUNGEON_FLOOR_BROKEN_RIGHT);
  floor_broken_front = load_bitmap (VDUNGEON_FLOOR_BROKEN_FRONT);
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
}

void
draw_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
  draw_bitmap_xy (floor_normal_left, bitmap, floor_left_xy (p), 0);
  draw_bitmap_xy (floor_normal_right, bitmap, floor_right_xy (p), 0);
}

void
draw_broken_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
  draw_bitmap_xy (floor_broken_left, bitmap, floor_left_xy (p), 0);
  draw_bitmap_xy (floor_broken_right, bitmap, floor_right_xy (p), 0);
}

void
draw_broken_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_broken_front, bitmap,
                  broken_floor_front_xy (p), 0);
}

void
draw_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  /* fix a bug where the floor would be redraw over the last kid
     stabilization frame when turning on the edge */
  if (kid.frame == kid_stabilize_08) return;

  /* don't redraw floors while the kid is on them */
  if ((! is_falling (kid) && ! is_kid_fall () && ! is_kid_vjump ())

      /* don't draw floors over kid's foots when jumping from the
         edge */
      || is_kid_start_jump ()) return;

  draw_floor (bitmap, p);

  /* draw next floor left side to cover right edge of the current
     one */
  p.place++;
  switch (construct (p).fg) {
  case NO_FLOOR:
    break;
  case FLOOR:
    draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
    draw_bitmap_xy (floor_normal_left, bitmap, floor_left_xy (p), 0);
    break;
  case BROKEN_FLOOR:
    draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
    draw_bitmap_xy (floor_broken_left, bitmap, floor_left_xy (p), 0);
    break;
  case LOOSE_FLOOR:
    draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
    draw_bitmap_xy (floor_normal_left, bitmap, floor_left_xy (p), 0);
    break;
  case PILLAR:
    draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
    draw_bitmap_xy (pillar_left, bitmap, pillar_left_xy (p), 0);
    break;
  case WALL:
    draw_wall (bitmap, p);
    break;
  default:
    error (-1, 0, "%s: unknown foreground (%u)",
           __func__, construct (p).fg);
  }
}

struct xy
floor_base_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * p.place;
  xy.y = PLACE_HEIGHT * (p.floor + 1);
  return xy;
}

struct xy
floor_left_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * p.place;
  xy.y = PLACE_HEIGHT * p.floor + 50;
  return xy;
}

struct xy
floor_right_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * (p.place + 1);
  xy.y = PLACE_HEIGHT * p.floor + 50;
  return xy;
}

struct xy
broken_floor_front_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * p.place;
  xy.y = PLACE_HEIGHT * p.floor + 54;
  return xy;
}

/*
  pillar.c -- pillar module;

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
#include "floor.h"
#include "pillar.h"

ALLEGRO_BITMAP *pillar_left, *pillar_right, *pillar_top, *pillar;

void
load_vdungeon_pillar (void)
{
  pillar_left = load_bitmap (VDUNGEON_PILLAR_LEFT);
  pillar_right = load_bitmap (VDUNGEON_PILLAR_RIGHT);
  pillar_top = load_bitmap (VDUNGEON_PILLAR_TOP);
  pillar = load_bitmap (VDUNGEON_PILLAR);
}

void
unload_pillar (void)
{
  /* bitmaps */
  al_destroy_bitmap (pillar_left);
  al_destroy_bitmap (pillar_right);
  al_destroy_bitmap (pillar_top);
  al_destroy_bitmap (pillar);
}

void
draw_pillar (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
  draw_bitmap_xy (pillar_left, bitmap, pillar_left_xy (p), 0);
  draw_bitmap_xy (pillar_right, bitmap, pillar_right_xy (p), 0);
  draw_bitmap_xy (pillar_top, bitmap, pillar_top_xy (p), 0);
}

void
draw_pillar_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
  draw_bitmap_xy (pillar_left, bitmap, pillar_left_xy (p), 0);
}

void
draw_pillar_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
  draw_bitmap_xy (pillar_right, bitmap, pillar_right_xy (p), 0);
}

void
draw_pillar_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (pillar, screen, pillar_xy (p), 0);
}

struct xy
pillar_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * p.place + 8;
  xy.y = PLACE_HEIGHT * p.floor + 3;
  return xy;
}

struct xy
pillar_left_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * p.place;
  xy.y = PLACE_HEIGHT * p.floor + 3;
  return xy;
}

struct xy
pillar_right_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * (p.place + 1);
  xy.y = PLACE_HEIGHT * p.floor + 3;
  return xy;
}

struct xy
pillar_top_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * (p.place + 1);
  xy.y = PLACE_HEIGHT * p.floor - 4;
  return xy;
}

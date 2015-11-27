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
#include "room.h"
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
  draw_bitmapc (normal_floor_base, bitmap, floor_base_coord (p), 0);
  draw_bitmapc (pillar_left, bitmap, pillar_left_coord (p), 0);
  draw_bitmapc (pillar_right, bitmap, pillar_right_coord (p), 0);
  draw_con_left (bitmap, prel (p, 0, +1));
  draw_bitmapc (pillar_top, bitmap, pillar_top_coord (p), 0);
  draw_con_left (bitmap, prel (p, -1, +1));
}

void
draw_pillar_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (normal_floor_base, bitmap, floor_base_coord (p), 0);
}

void
draw_pillar_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_pillar_base (bitmap, p);
  draw_bitmapc (pillar_left, bitmap, pillar_left_coord (p), 0);
}

void
draw_pillar_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_pillar_base (bitmap, p);
  draw_bitmapc (pillar_right, bitmap, pillar_right_coord (p), 0);
  draw_con_left (bitmap, prel (p, 0, +1));
}

void
draw_pillar_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_pillar_base (bitmap, p);
  draw_floor_corner_03 (bitmap, p);
  draw_bitmapc (pillar, screen, pillar_coord (p), 0);
}

struct coord
pillar_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place + 8;
  c.y = PLACE_HEIGHT * p.floor + 3;
  c.room = p.room;
  return c;
}

struct coord
pillar_left_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 3;
  c.room = p.room;
  return c;
}

struct coord
pillar_right_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 3;
  c.room = p.room;
  return c;
}

struct coord
pillar_top_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor - 4;
  c.room = p.room;
  return c;
}

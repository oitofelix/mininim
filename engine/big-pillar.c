/*
  big-pillar.c -- big pillar module;

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
#include "big-pillar.h"

ALLEGRO_BITMAP *big_pillar_bottom_left, *big_pillar_bottom_right,
  *big_pillar_top_left, *big_pillar_top_right, *big_pillar_top_right_top,
  *big_pillar_bottom_fg;

void
load_vdungeon_big_pillar (void)
{
  big_pillar_bottom_left = load_bitmap (VDUNGEON_BIG_PILLAR_BOTTOM_LEFT);
  big_pillar_bottom_right = load_bitmap (VDUNGEON_BIG_PILLAR_BOTTOM_RIGHT);
  big_pillar_top_left = load_bitmap (VDUNGEON_BIG_PILLAR_TOP_LEFT);
  big_pillar_top_right = load_bitmap (VDUNGEON_BIG_PILLAR_TOP_RIGHT);
  big_pillar_top_right_top = load_bitmap (VDUNGEON_BIG_PILLAR_TOP_RIGHT_TOP);
  big_pillar_bottom_fg = load_bitmap (VDUNGEON_BIG_PILLAR_BOTTOM_FG);
}

void
unload_big_pillar (void)
{
  /* bitmaps */
  al_destroy_bitmap (big_pillar_bottom_left);
  al_destroy_bitmap (big_pillar_bottom_right);
  al_destroy_bitmap (big_pillar_top_left);
  al_destroy_bitmap (big_pillar_top_right);
  al_destroy_bitmap (big_pillar_top_right_top);
  al_destroy_bitmap (big_pillar_bottom_fg);
}

void
draw_big_pillar_bottom (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_floor_base (bitmap, p);
  draw_big_pillar_bottom_left (bitmap, p);
  draw_big_pillar_bottom_right (bitmap, p);
}

void
draw_big_pillar_bottom_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (big_pillar_bottom_left, bitmap,
                big_pillar_bottom_left_coord (p, &c), 0);
}

void
draw_big_pillar_bottom_right (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (big_pillar_bottom_right, bitmap,
                big_pillar_bottom_right_coord (p, &c), 0);
}

void
draw_big_pillar_bottom_fg (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_floor_corner_03 (bitmap, p);
  draw_bitmapc (big_pillar_bottom_fg, bitmap, big_pillar_bottom_fg_coord (p, &c), 0);
}

void
draw_big_pillar_top (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_floor_base (bitmap, p);
  draw_big_pillar_top_left (bitmap, p);
  draw_big_pillar_top_right (bitmap, p);
}

void
draw_big_pillar_top_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (big_pillar_top_left, bitmap,
                big_pillar_top_left_coord (p, &c), 0);
}

void
draw_big_pillar_top_right (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (big_pillar_top_right, bitmap,
                big_pillar_top_right_coord (p, &c), 0);
  draw_bitmapc (big_pillar_top_right_top, bitmap,
                big_pillar_top_right_top_coord (p, &c), 0);
}

struct coord *
big_pillar_bottom_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
big_pillar_bottom_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
big_pillar_top_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 8;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
big_pillar_top_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
big_pillar_top_right_top_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 3;
  c->room = p->room;
  return c;
}

struct coord *
big_pillar_bottom_fg_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 8;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

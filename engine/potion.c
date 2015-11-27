/*
  potion.c -- potion module;

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
#include "level.h"
#include "physics.h"
#include "room.h"
#include "potion.h"

static ALLEGRO_BITMAP *small_potion, *big_potion,
  *bubble_01, *bubble_02, *bubble_03, *bubble_04, *bubble_05,
  *bubble_06, *bubble_07;

static struct coord small_potion_coord (struct pos p);
static struct coord small_potion_bubble_coord (struct pos p);
static struct coord big_potion_coord (struct pos p);
static struct coord big_potion_bubble_coord (struct pos p);
ALLEGRO_COLOR life_palette (ALLEGRO_COLOR c);

void
load_potion (void)
{
  switch (level->type) {
  case DUNGEON:
    small_potion = load_bitmap (VDUNGEON_SMALL_POTION);
    big_potion = load_bitmap (VDUNGEON_BIG_POTION);
    break;
  case PALACE: break;
  }

  bubble_01 = load_bitmap (BUBBLE_01);
  bubble_02 = load_bitmap (BUBBLE_02);
  bubble_03 = load_bitmap (BUBBLE_03);
  bubble_04 = load_bitmap (BUBBLE_04);
  bubble_05 = load_bitmap (BUBBLE_05);
  bubble_06 = load_bitmap (BUBBLE_06);
  bubble_07 = load_bitmap (BUBBLE_07);
}

void
unload_potion (void)
{
  al_destroy_bitmap (small_potion);
  al_destroy_bitmap (big_potion);
  al_destroy_bitmap (bubble_01);
  al_destroy_bitmap (bubble_02);
  al_destroy_bitmap (bubble_03);
  al_destroy_bitmap (bubble_04);
  al_destroy_bitmap (bubble_05);
  al_destroy_bitmap (bubble_06);
  al_destroy_bitmap (bubble_07);
}

static ALLEGRO_BITMAP *
get_bubble_frame (int i)
{
  switch (i) {
  case 0: return bubble_01;
  case 1: return bubble_02;
  case 2: return bubble_03;
  case 3: return bubble_04;
  case 4: return bubble_05;
  case 5: return bubble_06;
  case 6: return bubble_07;
  default:
    error (-1, 0, "%s (%i): unknown bubble frame", __func__, i);
  }

  return NULL;
}

void
draw_potion (ALLEGRO_BITMAP *bitmap, struct pos p, int i)
{
  if (! is_potion (p)) return;

  ALLEGRO_BITMAP *bottle, *bubble;
  struct coord bottle_coord, bubble_coord;
  palette bubble_palette;

  seedp (p);
  enum item item = con (p)->ext.item;
  switch (item) {
  case SMALL_LIFE_POTION:
    bottle = small_potion;
    bottle_coord = small_potion_coord (p);
    bubble_palette = life_palette;
    bubble_coord = small_potion_bubble_coord (p);
    break;
  case BIG_LIFE_POTION:
    bottle = big_potion;
    bottle_coord = big_potion_coord (p);
    bubble_palette = life_palette;
    bubble_coord = big_potion_bubble_coord (p);
    break;
  default:
    error (-1, 0, "%s (%i): unknown potion type", __func__, item);
    break;
  }
  draw_bitmapc (bottle, bitmap, bottle_coord,
                prandom (1) ? ALLEGRO_FLIP_HORIZONTAL : 0);
  bubble = get_bubble_frame (i % 7);
  bubble = apply_palette (bubble, bubble_palette);
  int r = prandom (1);
  bubble_coord.x -= r;
  draw_bitmapc (bubble, bitmap, bubble_coord,
                r ? ALLEGRO_FLIP_HORIZONTAL : 0);
  al_destroy_bitmap (bubble);
  unseedp ();
}

bool
is_potion (struct pos p)
{
  return con (p)->fg == FLOOR
    && (con (p)->ext.item == SMALL_LIFE_POTION
        || con (p)->ext.item == BIG_LIFE_POTION);
}

struct coord
small_potion_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1) - 10;
  c.y = PLACE_HEIGHT * p.floor + 49;
  c.room = p.room;
  return c;
}

struct coord
small_potion_bubble_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1) - 7;
  c.y = PLACE_HEIGHT * p.floor + 40;
  c.room = p.room;
  return c;
}

struct coord
big_potion_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1) - 10;
  c.y = PLACE_HEIGHT * p.floor + 45;
  c.room = p.room;
  return c;
}

struct coord
big_potion_bubble_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1) - 7;
  c.y = PLACE_HEIGHT * p.floor + 36;
  c.room = p.room;
  return c;
}

ALLEGRO_COLOR
life_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return RED;
  else return c;
}

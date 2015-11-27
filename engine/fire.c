/*
  fire.c -- fire module;

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
#include "physics.h"
#include "level.h"
#include "pos.h"
#include "room.h"
#include "fire.h"

static ALLEGRO_BITMAP *fire_01, *fire_02, *fire_03, *fire_04, *fire_05,
  *fire_06, *fire_07, *fire_08, *fire_09;

void
load_fire (void)
{
  fire_01 = load_bitmap (FIRE_01);
  fire_02 = load_bitmap (FIRE_02);
  fire_03 = load_bitmap (FIRE_03);
  fire_04 = load_bitmap (FIRE_04);
  fire_05 = load_bitmap (FIRE_05);
  fire_06 = load_bitmap (FIRE_06);
  fire_07 = load_bitmap (FIRE_07);
  fire_08 = load_bitmap (FIRE_08);
  fire_09 = load_bitmap (FIRE_09);
}

void
unload_fire (void)
{
  /* bitmaps */
  al_destroy_bitmap (fire_01);
  al_destroy_bitmap (fire_02);
  al_destroy_bitmap (fire_03);
  al_destroy_bitmap (fire_04);
  al_destroy_bitmap (fire_05);
  al_destroy_bitmap (fire_06);
  al_destroy_bitmap (fire_07);
  al_destroy_bitmap (fire_08);
  al_destroy_bitmap (fire_09);
}

static ALLEGRO_BITMAP *
get_fire_frame (int i)
{
  switch (i) {
  case 0: return fire_01;
  case 1: return fire_02;
  case 2: return fire_03;
  case 3: return fire_04;
  case 4: return fire_05;
  case 5: return fire_06;
  case 6: return fire_07;
  case 7: return fire_08;
  case 8: return fire_09;
  default:
    error (-1, 0, "%s (%i): unknown fire frame", __func__, i);
  }

  return NULL;
}

void
draw_fire (ALLEGRO_BITMAP* bitmap, struct pos p, int i)
{
  if (con (p)->bg != TORCH) return;

  enum confg fg = con (p)->fg;

  /* consistency: wall, pillars and doors can't have background */
  if (fg == WALL || fg == PILLAR || fg == DOOR) return;

  ALLEGRO_BITMAP *fire = get_fire_frame (prandom_pos (p, i, 1, 8));
  draw_bitmapc (fire, bitmap, fire_coord (p),
                prandom (1) ? ALLEGRO_FLIP_HORIZONTAL : 0);

  i++;
}

void
draw_princess_room_fire (void)
{
  static int i = 0;

  ALLEGRO_BITMAP *fire_0 = get_fire_frame (prandom_uniq (FIRE_RANDOM_SEED_0 + i, 1, 8));
  ALLEGRO_BITMAP *fire_1 = get_fire_frame (prandom_uniq (FIRE_RANDOM_SEED_1 + i, 1, 8));

  draw_bitmap (fire_0, screen, 92, 99, prandom (1) ? ALLEGRO_FLIP_HORIZONTAL : 0);
  draw_bitmap (fire_1, screen, 210, 99, prandom (1) ? ALLEGRO_FLIP_HORIZONTAL : 0);

  i++;
}

struct coord
fire_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1) + 8;
  c.y = PLACE_HEIGHT * p.floor + 4;
  c.room = p.room;
  return c;
}

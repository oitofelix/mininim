/*
  sword.c -- sword module;

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
#include "kernel/random.h"
#include "level.h"
#include "physics.h"
#include "room.h"
#include "sword.h"

struct frameset sword_frameset[SWORD_FRAMESET_NMEMB];
static void init_sword_frameset (void);

static ALLEGRO_BITMAP *normal_sword, *shiny_sword,
  *sword_01, *sword_02, *sword_03, *sword_04, *sword_05, *sword_06, *sword_07,
  *sword_08, *sword_09, *sword_10, *sword_11, *sword_12, *sword_13, *sword_14,
  *sword_15, *sword_16, *sword_17, *sword_18, *sword_19, *sword_20, *sword_21,
  *sword_22, *sword_23, *sword_24, *sword_25, *sword_26, *sword_27, *sword_28,
  *sword_29, *sword_30, *sword_31, *sword_32, *sword_33, *sword_34;

static struct coord sword_coord (struct pos p);

void
load_sword (void)
{
  /* bitmaps */
  normal_sword = load_bitmap (NORMAL_SWORD);
  shiny_sword = load_bitmap (SHINY_SWORD);
  sword_01 = load_bitmap (SWORD_01);
  sword_02 = load_bitmap (SWORD_02);
  sword_03 = load_bitmap (SWORD_03);
  sword_04 = load_bitmap (SWORD_04);
  sword_05 = load_bitmap (SWORD_05);
  sword_06 = load_bitmap (SWORD_06);
  sword_07 = load_bitmap (SWORD_07);
  sword_08 = load_bitmap (SWORD_08);
  sword_09 = load_bitmap (SWORD_09);
  sword_10 = load_bitmap (SWORD_10);
  sword_11 = load_bitmap (SWORD_11);
  sword_12 = load_bitmap (SWORD_12);
  sword_13 = load_bitmap (SWORD_13);
  sword_14 = load_bitmap (SWORD_14);
  sword_15 = load_bitmap (SWORD_15);
  sword_16 = load_bitmap (SWORD_16);
  sword_17 = load_bitmap (SWORD_17);
  sword_18 = load_bitmap (SWORD_18);
  sword_19 = load_bitmap (SWORD_19);
  sword_20 = load_bitmap (SWORD_20);
  sword_21 = load_bitmap (SWORD_21);
  sword_22 = load_bitmap (SWORD_22);
  sword_23 = load_bitmap (SWORD_23);
  sword_24 = load_bitmap (SWORD_24);
  sword_25 = load_bitmap (SWORD_25);
  sword_26 = load_bitmap (SWORD_26);
  sword_27 = load_bitmap (SWORD_27);
  sword_28 = load_bitmap (SWORD_28);
  sword_29 = load_bitmap (SWORD_29);
  sword_30 = load_bitmap (SWORD_30);
  sword_31 = load_bitmap (SWORD_31);
  sword_32 = load_bitmap (SWORD_32);
  sword_33 = load_bitmap (SWORD_33);
  sword_34 = load_bitmap (SWORD_34);

  /* frameset */
  init_sword_frameset ();
}

void
unload_sword (void)
{
  al_destroy_bitmap (normal_sword);
  al_destroy_bitmap (shiny_sword);
  al_destroy_bitmap (sword_01);
  al_destroy_bitmap (sword_02);
  al_destroy_bitmap (sword_03);
  al_destroy_bitmap (sword_04);
  al_destroy_bitmap (sword_05);
  al_destroy_bitmap (sword_06);
  al_destroy_bitmap (sword_07);
  al_destroy_bitmap (sword_08);
  al_destroy_bitmap (sword_09);
  al_destroy_bitmap (sword_10);
  al_destroy_bitmap (sword_11);
  al_destroy_bitmap (sword_12);
  al_destroy_bitmap (sword_13);
  al_destroy_bitmap (sword_14);
  al_destroy_bitmap (sword_15);
  al_destroy_bitmap (sword_16);
  al_destroy_bitmap (sword_17);
  al_destroy_bitmap (sword_18);
  al_destroy_bitmap (sword_19);
  al_destroy_bitmap (sword_20);
  al_destroy_bitmap (sword_21);
  al_destroy_bitmap (sword_22);
  al_destroy_bitmap (sword_23);
  al_destroy_bitmap (sword_24);
  al_destroy_bitmap (sword_25);
  al_destroy_bitmap (sword_26);
  al_destroy_bitmap (sword_27);
  al_destroy_bitmap (sword_28);
  al_destroy_bitmap (sword_29);
  al_destroy_bitmap (sword_30);
  al_destroy_bitmap (sword_31);
  al_destroy_bitmap (sword_32);
  al_destroy_bitmap (sword_33);
  al_destroy_bitmap (sword_34);
}

void
init_sword_frameset (void)
{
  struct frameset frameset[SWORD_FRAMESET_NMEMB] =
    {{sword_01,+0,+0},{sword_02,+0,+0},{sword_03,+0,+0},
     {sword_04,+0,+0},{sword_05,+0,+0},{sword_06,+0,+0},
     {sword_07,+0,+0},{sword_08,+0,+0},{sword_09,+0,+0},
     {sword_10,+0,+0},{sword_11,-11,+13},{sword_12,+0,+0},
     {sword_13,+0,+0},{sword_14,-28,+10},{sword_15,+0,+0},
     {sword_16,+0,+0},{sword_17,+0,+0},{sword_18,-15,+15},
     {sword_19,+0,+0},{sword_20,+0,+0},{sword_21,-7,-11},
     {sword_22,-14,-11},{sword_23,-14,-11},{sword_24,-14,-5},
     {sword_25,-28,+3},{sword_26,-28,+8},{sword_27,-21,+7},
     {sword_28,-14,+5},{sword_29,+0,+0},{sword_30,+0,+0},
     {sword_31,+0,+0},{sword_32,+0,+0},{sword_33,+0,+0},
     {sword_34,+0,+0}};

  memcpy (&sword_frameset, &frameset,
          SWORD_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
draw_sword (ALLEGRO_BITMAP *bitmap, struct pos p, int i)
{
  if (! is_sword (p)) return;
  ALLEGRO_BITMAP *sword = i % 60 ? normal_sword : shiny_sword;
  seedp (p);
  draw_bitmapc (sword, bitmap, sword_coord (p),
                prandom (1) ? ALLEGRO_FLIP_HORIZONTAL : 0);
  unseedp ();
}

bool
is_sword (struct pos p)
{
  return con (p)->fg == FLOOR
    && con (p)->ext.item == SWORD;
}

struct coord
sword_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 50;
  c.room = p.room;
  return c;
}

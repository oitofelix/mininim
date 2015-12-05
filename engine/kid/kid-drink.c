/*
  kid-drink.c -- kid drink module;

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

#include "prince.h"
#include "kernel/video.h"
#include "kernel/keyboard.h"
#include "engine/anim.h"
#include "engine/physics.h"
#include "engine/door.h"
#include "engine/potion.h"
#include "engine/sword.h"
#include "engine/loose-floor.h"
#include "kid.h"

struct frameset kid_drink_frameset[KID_DRINK_FRAMESET_NMEMB];

static void init_kid_drink_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_drink_01, *kid_drink_02, *kid_drink_03,
  *kid_drink_04, *kid_drink_05, *kid_drink_06, *kid_drink_07,
  *kid_drink_08, *kid_drink_09, *kid_drink_10, *kid_drink_11,
  *kid_drink_12, *kid_drink_13, *kid_drink_14, *kid_drink_15;

static void
init_kid_drink_frameset (void)
{
  struct frameset frameset[KID_DRINK_FRAMESET_NMEMB] =
    {{kid_drink_01,-7,0},{kid_drink_02,+1,+1},{kid_drink_03,+1,-1},
     {kid_drink_04,+0,0},{kid_drink_05,+2,0},{kid_drink_06,-1,0},
     {kid_drink_07,+1,0},{kid_drink_08,+6,0},{kid_drink_09,-1,0},
     {kid_drink_10,+2,-1},{kid_drink_11,-2,+1},{kid_drink_12,+0,-1},
     {kid_drink_13,-1,0},{kid_drink_14,+1,0},{kid_drink_15,+1,0}};

  memcpy (&kid_drink_frameset, &frameset,
          KID_DRINK_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_drink (void)
{
  /* bitmaps */
  kid_drink_01 = load_bitmap (KID_DRINK_01);
  kid_drink_02 = load_bitmap (KID_DRINK_02);
  kid_drink_03 = load_bitmap (KID_DRINK_03);
  kid_drink_04 = load_bitmap (KID_DRINK_04);
  kid_drink_05 = load_bitmap (KID_DRINK_05);
  kid_drink_06 = load_bitmap (KID_DRINK_06);
  kid_drink_07 = load_bitmap (KID_DRINK_07);
  kid_drink_08 = load_bitmap (KID_DRINK_08);
  kid_drink_09 = load_bitmap (KID_DRINK_09);
  kid_drink_10 = load_bitmap (KID_DRINK_10);
  kid_drink_11 = load_bitmap (KID_DRINK_11);
  kid_drink_12 = load_bitmap (KID_DRINK_12);
  kid_drink_13 = load_bitmap (KID_DRINK_13);
  kid_drink_14 = load_bitmap (KID_DRINK_14);
  kid_drink_15 = load_bitmap (KID_DRINK_15);

  /* frameset */
  init_kid_drink_frameset ();
}

void
unload_kid_drink (void)
{
  al_destroy_bitmap (kid_drink_01);
  al_destroy_bitmap (kid_drink_02);
  al_destroy_bitmap (kid_drink_03);
  al_destroy_bitmap (kid_drink_04);
  al_destroy_bitmap (kid_drink_05);
  al_destroy_bitmap (kid_drink_06);
  al_destroy_bitmap (kid_drink_07);
  al_destroy_bitmap (kid_drink_08);
  al_destroy_bitmap (kid_drink_09);
  al_destroy_bitmap (kid_drink_10);
  al_destroy_bitmap (kid_drink_11);
  al_destroy_bitmap (kid_drink_12);
  al_destroy_bitmap (kid_drink_13);
  al_destroy_bitmap (kid_drink_14);
  al_destroy_bitmap (kid_drink_15);
}

void
kid_drink (void)
{
  kid.oaction = kid.action;
  kid.action = kid_drink;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_drink)
    kid->i = -1, kid->wait = 4, kid->reverse = false;

  if (kid->i < 14 && ! kid->reverse) kid->i++;
  else if (kid->wait > 0) kid->wait--;
  else if (kid->i == 14 && kid->wait == 0) kid->reverse = true, kid->i = 10;
  else if (kid->i == 10 && kid->reverse) kid->i = 7;
  else {
    kid_normal ();
    item_pos.room = -1;
    return false;
  }

  select_frame (kid, kid_drink_frameset, kid->i);

  if (kid->i == 14 && kid->wait < 4) kid->fo.dx = 0;
  if (kid->i == 10 && kid->reverse) kid->fo.dx = -2, kid->fo.dy = +1;
  if (kid->i == 7 && kid->reverse) kid->fo.dx = +1;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  keep_depressible_floor (kid);

  /* sound */
  if (kid->i == 7 && ! kid->reverse) sample_drink = true;

  /* consume bottle */
  if (kid->i == 0) con (&item_pos)->ext.item = NO_ITEM;
}

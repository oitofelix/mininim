/*
  kid-keep-sword.c -- kid keep sword module;

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

struct frameset kid_keep_sword_frameset[KID_KEEP_SWORD_FRAMESET_NMEMB];

static void init_kid_keep_sword_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_keep_sword_01, *kid_keep_sword_02,
  *kid_keep_sword_03, *kid_keep_sword_04, *kid_keep_sword_05,
  *kid_keep_sword_06, *kid_keep_sword_07, *kid_keep_sword_08,
  *kid_keep_sword_09, *kid_keep_sword_10;

static void
init_kid_keep_sword_frameset (void)
{
  struct frameset frameset[KID_KEEP_SWORD_FRAMESET_NMEMB] =
    {{kid_keep_sword_01,+2,0},{kid_keep_sword_02,-2,0},{kid_keep_sword_03,+0,0},
     {kid_keep_sword_04,+1,0},{kid_keep_sword_05,-6,0},{kid_keep_sword_06,+2,0},
     {kid_keep_sword_07,+3,0},{kid_keep_sword_08,+0,0},{kid_keep_sword_09,+3,0},
     {kid_keep_sword_10,+0,0}};

  memcpy (&kid_keep_sword_frameset, &frameset,
          KID_KEEP_SWORD_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_keep_sword (void)
{
  /* bitmaps */
  kid_keep_sword_01 = load_bitmap (KID_KEEP_SWORD_01);
  kid_keep_sword_02 = load_bitmap (KID_KEEP_SWORD_02);
  kid_keep_sword_03 = load_bitmap (KID_KEEP_SWORD_03);
  kid_keep_sword_04 = load_bitmap (KID_KEEP_SWORD_04);
  kid_keep_sword_05 = load_bitmap (KID_KEEP_SWORD_05);
  kid_keep_sword_06 = load_bitmap (KID_KEEP_SWORD_06);
  kid_keep_sword_07 = load_bitmap (KID_KEEP_SWORD_07);
  kid_keep_sword_08 = load_bitmap (KID_KEEP_SWORD_08);
  kid_keep_sword_09 = load_bitmap (KID_KEEP_SWORD_09);
  kid_keep_sword_10 = load_bitmap (KID_KEEP_SWORD_10);

  /* frameset */
  init_kid_keep_sword_frameset ();
}

void
unload_kid_keep_sword (void)
{
  al_destroy_bitmap (kid_keep_sword_01);
  al_destroy_bitmap (kid_keep_sword_02);
  al_destroy_bitmap (kid_keep_sword_03);
  al_destroy_bitmap (kid_keep_sword_04);
  al_destroy_bitmap (kid_keep_sword_05);
  al_destroy_bitmap (kid_keep_sword_06);
  al_destroy_bitmap (kid_keep_sword_07);
  al_destroy_bitmap (kid_keep_sword_08);
  al_destroy_bitmap (kid_keep_sword_09);
  al_destroy_bitmap (kid_keep_sword_10);
}

void
kid_keep_sword (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_keep_sword;
  kid->f.flip = (kid->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame (&kid->f, &kid->f, &kid->fo);
  physics_out (kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_keep_sword) kid->i = -1, kid->wait = 1;

  if (kid->i < 8) kid->i++;
  else if (kid->i == 8 && kid->wait > 0) kid->wait--;
  else if (kid->i == 8 && kid->wait == 0) kid->i++, kid->wait = 2;
  else if (kid->i == 9 && kid->wait > 0) kid->wait--;
  else {
    if (kid->keep_sword_fast) {
      kid_normal (kid);
      kid->keep_sword_fast = false;
      return false;
    }
    else {
      /* kid turn will invert kid's direction */
      kid->f.dir = (kid->f.dir == RIGHT) ? LEFT : RIGHT;
      kid_turn (kid);
      return false;
    }
  }

  if (kid->keep_sword_fast && kid->i % 2 == 0) kid->i++, kid->wait = 0;

  kid->j = 24 + kid->i;

  select_frame (kid, kid_keep_sword_frameset, kid->i);
  if (kid->i < 4)
    select_xframe (kid, sword_frameset, kid->j);

  if (kid->i == 8 && kid->wait < 1) kid->fo.dx = 0;
  if (kid->i == 9 && kid->wait < 2) kid->fo.dx = 0;
  if (kid->keep_sword_fast && kid->i % 2)
    kid->fo.dx += kid_keep_sword_frameset[kid->i - 1].dx;
  if (kid->f.b == kid_sword_normal_08) kid->fo.dx = +8;

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
}

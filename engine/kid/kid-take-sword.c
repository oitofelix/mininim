/*
  kid-take-sword.c -- kid take sword module;

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

struct frameset kid_take_sword_frameset[KID_TAKE_SWORD_FRAMESET_NMEMB];

static void init_kid_take_sword_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_take_sword_01, *kid_take_sword_02,
  *kid_take_sword_03, *kid_take_sword_04;

static void
init_kid_take_sword_frameset (void)
{
  struct frameset frameset[KID_TAKE_SWORD_FRAMESET_NMEMB] =
    {{kid_take_sword_01,-6,0},{kid_take_sword_02,+0,0},
     {kid_take_sword_03,-4,0},{kid_take_sword_04,-6,0}};

  memcpy (&kid_take_sword_frameset, &frameset,
          KID_TAKE_SWORD_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_take_sword (void)
{
  /* bitmaps */
  kid_take_sword_01 = load_bitmap (KID_TAKE_SWORD_01);
  kid_take_sword_02 = load_bitmap (KID_TAKE_SWORD_02);
  kid_take_sword_03 = load_bitmap (KID_TAKE_SWORD_03);
  kid_take_sword_04 = load_bitmap (KID_TAKE_SWORD_04);

  /* frameset */
  init_kid_take_sword_frameset ();
}

void
unload_kid_take_sword (void)
{
  al_destroy_bitmap (kid_take_sword_01);
  al_destroy_bitmap (kid_take_sword_02);
  al_destroy_bitmap (kid_take_sword_03);
  al_destroy_bitmap (kid_take_sword_04);
}

void
kid_take_sword (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_take_sword;
  kid->f.flip = (kid->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame (&kid->f, &kid->f, &kid->fo);
  physics_out (kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_take_sword) kid->i = -1;

  if (kid->i == 3) {
    kid_sword_normal (kid);
    return false;
  }

  select_frame (kid, kid_take_sword_frameset, kid->i + 1);

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord tf; struct pos np, ptf;
  enum confg ctf;

  /* collision */
  if (is_colliding (&kid->f, &kid->fo, +PLACE_WIDTH, false, &kid->ci)
      && kid->i == 0) {
    /* sample_action_not_allowed = true; */
    kid_normal (kid);
    return false;
  }

  /* fall */
  ctf = survey (_tf, pos, &kid->f, &tf, &ptf, &np)->fg;
  if (ctf == NO_FLOOR) {
    kid_fall (kid);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* sound */
  if (kid->i == 0) sample_take_sword = true;

  /* depressible floors */
  if (kid->i == 3) update_depressible_floor (kid, -4, -23);
  keep_depressible_floor (kid);
}

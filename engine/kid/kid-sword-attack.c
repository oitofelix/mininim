/*
  kid-sword-attack.c -- kid sword attack module;

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

struct frameset kid_sword_attack_frameset[KID_SWORD_ATTACK_FRAMESET_NMEMB];

static void init_kid_sword_attack_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_sword_attack_01, *kid_sword_attack_02,
  *kid_sword_attack_03, *kid_sword_attack_04, *kid_sword_attack_05,
  *kid_sword_attack_06, *kid_sword_attack_07;

static void
init_kid_sword_attack_frameset (void)
{
  struct frameset frameset[KID_SWORD_ATTACK_FRAMESET_NMEMB] =
    {{kid_sword_attack_01,+1,0},{kid_sword_attack_02,-8,0},
     {kid_sword_attack_03,-8,0},{kid_sword_attack_04,-9,0},
     {kid_sword_attack_05,+8,0},{kid_sword_attack_06,+8,0},
     {kid_sword_attack_07,+8,0}};

  memcpy (&kid_sword_attack_frameset, &frameset,
          KID_SWORD_ATTACK_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_sword_attack (void)
{
  /* bitmaps */
  kid_sword_attack_01 = load_bitmap (KID_SWORD_ATTACK_01);
  kid_sword_attack_02 = load_bitmap (KID_SWORD_ATTACK_02);
  kid_sword_attack_03 = load_bitmap (KID_SWORD_ATTACK_03);
  kid_sword_attack_04 = load_bitmap (KID_SWORD_ATTACK_04);
  kid_sword_attack_05 = load_bitmap (KID_SWORD_ATTACK_05);
  kid_sword_attack_06 = load_bitmap (KID_SWORD_ATTACK_06);
  kid_sword_attack_07 = load_bitmap (KID_SWORD_ATTACK_07);

  /* frameset */
  init_kid_sword_attack_frameset ();
}

void
unload_kid_sword_attack (void)
{
  al_destroy_bitmap (kid_sword_attack_01);
  al_destroy_bitmap (kid_sword_attack_02);
  al_destroy_bitmap (kid_sword_attack_03);
  al_destroy_bitmap (kid_sword_attack_04);
  al_destroy_bitmap (kid_sword_attack_05);
  al_destroy_bitmap (kid_sword_attack_06);
  al_destroy_bitmap (kid_sword_attack_07);
}

void
kid_sword_attack (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_sword_attack;
  kid->f.flip = (kid->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame (&kid->f, &kid->f, &kid->fo);
  physics_out (kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_sword_attack) kid->i = -1;

  if (kid->i == 6) {
    kid_sword_normal (kid);
    return false;
  }

  if (kid->i == -1) kid->j = 1;
  if (kid->i == 1 || kid->i == 2) kid->j = 19;
  if (kid->i == 3 || kid->i == 5) kid->j = 17;
  if (kid->i == 4) kid->j = 7;

  select_frame (kid, kid_sword_attack_frameset, kid->i + 1);
  select_xframe (&kid->xf, sword_frameset, kid->j);

  if (kid->i == 1) kid->xf.b = NULL;
  if (kid->i == 3) kid->xf.dx = -21, kid->xf.dy = +7;
  if (kid->i == 4) kid->xf.dx = -7, kid->xf.dy = +17;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np, pbf, pmbo, pbb;

  /* collision */
  if (is_colliding (&kid->f, &kid->fo, +PLACE_WIDTH, false, &kid->ci)
      && kid->i == 0) {
    /* sample_action_not_allowed = true; */
    kid_sword_normal (kid);
    return false;
  }

  /* fall */
  survey (_bf, pos, &kid->f, &nc, &pbf, &np);
  survey (_mbo, pos, &kid->f, &nc, &pmbo, &np);
  survey (_bb, pos, &kid->f, &nc, &pbb, &np);
  if (is_strictly_traversable (&pbf)
      || is_strictly_traversable (&pmbo)
      || is_strictly_traversable (&pbb)) {
    kid->xf.b = NULL;
    kid_fall (kid);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* sound */
  if (kid->i == 3) play_sample (sword_attack_sample, kid->f.c.room);

  /* depressible floors */
  if (kid->i == 2) update_depressible_floor (kid, -8, -40);
  else if (kid->i == 3) update_depressible_floor (kid, -11, -47);
  else if (kid->i == 5) update_depressible_floor (kid, -4, -33);
  else if (kid->i == 6) update_depressible_floor (kid, -1, -24);
  else keep_depressible_floor (kid);
}

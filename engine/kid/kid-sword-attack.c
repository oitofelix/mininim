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
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

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
kid_sword_attack (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_sword_attack;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_sword_attack) k->i = -1;

  if (k->i == 6) {
    kid_sword_normal (k);
    return false;
  }

  if (k->i == -1) k->j = 1;
  if (k->i == 1 || k->i == 2) k->j = 19;
  if (k->i == 3 || k->i == 5) k->j = 17;
  if (k->i == 4) k->j = 7;

  select_frame (k, kid_sword_attack_frameset, k->i + 1);
  select_xframe (&k->xf, sword_frameset, k->j);

  if (k->i == 1) k->xf.b = NULL;
  if (k->i == 3) k->xf.dx = -21, k->xf.dy = +7;
  if (k->i == 4) k->xf.dx = -7, k->xf.dy = +17;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct coord nc; struct pos np, pbf, pmbo, pbb;

  /* collision */
  if (is_colliding (&k->f, &k->fo, +PLACE_WIDTH, false, &k->ci)
      && k->i == 0) {
    /* sample_action_not_allowed = true; */
    kid_sword_normal (k);
    return false;
  }

  /* fall */
  survey (_bf, pos, &k->f, &nc, &pbf, &np);
  survey (_mbo, pos, &k->f, &nc, &pmbo, &np);
  survey (_bb, pos, &k->f, &nc, &pbb, &np);
  if (is_strictly_traversable (&pbf)
      || is_strictly_traversable (&pmbo)
      || is_strictly_traversable (&pbb)) {
    k->xf.b = NULL;
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  /* sound */
  if (k->i == 3) play_sample (sword_attack_sample, k->f.c.room);

  /* depressible floors */
  if (k->i == 2) update_depressible_floor (k, -8, -40);
  else if (k->i == 3) update_depressible_floor (k, -11, -47);
  else if (k->i == 5) update_depressible_floor (k, -4, -33);
  else if (k->i == 6) update_depressible_floor (k, -1, -24);
  else keep_depressible_floor (k);
}

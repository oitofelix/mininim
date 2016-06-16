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

#include "mininim.h"

struct frameset kid_sword_attack_frameset[KID_SWORD_ATTACK_FRAMESET_NMEMB];

static void init_kid_sword_attack_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_sword_attack_00, *kid_sword_attack_01,
  *kid_sword_attack_02, *kid_sword_attack_03, *kid_sword_attack_04,
  *kid_sword_attack_05, *kid_sword_attack_06, *kid_sword_attack_counter_defense,
  *kid_sword_attack_defended;

static void
init_kid_sword_attack_frameset (void)
{
  struct frameset frameset[KID_SWORD_ATTACK_FRAMESET_NMEMB] =
    {{kid_sword_attack_00,+1,0},{kid_sword_attack_01,-8,0},
     {kid_sword_attack_02,-8,0},{kid_sword_attack_03,-9,0},
     {kid_sword_attack_04,+8,0},{kid_sword_attack_05,+8,0},
     {kid_sword_attack_06,+8,0},{kid_sword_attack_counter_defense,-8,0}};

  memcpy (&kid_sword_attack_frameset, &frameset,
          KID_SWORD_ATTACK_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_sword_attack (void)
{
  /* bitmaps */
  kid_sword_attack_00 = load_bitmap (KID_SWORD_ATTACK_00);
  kid_sword_attack_01 = load_bitmap (KID_SWORD_ATTACK_01);
  kid_sword_attack_02 = load_bitmap (KID_SWORD_ATTACK_02);
  kid_sword_attack_03 = load_bitmap (KID_SWORD_ATTACK_03);
  kid_sword_attack_04 = load_bitmap (KID_SWORD_ATTACK_04);
  kid_sword_attack_05 = load_bitmap (KID_SWORD_ATTACK_05);
  kid_sword_attack_06 = load_bitmap (KID_SWORD_ATTACK_06);
  kid_sword_attack_counter_defense = load_bitmap (KID_SWORD_ATTACK_COUNTER_DEFENSE);

  kid_sword_attack_defended = load_bitmap (KID_SWORD_ATTACK_DEFENDED);

  /* frameset */
  init_kid_sword_attack_frameset ();
}

void
unload_kid_sword_attack (void)
{
  destroy_bitmap (kid_sword_attack_00);
  destroy_bitmap (kid_sword_attack_01);
  destroy_bitmap (kid_sword_attack_02);
  destroy_bitmap (kid_sword_attack_03);
  destroy_bitmap (kid_sword_attack_04);
  destroy_bitmap (kid_sword_attack_05);
  destroy_bitmap (kid_sword_attack_06);
  destroy_bitmap (kid_sword_attack_counter_defense);

  destroy_bitmap (kid_sword_attack_defended);
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
  if (k->oaction != kid_sword_attack) {
    /* if (k->id == 0) */
    /*   printf ("kid_sword_attack: dx = %i, k->i = %i, k->f.b = %p\n", */
    /*           k->f.c.x - k->of.c.x, k->i, k->f.b); */

    k->i = -1;
    k->attack_defended = 0;
    k->counter_attacked = 0;
    k->counter_defense = 0;
    k->of = k->f;
  }

  if (k->oaction == kid_sword_defense) k->i = 0;

  if (k->i < 6 && k->counter_defense == 2) {
    k->i = 6;
    k->f = k->of;
  }

  if (k->i == 7) {
    k->attack_defended = 0;
    k->counter_defense = 0;
    k->counter_attacked = 0;
    k->hurt_enemy_in_counter_attack = false;
    kid_sword_defense (k);
    return false;
  } else if (k->i == 6
             && k->counter_defense != 2) {
    k->attack_defended = 0;
    k->counter_defense = 0;
    k->counter_attacked = 0;
    k->hurt_enemy_in_counter_attack = false;
    kid_sword_normal (k);
    return false;
  }

  if (k->i == 3 && k->attack_defended)
    k->f.c.x += (k->f.dir == LEFT) ? +9 : -9;

  if (k->i == 2 && k->attack_defended)
    k->i = 3;

  if (k->i == -1) k->j = 1;
  if (k->i == 1 || k->i == 2) k->j = 19;
  if (k->i == 3 || k->i == 5) k->j = 17;
  if (k->i == 4) k->j = 7;
  if (k->i == 6) k->j = 18;

  select_frame (k, kid_sword_attack_frameset, k->i + 1);
  select_xframe (&k->xf, sword_frameset, k->j);

  if (k->oaction == kid_sword_defense) k->fo.dx += -1;
  if (k->i == 1) k->xf.b = NULL;
  if (k->i == 3) k->xf.dx = -21, k->xf.dy = +7;
  if (k->i == 4) {
    if (k->attack_defended) k->fo.dx = -1;
    k->xf.dx = -7, k->xf.dy = +17;
  }

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct pos pbf, pmbo, pbb;

  /* collision */
  uncollide (&k->f, &k->fo, &k->fo, +8, false, &k->ci);

  /* fall */
  survey (_bf, pos, &k->f, NULL, &pbf, NULL);
  survey (_mbo, pos, &k->f, NULL, &pmbo, NULL);
  survey (_bb, pos, &k->f, NULL, &pbb, NULL);
  if (/* is_strictly_traversable (&pbf) */
      is_strictly_traversable (&pmbo)
      /* || is_strictly_traversable (&pbb) */
      ) {
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
  if (k->i == 3) play_sample (sword_attack_sample, NULL, k->id);

  /* depressible floors */
  if (k->i == 2) update_depressible_floor (k, -8, -40);
  else if (k->i == 3) update_depressible_floor (k, -11, -47);
  else if (k->i == 5) update_depressible_floor (k, -4, -33);
  else if (k->i == 6) update_depressible_floor (k, -1, -24);
  else keep_depressible_floor (k);
}

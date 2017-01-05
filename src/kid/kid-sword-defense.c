/*
  kid-sword-defense.c -- kid sword defense module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

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

struct frameset kid_sword_defense_frameset[KID_SWORD_DEFENSE_FRAMESET_NMEMB];

static void init_kid_sword_defense_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_sword_defense_00, *kid_sword_defense_01,
  *kid_sword_defense_02;

static void
init_kid_sword_defense_frameset (void)
{
  struct frameset frameset[KID_SWORD_DEFENSE_FRAMESET_NMEMB] =
    {{kid_sword_defense_00,+0,0},{kid_sword_defense_01,+0,0},
     {kid_sword_defense_02,+0,0}};

  memcpy (&kid_sword_defense_frameset, &frameset,
          KID_SWORD_DEFENSE_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_sword_defense (void)
{
  /* bitmaps */
  kid_sword_defense_00 = load_bitmap (KID_SWORD_DEFENSE_00);
  kid_sword_defense_01 = load_bitmap (KID_SWORD_DEFENSE_01);
  kid_sword_defense_02 = load_bitmap (KID_SWORD_DEFENSE_02);

  /* frameset */
  init_kid_sword_defense_frameset ();
}

void
unload_kid_sword_defense (void)
{
  destroy_bitmap (kid_sword_defense_00);
  destroy_bitmap (kid_sword_defense_01);
  destroy_bitmap (kid_sword_defense_02);
}

void
kid_sword_defense (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_sword_defense;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_sword_defense) {
    k->i = -1;

    struct anim *ke = get_anim_by_id (k->enemy_id);
    if (ke && k->i_counter_defended)
      ke->enemy_defended_my_attack = 1;

    k->enemy_defended_my_attack = 0;
    k->i_counter_defended = 0;
    k->enemy_counter_attacked_myself = 0;
    k->hurt_enemy_in_counter_attack = false;
  }

  struct anim *ke = get_anim_by_id (k->enemy_id);
  if (k->i == 2) {
    kid_sword_attack (k);
    return false;
  } else if (k->i == 1 && ke && ke->enemy_defended_my_attack == 2
           && ke->enemy_counter_attacked_myself != 2) {
    kid_sword_walkb (k);
    return false;
  } else if (k->i == 1
             && ! (ke && ke->enemy_defended_my_attack == 2)) {
    kid_sword_normal (k);
    return false;
  }

  if (k->oaction == kid_sword_attack) {
    select_frame (k, kid_sword_walkb_frameset, 0);
    k->j = 10;
  } else if (k->f.b == kid_sword_walkb_frameset[0].frame) {
    select_frame (k, kid_sword_defense_frameset, 1);
    k->fo.dx += 7;
    k->j = 14;
  } else {
    select_frame (k, kid_sword_defense_frameset, k->i + 1);

    if (k->i == 0) k->j = 28;
    if (k->i == 1) k->j = 14;
    if (k->i == 2) k->j = 15;
  }

  select_xframe (&k->xf, sword_frameset, k->j);

  if (k->oaction == kid_sword_attack) k->fo.dx += +2;

  /* if (k->id == 0) */
  /*   printf ("kid_sword_defense: k->i = %i, k->fo.dx = %i\n", */
  /*           k->i, k->fo.dx); */

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct pos pmbo;

  /* collision */
  uncollide_back_fight (k);

  /* fall */
  survey (_mbo, pos, &k->f, NULL, &pmbo, NULL);
  if (is_strictly_traversable (&pmbo)) {
    k->xf.b = NULL;
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  /* depressible floors */
  keep_depressible_floor (k);
}

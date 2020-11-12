/*
  kid-sword-defense.c -- kid sword defense module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

static bool flow (struct actor *k);
static bool physics_in (struct actor *k);
static void physics_out (struct actor *k);

void
kid_sword_defense (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_sword_defense;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_sword_defense) {
    k->i = -1;

    struct actor *ke = get_actor_by_id (k->enemy_id);
    if (ke && k->i_counter_defended)
      ke->enemy_defended_my_attack = 1;

    k->enemy_defended_my_attack = 0;
    k->i_counter_defended = 0;
    k->enemy_counter_attacked_myself = 0;
    k->hurt_enemy_in_counter_attack = false;
  }

  struct actor *ke = get_actor_by_id (k->enemy_id);
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
    select_actor_frame (k, "KID", "SWORD_WALKB", 0);
    k->j = 10;
  } else if (k->j == 10) {
    select_actor_frame (k, "KID", "SWORD_DEFENSE", 1);
    k->j = 14;
  } else {
    select_actor_frame (k, "KID", "SWORD_DEFENSE", k->i + 1);

    if (k->i == 0) k->j = 28;
    if (k->i == 1) k->j = 14;
    if (k->i == 2) k->j = 15;
  }

  select_actor_xframe (k, "KID", "SWORD", k->j);

  /* if (k->id == 0) */
  /*   printf ("kid_sword_defense: k->i = %i, k->fo.dx = %i\n", */
  /*           k->i, k->fo.dx); */

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* place on the ground */
  place_on_the_ground (&k->f, &k->f.c);

  /* collision */
  uncollide_back_fight (k);

  /* fall */
  if (is_falling (&k->f, _mbo, +0, +0)) {
    k->xf.b = NULL;
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  /* depressible floors */
  keep_depressible_floor (k);
}

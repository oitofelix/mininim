/*
  kid-sword-attack.c -- kid sword attack module;

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
kid_sword_attack (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_sword_attack;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_sword_attack) {
    /* if (k->id == 0) */
    /*   printf ("kid_sword_attack: dx = %i, k->i = %i, k->f.b = %p\n", */
    /*           k->f.c.x - k->of.c.x, k->i, k->f.b); */

    k->i = -1;
    k->of = k->f;
    if (k->oaction == kid_sword_normal)
      k->i_initiated_attack = true;
  }

  if (k->oaction == kid_sword_defense) k->i = 0;

  if (k->i < 6 && k->i_counter_defended == 2) {
    k->i = 6;
    k->f = k->of;
  }

  if (k->i == 7) {
    kid_sword_defense (k);
    return false;
  } else if (k->i == 6
             && k->i_counter_defended != 2) {
    kid_sword_normal (k);
    return false;
  }

  /* TODO: query video mode for these offsets */
  if (k->i == 3 && k->placed_at_attack_frame)
    move_frame (&k->f, _tb, +0, +8, +8);

  select_actor_frame (k, "KID", "SWORD_ATTACK", k->i + 1);

  if (k->i == 0) k->j = 1;
  if (k->i == 1) k->j = 16;
  if (k->i == 2 || k->i == 3) k->j = 19;
  if (k->i == 4 || k->i == 6) k->j = 17;
  if (k->i == 5) k->j = 7;
  if (k->i == 7) k->j = 18;

  select_actor_xframe (k, "KID", "SWORD", k->j);

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* collision */
  uncollide_front_fight (k);

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
  /* place on the ground */
  place_on_the_ground (&k->f, &k->f.c);

  /* sound */
  if (k->i == 3
      && ! search_audio_instance (&guard_hit_audio, 0, NULL, k->enemy_id))
    play_audio (&sword_attack_audio, NULL, k->id);

  /* depressible floors */
  if (k->i == 2) update_depressible_floor (k, -8, -40);
  else if (k->i == 3) update_depressible_floor (k, -11, -47);
  else if (k->i == 5) update_depressible_floor (k, -4, -33);
  else if (k->i == 6) update_depressible_floor (k, -1, -24);
  else keep_depressible_floor (k);
}

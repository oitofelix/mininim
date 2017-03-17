/*
  kid-take-sword.c -- kid take sword module;

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

static bool flow (struct actor *k);
static bool physics_in (struct actor *k);
static void physics_out (struct actor *k);

void
kid_take_sword (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_take_sword;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_take_sword) {
    k->auto_taken_sword = false;
    k->i = -1;
    if (! k->has_sword) {
      kid_normal (k);
      return false;
    }
  }

  if (k->i == 3) {
    kid_sword_normal (k);
    return false;
  }

  select_actor_frame (k, "KID", "TAKE_SWORD", k->i + 1);

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* collision */
  uncollide_back_fight (k);

  /* fall */
  if (is_falling (&k->f, _mbo, +0, +0)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  /* sound */
  if (k->i == 0) play_audio (&take_sword_audio, NULL, k->id);

  /* depressible floors */
  if (k->i == 3) update_depressible_floor (k, -4, -23);
  else keep_depressible_floor (k);
}

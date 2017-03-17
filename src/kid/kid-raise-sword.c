/*
  kid-raise-sword.c -- kid raise sword module;

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
kid_raise_sword (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_raise_sword;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_raise_sword) k->i = -1, k->wait = 5;

  if (k->i == 0 && k->wait > 0) k->wait--;
  else if (k->i < 3) k->i++;
  else {
    kid_keep_sword (k);
    invalid_pos (&k->item_pos);
    k->has_sword = true;
    return false;
  }

  k->j = 20 + k->i;

  select_actor_frame (k, "KID", "RAISE_SWORD", k->i);
  select_actor_xframe (k, "KID", "SWORD", k->j);

  if (k->i == 0 && k->wait < 5) k->fo.dx = 0;

  return true;
}

static bool
physics_in (struct actor *k)
{
  return true;
}

static void
physics_out (struct actor *k)
{
  /* depressible floors */
  keep_depressible_floor (k);

  /* sound */
  if (k->i == 0 && k->wait == 5) {
    if (k->id == current_kid_id) {
      mr.flicker = 8;
      mr.color = get_flicker_raise_sword_color ();
    }
    play_audio (&glory_audio, NULL, k->id);
    kid_haptic (k, KID_HAPTIC_RAISE_SWORD);
  }

  /* consume sword */
  if (k->i == 0)
    register_tile_undo (&undo, &k->item_pos,
                       MIGNORE, MIGNORE, NO_ITEM, MIGNORE,
                       NULL, false, "CONSUME SWORD");
}

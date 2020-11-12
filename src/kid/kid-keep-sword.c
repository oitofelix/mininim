/*
  kid-keep-sword.c -- kid keep sword module;

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
kid_keep_sword (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_keep_sword;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_keep_sword) k->i = -1, k->wait = 1;

  if (k->i < 8) k->i++;
  else if (k->i == 8 && k->wait > 0) k->wait--;
  else if (k->i == 8 && k->wait == 0) k->i++, k->wait = 2;
  else if (k->i == 9 && k->wait > 0) k->wait--;
  else {
    if (k->keep_sword_fast) {
      kid_normal (k);
      k->keep_sword_fast = false;
      return false;
    } else {
      /* kid turn will invert kid's direction */
      k->f.dir = (k->f.dir == RIGHT) ? LEFT : RIGHT;
      kid_turn (k);
      return false;
    }
  }

  if (k->keep_sword_fast && k->i % 2 == 0) k->i++, k->wait = 0;

  k->j = 24 + k->i;

  select_actor_frame (k, "KID", "KEEP_SWORD", k->i);
  if (k->i < 4) select_actor_xframe (k, "KID", "SWORD", k->j);

  if (k->i == 8 && k->wait < 1) k->fo.dx = 0;
  if (k->i == 9 && k->wait < 2) k->fo.dx = 0;
  if (k->keep_sword_fast && k->i % 2)
    k->fo.dx += actor_dx (k, "KID", "KEEP_SWORD", k->i - 1);

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* collision */
  uncollide (&k->f, &k->fo, _bb, +5, +0, &k->fo, NULL);

  return true;
}

static void
physics_out (struct actor *k)
{
  /* place on the ground */
  place_on_the_ground (&k->f, &k->f.c);

  /* depressible floors */
  keep_depressible_floor (k);
}

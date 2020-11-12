/*
  kid-stairs.c -- kid stairs module;

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
kid_stairs (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_stairs;
  k->f.dir = RIGHT;
  k->f.flip = 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame_inv = true;
  next_frame (&k->f, &k->f, &k->fo);
  next_frame_inv = false;
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_stairs) {
    struct rect r; actor_rect (&r, k, "KID", "STAIRS", "STAIRS");
    place_actor (k, &k->p, r.c.x, r.c.y, "KID", "STAIRS", 0);
    k->i = k->j = -1;
  }

  if (k->i >= 9) if (k->f.c.l->end) k->f.c.l->end (&k->p);

  if (k->i == 11 && k->j < 17) {
    k->invisible = true;
    k->j++;
  } else if (k->i < 11) k->j = k->i + 1;

  int i = k->i <= 10 ? k->i + 1: 11;

  select_actor_frame (k, "KID", "STAIRS", i);

  if (k->j > 11) k->fo.dx = k->fo.dy = 0;

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
  if (k->i > 4) clear_depressible_floor (k);
  else keep_depressible_floor (k);

  /* sound and haptic */
  if (k->j % 4 == 0) {
    play_audio (&step_audio, NULL, k->id);
    kid_haptic (k, KID_HAPTIC_STEP);
  }
}

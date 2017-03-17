/*
  kid-misstep.c -- kid misstep module;

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
kid_misstep (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_misstep;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_misstep) k->i = -1, k->misstep = true;

  if (k->i == 10) {
    int dx = (k->f.dir == LEFT) ? +11 : PLACE_WIDTH + 7;
    int dy = +15;
    place_actor (k, &k->p, dx, dy, "KID", "NORMAL", 0);
    kid_normal (k);
    return false;
  }

  select_actor_frame (k, "KID", "MISSTEP", k->i + 1);

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* fall */
  if (is_falling (&k->f, _mba, +0, +0)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  struct pos pmbo;

  /* depressible floors */
  keep_depressible_floor (k);

  /* loose floor shaking */
  survey (_mbo, pos, &k->f, NULL, &pmbo, NULL);
  if (k->i == 8) shake_loose_floor_row (&pmbo);

  /* audio and haptic */
  if (k->i == 7) {
    play_audio (&step_audio, NULL, k->id);
    kid_haptic (k, KID_HAPTIC_STEP);
  }
}

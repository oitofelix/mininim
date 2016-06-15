/*
  kid-unclimb.c -- kid unclimb module;

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

static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

void
load_kid_unclimb (void)
{
  return;
}

void
unload_kid_unclimb (void)
{
  return;
}

void
kid_unclimb (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_unclimb;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  struct pos hanged_pos;

  if (k->oaction != kid_unclimb) k->i = 14;
  if (k->oaction == kid_climb) k->i = 3;

  if (k->i == 0) {
    kid_hang (k);
    return false;
  }

  if (k->i == 14) {
    get_hanged_pos (&k->hang_pos, k->f.dir, &hanged_pos);
    place_frame (&k->f, &k->f, kid_climb_frameset[13].frame,
                 &hanged_pos, 18, 25);
  }

  k->i--;

  k->fo.b = kid_climb_frameset[k->i].frame;
  k->fo.dx = -kid_climb_frameset[k->i + 1].dx;
  k->fo.dy = -kid_climb_frameset[k->i + 1].dy;

  if (k->oaction == kid_climb) k->fo.dx += +2;
  if (k->i == 1) k->fo.dx += -3;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct pos ptf;

  /* fall */
  survey (_tf, pos, &k->f, NULL, &ptf, NULL);
  if (is_strictly_traversable (&ptf)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  struct pos hanged_pos;

  /* depressible floors */
  clear_depressible_floor (k);
  get_hanged_pos (&k->hang_pos, k->f.dir, &hanged_pos);
  press_depressible_floor (&hanged_pos);
}

/*
  kid-hang-free.c -- kid hang free module;

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
load_kid_hang_free (void)
{
  /* for symmetry */
}

void
unload_kid_hang_free (void)
{
  /* for symmetry */
}

void
kid_hang_free (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_hang_free;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  struct pos np;

  if (k->oaction != kid_hang_free)
    k->i = 5, k->j = -1, k->wait = 3, k->reverse = true;

  bool hang_back = ((k->f.dir == LEFT) ? k->key.right : k->key.left)
    && ! k->key.up && k->key.shift;

  int back_dir = (k->f.dir == LEFT) ? RIGHT : LEFT;

    /* hang back */
  if (k->i >= 7
      && hang_back && is_hangable_pos (&k->hang_pos, back_dir)) {
    play_sample (hang_on_fall_sample, k->f.c.room);
    kid_turn (k);
    return false;
  }

  /* climb */
  if ((k->i < 5 || k->j > -1
       || k->hang_caller != kid_unclimb)
      && k->key.up
      && ! ((k->key.left || k->key.right)
            && k->hang_caller == kid_unclimb)
      && ! k->hang_limit) {
    kid_climb (k);
    return false;
  }

  /* release */
  if ((! k->key.shift || k->hang_limit
       || get_hanged_con (&k->hang_pos, k->f.dir) == NO_FLOOR)
      && (k->i < 5 || k->j > -1)) {
    int dir = (k->f.dir == LEFT) ? -1 : +1;
    k->hang_limit = false;
    if (! is_strictly_traversable (&k->hang_pos)
        && k->i >= 4) {
      place_frame (&k->f, &k->f, kid_vjump_frameset[13].frame,
                   &k->hang_pos, (k->f.dir == LEFT) ? +7 : PLACE_WIDTH + 9, -8);
      kid_vjump (k);
      return false;
    }
    if (! is_strictly_traversable (prel (&k->hang_pos, &np, +0, dir))
        && k->i <= 4) {
      place_frame (&k->f, &k->f, kid_vjump_frameset[13].frame,
                   &k->hang_pos, (k->f.dir == LEFT) ? +7 : PLACE_WIDTH + 5, -8);
      kid_vjump (k);
      return false;
    }
    if (is_strictly_traversable (&k->hang_pos)
        && k->i >= 4) {
      place_frame (&k->f, &k->f, kid_fall_frameset[0].frame,
                   &k->hang_pos, (k->f.dir == LEFT) ? +16 : PLACE_WIDTH - 16, +12);
      kid_fall (k);
      return false;
    }
    if (is_strictly_traversable (prel (&k->hang_pos, &np, +0, dir))
        && k->i <= 4) {
      place_frame (&k->f, &k->f, kid_fall_frameset[0].frame,
                   &k->hang_pos, (k->f.dir == LEFT)
                   ? -10 : PLACE_WIDTH + 10, +12);
      kid_fall (k);
      return false;
    }
  }

  if (k->reverse && k->i > 0) {
    if (k->i == 4  && k->j++ > 0) k->hang_limit = true;
    k->i--;
  } else if (k->reverse && k->i == 0) {
    if (k->wait == 0) {
      k->reverse = false; k->i++;
    } else k->wait--;
  } else if (! k->reverse
             && ((k->j == 0 && k->i < 12)
                 || (k->j > 0 && k->i < 9))) k->i++;
  else if (! k->reverse
           && ((k->j == 0 && k->i == 12)
               || (k->j > 0 && k->i == 9))) {
    k->reverse = true; k->i--;
  }

  k->fo.b = kid_hang_frameset[k->i].frame;
  k->fo.dx = (k->reverse) ? -kid_hang_frameset[k->i + 1].dx
    : kid_hang_frameset[k->i].dx;
  k->fo.dy = (k->reverse) ? -kid_hang_frameset[k->i + 1].dy
    : kid_hang_frameset[k->i].dy;

  if (k->f.b == kid_hang_13) k->fo.dx = +0, k->fo.dy = +1;

  if (k->reverse && k->j == 0 && k->i == 0
      && k->wait < 3) k->fo.dy = 0;

  return true;
}

static bool
physics_in (struct anim *k)
{
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

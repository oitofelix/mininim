/*
  kid-jump.c -- kid jump module;

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
kid_jump (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_jump;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  struct pos pm, pmf, ptf;

  if (k->oaction != kid_jump)
    k->i = -1, k->misstep = k->hang = false;

  bool hang_front = ((k->f.dir == LEFT) ? k->key.left : k->key.right)
    && ! k->key.up && k->key.shift;

  bool hang_back = ((k->f.dir == LEFT) ? k->key.right : k->key.left)
    && ! k->key.up && k->key.shift;

  int back_dir = (k->f.dir == LEFT) ? RIGHT : LEFT;

  survey (_m, pos, &k->f, NULL, &pm, NULL);
  surveyo (_m, +4, +0, pos, &k->f, NULL, &pmf, NULL);
  survey (_tf, pos, &k->f, NULL, &ptf, NULL);

  /* hang front */
  if (movements == NATIVE_MOVEMENTS
      && k->i >= 7 && k->i <= 10 && hang_front
      && is_hangable_pos (&pmf, k->f.dir)
      && is_immediately_accessible_pos (&pmf, &pm, &k->f)) {
    if (is_hangable_pos (&pm, k->f.dir)) k->hang_pos = pm;
    else if (is_hangable_pos (&ptf, k->f.dir)) k->hang_pos = ptf;
    pos2room (&k->hang_pos, k->f.c.room, &k->hang_pos);
    k->hang = true;
    play_audio (&hang_on_fall_audio, NULL, k->id);
    kid_haptic (k, KID_HAPTIC_HANG);
    kid_hang (k);
    return false;
  }

  /* hang back */
  if (movements == NATIVE_MOVEMENTS
      && k->i >= 7 && k->i <= 10
      && hang_back && is_hangable_pos (&ptf, back_dir)
      && is_immediately_accessible_pos (&ptf, &pm, &k->f)) {
    k->hang_pos = ptf;
    pos2room (&k->hang_pos, k->f.c.room, &k->hang_pos);
    k->hang = true;
    play_audio (&hang_on_fall_audio, NULL, k->id);
    kid_haptic (k, KID_HAPTIC_HANG);
    kid_turn (k);
    return false;
  }

  if (k->i == 17) {
    kid_normal (k);
    return false;
  }

  select_actor_frame (k, "KID", "JUMP", k->i + 1);

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* inertia */
  if (k->i >= 8 && k->i <= 10) k->inertia = 5;
  else if (k->i == 11) k->inertia = 3;
  else k->inertia = 0;

  /* collision */
  int dx = k->i < 7 || k->i > 10
    ? +0
    : k->f.dir == LEFT ? -8 : -4;
  if (uncollide (&k->f, &k->fo, _bf, dx, dx, NULL, &k->ci)) {
    if (k->i < 7 || k->i > 10) kid_stabilize_collision (k);
    else kid_crouch_collision (k);
    return false;
  }

  /* fall */
  if ((is_falling (&k->f, _bb, +0, +0)
       && is_falling (&k->f, _mbo, +0, +0) && k->i <= 6)
      || (is_falling (&k->f, _bf, +0, +0)
          && is_falling (&k->f, _mbo, +0, +0)
          && k->i >= 10)) {
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
  if (k->i == 8) clear_depressible_floor (k);
  else if (k->i == 11) update_depressible_floor (k, -11, -15);
  else if (k->i == 14) update_depressible_floor (k, 0, -6);
  else keep_depressible_floor (k);

  /* loose floor shaking */
  survey (_mbo, pos, &k->f, NULL, &pmbo, NULL);
  if (k->i == 12) shake_loose_floor_row (&pmbo);

  /* sound and haptic */
  if (k->i == 11 || k->i == 14) {
    play_audio (&step_audio, NULL, k->id);
    kid_haptic (k, KID_HAPTIC_STEP);
  }
}

bool
is_kid_jump_start (struct actor *k)
{
  return k->action == kid_jump && k->i <= 5;
}

bool
is_kid_jump_air (struct actor *k)
{
  return k->action == kid_jump && k->i >= 6 && k->i <= 10;
}

bool
is_kid_jump_landing (struct actor *k)
{
  return k->action == kid_jump && k->i >=11;
}

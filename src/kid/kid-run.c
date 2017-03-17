/*
  kid-run.c -- kid run module;

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
kid_run (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_run;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! cutscene && ! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_run) {
    k->i = -1;
    k->edge_detection = false;
  }

  bool stop = ! ((k->f.dir == RIGHT) ? k->key.right : k->key.left);
  bool crouch = k->key.down;
  bool jump = ((k->f.dir == RIGHT) && k->key.right && k->key.up)
    || ((k->f.dir == LEFT) && k->key.left && k->key.up);

  if (crouch) {
    kid_crouch (k);
    return false;
  }

  /* constrained turn run */
  if (jump && k->oaction == kid_turn_run
      && k->constrained_turn_run)
    move_frame (&k->f, _tb, +0, -4, -4);

  /* constrained run jump */
  if (jump && k->oaction == kid_run_jump) {
    struct pos ptf; surveyo (_tf, +0, +0, pos, &k->f, NULL, &ptf, NULL);
    if (is_constrained_pos (&ptf, &k->f))
      move_frame (&k->f, _tb, +0, -4, -4);
  }

  if (jump && k->oaction != kid_run_jump) {
    /* platform edge detection */
    struct coord mf, cm; struct pos pmf;
    survey (_mf, pos, &k->f, &mf, &pmf, NULL);
    int dir = (k->f.dir == LEFT) ? -1 : +1;
    struct pos p;
    prel (&pmf, &p, +0, dir * 2);
    tile_coord (&p, _m, &cm);
    if (! (traversable_cs (fg_rel (&pmf, +0, dir * 2))
           && ! traversable_cs (fg_rel (&pmf, +0, dir))
           && dist_coord (&mf, &cm) > 2 * PLACE_WIDTH - 3)) {
      kid_run_jump (k);
      return false;
    } else k->edge_detection = true;
  }

  if (stop && k->oaction != kid_run_jump) {
    kid_stop_run (k);
    return false;
  }

  if (k->i == 7) k->i = -1;

  if (k->oaction == kid_turn_run) k->i = 5;

  select_actor_frame (k, "KID", "RUN", k->i + 1);

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* inertia */
  k->inertia = 0;
  k->cinertia = 6;

  /* collision */
  if (uncollide (&k->f, &k->fo, _bf, -4, -4, NULL, &k->ci)) {
    kid_stabilize_collision (k);
    return false;
  }

  /* fall */
  if (is_falling (&k->f, _tf, -4, -4)) {
    play_audio (&step_audio, NULL, k->id);
    kid_haptic (k, KID_HAPTIC_STEP);
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  /* depressible floors */
  if (k->i == 2) update_depressible_floor (k, -7, -13);
  else if (k->i == 5) update_depressible_floor (k, -5, -30);
  else if (k->i == 6) update_depressible_floor (k, -4, -11);
  else keep_depressible_floor (k);

  /* sound and haptic */
  if (k->oaction == kid_run_jump) {
    play_audio (&step_audio, NULL, k->id);
    kid_haptic (k, KID_HAPTIC_STEP);
  }

  if (k->i == 2 || k->i == 6) {
    play_audio (&step_audio, NULL, k->id);
    kid_haptic (k, KID_HAPTIC_STEP);
  }
}

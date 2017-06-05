/*
  kid-vjump.c -- kid vjump module;

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
kid_vjump (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_vjump;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_vjump)
    k->i = -1, k->j = 0,
      k->just_hanged = k->hit_ceiling =
      k->hit_ceiling_fake = k->hang = false;

  if (k->i == 12 && k->hang) {
    kid_hang (k);
    return false;
  }

  if (k->i == 17) {
    kid_normal (k);
    return false;
  }

  if (k->oaction == kid_hang_non_free
      || k->oaction == kid_hang_free) {
    k->just_hanged = true;
    k->i = 13;
  } else if (k->i == 12 && k->j > 0
             && ! k->hit_ceiling) k->i = 12;
  else k->i++;

  select_actor_frame (k, "KID", "VJUMP", k->i);

  if (k->i == 12 && k->j++ > 0)
    k->fo.dx = 0, k->fo.dy += 2 * k->j + 1;
  if (k->j == 4) k->j = 0;

  return true;
}

static bool
physics_in (struct actor *k)
{
  struct pos ptf, ptfb, ptr;

  /* collision */
  if (uncollide (&k->f, &k->fo, _bf, +0, +0, NULL, &k->ci)
      && fg (&k->ci.tile_p) == MIRROR)
    uncollide (&k->f, &k->fo, _bf, +0, +0, &k->fo, &k->ci);

  /* fall */
  if (is_falling (&k->f, _m, +0, +0)
      && is_falling (&k->f, _mf, +0, +0)
      && is_falling (&k->f, _mba, +0, +0)) {
    kid_fall (k);
    return false;
  }

  /* ceiling hit */
  surveyo (_tr, -4, +0, pos, &k->f, NULL, &ptr, NULL);
  struct pos ptra; prel (&ptr, &ptra, -1, 0);
  if (k->i == 12 && k->j == 1) {
    k->hit_ceiling_fake = ! is_traversable_fake (&ptra);
    k->hit_ceiling = ! is_traversable (&ptra) && ! k->hang;
  }

  /* hang */
  int dir = (k->f.dir == LEFT) ? +1 : -1;
  surveyo (_tf, -4, +0, pos, &k->f, NULL, &ptf, NULL);
  prel (&ptf, &ptfb, 0, dir);
  if (k->i == 0
      && is_hangable (&ptfb, k->f.dir)
      && is_immediately_accessible_pos (&ptfb, &ptf, &k->f)) {
    prel (&ptf, &k->hang_pos, 0, dir);
    pos2room (&k->hang_pos, k->f.c.room, &k->hang_pos);
    k->hang = true;
    struct rect r; actor_rect (&r, k, "KID", "VJUMP", "HANG_BACK");
    place_actor (k, &k->hang_pos, r.c.x, MIGNORE, "KID", "VJUMP", 0);
  } else if (k->i == 0
             && is_hangable (&ptf, k->f.dir)) {
    k->hang_pos = ptf;
    pos2room (&k->hang_pos, k->f.c.room, &k->hang_pos);
    k->hang = true;
    struct rect r; actor_rect (&r, k, "KID", "VJUMP", "HANG_FRONT");
    place_actor (k, &k->hang_pos, r.c.x, MIGNORE, "KID", "VJUMP", 0);
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  struct pos ptf, ptb, pmbo;
  enum tile_fg ctf, ctb;

  /* place on the ground */
  if (k->i <= 10 || k->i >= 14) place_on_the_ground (&k->f, &k->f.c);

 /* depressible floors */
  if (k->i == 0 && k->hang
      && ! peq (&k->hang_pos, &k->df_pos[0])
      && ! peq (&k->hang_pos, &k->df_pos[1]))
    update_depressible_floor (k, -1, -8);
  else if (k->i == 11) {
    save_depressible_floor (k);
    clear_depressible_floor (k);
  } else if (k->i == 14 && ! k->just_hanged) {
    restore_depressible_floor (k);
    keep_depressible_floor (k);
  } else if (k->i == 14 && k->just_hanged)
    update_depressible_floor (k, -5, -7);
  else keep_depressible_floor (k);

  /* ceiling loose floor shaking and release */
  if (k->i == 12 && k->hit_ceiling) {
    survey (_tb, pos, &k->f, NULL, &ptb, NULL);
    ctb = fg (&ptb);
    survey (_tf, pos, &k->f, NULL, &ptf, NULL);
    ctf = fg (&ptf);
    shake_loose_floor_row (&ptb);
    if (ctb == LOOSE_FLOOR) release_loose_floor (&ptb, k);
    if (ctf == LOOSE_FLOOR) release_loose_floor (&ptf, k);
    kid_haptic (k, KID_HAPTIC_COLLISION);
  }

  /* loose floor shaking */
  survey (_mbo, pos, &k->f, NULL, &pmbo, NULL);
  if (k->i == 16) shake_loose_floor_row (&pmbo);
}

bool
is_kid_vjump_touching_above (struct actor *k)
{
  return k->action == kid_vjump && k->i >= 12 && k->i <= 13;
}

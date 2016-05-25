/*
  kid-vjump.c -- kid vjump module;

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

struct frameset kid_vjump_frameset[KID_VJUMP_FRAMESET_NMEMB];

static void init_kid_vjump_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_vjump_00, *kid_vjump_01, *kid_vjump_02, *kid_vjump_03,
  *kid_vjump_04, *kid_vjump_05, *kid_vjump_06, *kid_vjump_07, *kid_vjump_08,
  *kid_vjump_09, *kid_vjump_10, *kid_vjump_11, *kid_vjump_12, *kid_vjump_13,
  *kid_vjump_14, *kid_vjump_15, *kid_vjump_16, *kid_vjump_17;

static void
init_kid_vjump_frameset (void)
{
  struct frameset frameset[KID_VJUMP_FRAMESET_NMEMB] =
    {{kid_vjump_00,+3,+0},{kid_vjump_01,-1,+0},{kid_vjump_02,-1,+0},
     {kid_vjump_03,+2,+0},{kid_vjump_04,+0,+0},{kid_vjump_05,-1,+0},
     {kid_vjump_06,-1,+0},{kid_vjump_07,-3,+0},{kid_vjump_08,-1,+0},
     {kid_vjump_09,-6,+0},{kid_vjump_10,+0,+0},{kid_vjump_11,+2,-3},
     {kid_vjump_12,+3,-7},{kid_vjump_13,+0,+8},{kid_vjump_14,+3,+2},
     {kid_vjump_15,-1,+0},{kid_vjump_16,+0,+0},{kid_vjump_17,+0,0}};

  memcpy (&kid_vjump_frameset, &frameset,
          KID_VJUMP_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_vjump (void)
{
  /* bitmaps */
  kid_vjump_00 = load_bitmap (KID_VJUMP_00);
  kid_vjump_01 = load_bitmap (KID_VJUMP_01);
  kid_vjump_02 = load_bitmap (KID_VJUMP_02);
  kid_vjump_03 = load_bitmap (KID_VJUMP_03);
  kid_vjump_04 = load_bitmap (KID_VJUMP_04);
  kid_vjump_05 = load_bitmap (KID_VJUMP_05);
  kid_vjump_06 = load_bitmap (KID_VJUMP_06);
  kid_vjump_07 = load_bitmap (KID_VJUMP_07);
  kid_vjump_08 = load_bitmap (KID_VJUMP_08);
  kid_vjump_09 = load_bitmap (KID_VJUMP_09);
  kid_vjump_10 = load_bitmap (KID_VJUMP_10);
  kid_vjump_11 = load_bitmap (KID_VJUMP_11);
  kid_vjump_12 = load_bitmap (KID_VJUMP_12);
  kid_vjump_13 = load_bitmap (KID_VJUMP_13);
  kid_vjump_14 = load_bitmap (KID_VJUMP_14);
  kid_vjump_15 = load_bitmap (KID_VJUMP_15);
  kid_vjump_16 = load_bitmap (KID_VJUMP_16);
  kid_vjump_17 = load_bitmap (KID_VJUMP_17);

  /* frameset */
  init_kid_vjump_frameset ();
}

void
unload_kid_vjump (void)
{
  destroy_bitmap (kid_vjump_00);
  destroy_bitmap (kid_vjump_01);
  destroy_bitmap (kid_vjump_02);
  destroy_bitmap (kid_vjump_03);
  destroy_bitmap (kid_vjump_04);
  destroy_bitmap (kid_vjump_05);
  destroy_bitmap (kid_vjump_06);
  destroy_bitmap (kid_vjump_07);
  destroy_bitmap (kid_vjump_08);
  destroy_bitmap (kid_vjump_09);
  destroy_bitmap (kid_vjump_10);
  destroy_bitmap (kid_vjump_11);
  destroy_bitmap (kid_vjump_12);
  destroy_bitmap (kid_vjump_13);
  destroy_bitmap (kid_vjump_14);
  destroy_bitmap (kid_vjump_15);
  destroy_bitmap (kid_vjump_16);
  destroy_bitmap (kid_vjump_17);
}

void
kid_vjump (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_vjump;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_vjump)
    k->i = -1, k->j = 0,
      k->just_hanged = k->hit_ceiling =
      k->hang = k->misstep = false;

  if (k->i == 12 && k->hang) {
    kid_hang (k);
    return false;
  }

  if (k->i == 17) {
    kid_normal (k);
    return false;
  }

  if (k->oaction == kid_hang_wall
      || k->oaction == kid_hang_free) {
    k->just_hanged = true;
    k->i = 13;
  } else if (k->i == 12 && k->j > 0
             && ! k->hit_ceiling) k->i = 12;
  else k->i++;

  select_frame (k, kid_vjump_frameset, k->i);

  if (k->oaction == kid_hang_free
      && is_hang_pos_critical (&k->hang_pos))
    k->fo.dx -= (k->f.dir == LEFT) ? 9 : 13;
  if (k->hang && is_hang_pos_critical (&k->hang_pos)
      && k->i == 11) k->fo.dx = +7;
  if (k->hang && ! is_hang_pos_critical (&k->hang_pos)
      && (k->i == 11 || k->i == 12)) {
    k->fo.dx += -1;
    k->fo.dy += -1;
  }
  if (k->i == 12 && k->j++ > 0)
    k->fo.dx = 0, k->fo.dy += 2 * k->j + 1;
  if (k->j == 4) k->j = 0;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct coord nc, tf; struct pos np, ptf, ptr, pmt, pm, pmf, pmba;

  survey (_m, pos, &k->f, &nc, &pm, &np);

  /* collision */
  if (is_colliding (&k->f, &k->fo, +0, false, &k->ci)
      && pm.floor == k->ci.p.floor
      && (k->ci.t == MIRROR
          || ((k->ci.t == CARPET || k->ci.t == DOOR)
              && k->f.dir == RIGHT)))
    uncollide (&k->f, &k->fo, &k->fo, +0, false, &k->ci);

  /* fall */
  survey (_mf, pos, &k->f, &nc, &pmf, &np);
  survey (_mba, pos, &k->f, &nc, &pmba, &np);
  if (is_strictly_traversable (&pm)
      && is_strictly_traversable (&pmf)
      && is_strictly_traversable (&pmba)) {
    kid_fall (k);
    return false;
  }

  /* ceiling hit */
  survey (_tr, pos, &k->f, &nc, &ptr, &np);
  survey (_mt, pos, &k->f, &nc, &pmt, &np);
  struct pos ptra; prel (&ptr, &ptra, -1, 0);
  struct pos pmta; prel (&pmt, &pmta, -1, 0);
  if (k->i == 12 && k->j == 1
      && (! is_strictly_traversable (&ptra)
          && ! is_strictly_traversable (&pmta)))
    k->hit_ceiling = true;

  /* hang */
  int dir = (k->f.dir == LEFT) ? +1 : -1;
  survey (_tf, pos, &k->f, &tf, &ptf, &np);
  if (k->i == 0
      && is_hangable_pos (prel (&ptf, &np, 0, dir), k->f.dir)
      && ! (con (&ptf)->fg == DOOR
            && k->f.dir == LEFT
            && tf.y <= door_grid_tip_y (&ptf) - 10)) {
    prel (&ptf, &k->hang_pos, 0, dir);
    pos2room (&k->hang_pos, k->f.c.room, &k->hang_pos);
    k->fo.dx += is_hang_pos_critical (&k->hang_pos) ? -12 : -3;
    k->hang = true;
  } else if (k->i == 0 && can_hang (&k->f, false, &k->hang_pos)
             && ! is_hang_pos_critical (&k->hang_pos)
             && (k->f.dir == LEFT || con (&k->hang_pos)->fg != DOOR)) {
    k->fo.dx -= 0; k->hang = true;
  }

  if (k->i == 0 && k->hang)
    place_frame (&k->f, &k->f, kid_vjump_frameset[0].frame,
                 &k->hang_pos, (k->f.dir == LEFT) ? +14 : PLACE_WIDTH + 5, +16);

  return true;
}

static void
physics_out (struct anim *k)
{
  struct coord nc; struct pos np, ptf, ptb, pmbo;
  enum confg ctf, ctb;

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
  if (k->i == 13 && k->hit_ceiling) {
    ctb = survey (_tb, pos, &k->f, &nc, &ptb, &np)->fg;
    ctf = survey (_tf, pos, &k->f, &nc, &ptf, &np)->fg;
    shake_loose_floor_row (&ptb);
    if (ctb == LOOSE_FLOOR) release_loose_floor (&ptb);
    if (ctf == LOOSE_FLOOR) release_loose_floor (&ptf);
  }

  /* loose floor shaking */
  survey (_mbo, pos, &k->f, &nc, &pmbo, &np);
  if (k->i == 17) shake_loose_floor_row (&pmbo);
}

bool
is_kid_vjump (struct frame *f)
{
  int i;
  for (i = 0; i < KID_VJUMP_FRAMESET_NMEMB; i++)
    if (f->b == kid_vjump_frameset[i].frame) return true;
  return false;
}

bool
is_kid_vjump_touching_above (struct frame *f)
{
  int i;
  for (i = 12; i < 14; i++)
    if (f->b == kid_vjump_frameset[i].frame) return true;
  return false;
}

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

#include "prince.h"
#include "kernel/video.h"
#include "kernel/keyboard.h"
#include "engine/anim.h"
#include "engine/physics.h"
#include "engine/door.h"
#include "engine/potion.h"
#include "engine/sword.h"
#include "engine/loose-floor.h"
#include "kid.h"

struct frameset kid_vjump_frameset[KID_VJUMP_FRAMESET_NMEMB];

static void init_kid_vjump_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_vjump_01, *kid_vjump_02, *kid_vjump_03, *kid_vjump_04,
  *kid_vjump_05, *kid_vjump_06, *kid_vjump_07, *kid_vjump_08, *kid_vjump_09,
  *kid_vjump_10, *kid_vjump_11, *kid_vjump_12, *kid_vjump_13, *kid_vjump_15,
  *kid_vjump_16, *kid_vjump_17, *kid_vjump_18, *kid_vjump_19;

static void
init_kid_vjump_frameset (void)
{
  struct frameset frameset[KID_VJUMP_FRAMESET_NMEMB] =
    {{kid_vjump_01,+3,+0},{kid_vjump_02,-1,+0},{kid_vjump_03,-1,+0},
     {kid_vjump_04,+2,+0},{kid_vjump_05,+0,+0},{kid_vjump_06,-1,+0},
     {kid_vjump_07,-1,+0},{kid_vjump_08,-3,+0},{kid_vjump_09,-1,+0},
     {kid_vjump_10,-6,+0},{kid_vjump_11,+0,+0},{kid_vjump_12,+2,-3},
     {kid_vjump_13,+3,-7},{kid_vjump_15,+0,+8},{kid_vjump_16,+3,+2},
     {kid_vjump_17,-1,+0},{kid_vjump_18,+0,+0},{kid_vjump_19,+0,0}};

  memcpy (&kid_vjump_frameset, &frameset,
          KID_VJUMP_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_vjump (void)
{
  /* bitmaps */
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
  kid_vjump_15 = load_bitmap (KID_VJUMP_15);
  kid_vjump_16 = load_bitmap (KID_VJUMP_16);
  kid_vjump_17 = load_bitmap (KID_VJUMP_17);
  kid_vjump_18 = load_bitmap (KID_VJUMP_18);
  kid_vjump_19 = load_bitmap (KID_VJUMP_19);

  /* frameset */
  init_kid_vjump_frameset ();
}

void
unload_kid_vjump (void)
{
  al_destroy_bitmap (kid_vjump_01);
  al_destroy_bitmap (kid_vjump_02);
  al_destroy_bitmap (kid_vjump_03);
  al_destroy_bitmap (kid_vjump_04);
  al_destroy_bitmap (kid_vjump_05);
  al_destroy_bitmap (kid_vjump_06);
  al_destroy_bitmap (kid_vjump_07);
  al_destroy_bitmap (kid_vjump_08);
  al_destroy_bitmap (kid_vjump_09);
  al_destroy_bitmap (kid_vjump_10);
  al_destroy_bitmap (kid_vjump_11);
  al_destroy_bitmap (kid_vjump_12);
  al_destroy_bitmap (kid_vjump_13);
  al_destroy_bitmap (kid_vjump_15);
  al_destroy_bitmap (kid_vjump_16);
  al_destroy_bitmap (kid_vjump_17);
  al_destroy_bitmap (kid_vjump_18);
  al_destroy_bitmap (kid_vjump_19);
}

void
kid_vjump (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_vjump;
  kid->f.flip = (kid->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame (&kid->f, &kid->f, &kid->fo);
  physics_out (kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_vjump)
    kid->i = -1, kid->j = 0,
      kid->just_hanged = kid->hit_ceiling =
      kid->hang = kid->misstep = false;

  if (kid->i == 12 && kid->hang) {
    kid_hang (kid);
    return false;
  }

  if (kid->i == 17) {
    kid_normal (kid);
    return false;
  }

  if (kid->oaction == kid_hang_wall
      || kid->oaction == kid_hang_free) {
    kid->just_hanged = true;
    kid->i = 13;
  } else if (kid->i == 12 && kid->j > 0
             && ! kid->hit_ceiling) kid->i = 12;
  else kid->i++;

  select_frame (kid, kid_vjump_frameset, kid->i);

  if (kid->oaction == kid_hang_free
      && is_hang_pos_critical (&kid->hang_pos))
    kid->fo.dx -= (kid->f.dir == LEFT) ? 9 : 13;
  if (kid->hang && is_hang_pos_critical (&kid->hang_pos)
      && kid->i == 11) kid->fo.dx = +7;
  if (kid->i == 12 && kid->j++ > 0)
    kid->fo.dx = 0, kid->fo.dy += 2 * kid->j + 1;
  if (kid->j == 4) kid->j = 0;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc, tf; struct pos np, ptf, ptb, pm, pmf, pmba;

  /* fall */
  survey (_m, pos, &kid->f, &nc, &pm, &np);
  survey (_mf, pos, &kid->f, &nc, &pmf, &np);
  survey (_mba, pos, &kid->f, &nc, &pmba, &np);
  if (is_strictly_traversable (&pm)
      && is_strictly_traversable (&pmf)
      && is_strictly_traversable (&pmba)) {
    kid_fall (kid);
    return false;
  }

  /* ceiling hit */
  survey (_tb, pos, &kid->f, &nc, &ptb, &np);
  struct pos ptba; prel (&ptb, &ptba, -1, 0);
  if (kid->i == 12 && kid->j == 1
      && ! is_strictly_traversable (&ptba))
    kid->hit_ceiling = true;

  /* hang */
  int dir = (kid->f.dir == LEFT) ? +1 : -1;
  survey (_tf, pos, &kid->f, &tf, &ptf, &np);
  if (kid->i == 0
      && is_hangable_pos (prel (&ptf, &np, 0, dir), kid->f.dir)
      && ! (con (&ptf)->fg == DOOR
            && tf.y <= door_grid_tip_y (&ptf) - 10)) {
    prel (&ptf, &kid->hang_pos, 0, dir);
    pos2room (&kid->hang_pos, kid->f.c.room, &kid->hang_pos);
    kid->fo.dx += is_hang_pos_critical (&kid->hang_pos) ? -12 : -3;
    kid->hang = true;
  } else if (kid->i == 0 && can_hang (&kid->f, false, &kid->hang_pos)
             && ! is_hang_pos_critical (&kid->hang_pos)
             && (kid->f.dir == LEFT || con (&kid->hang_pos)->fg != DOOR)) {
    kid->fo.dx -= 4; kid->hang = true;
  }

  if (kid->i == 0 && kid->hang)
    place_frame (&kid->f, &kid->f, kid_vjump_frameset[0].frame,
                 &kid->hang_pos, (kid->f.dir == LEFT) ? +14 : PLACE_WIDTH + 5, +16);

  return true;
}

static void
physics_out (struct anim *kid)
{
  struct coord nc; struct pos np, ptf, ptb, pmbo;
  enum confg ctf, ctb;

 /* depressible floors */
  if (kid->i == 0 && kid->hang
      && ! peq (&kid->hang_pos, &kid->df_pos[0])
      && ! peq (&kid->hang_pos, &kid->df_pos[1]))
    update_depressible_floor (kid, -1, -8);
  else if (kid->i == 11) {
    save_depressible_floor (kid);
    clear_depressible_floor (kid);
  } else if (kid->i == 14 && ! kid->just_hanged) {
    restore_depressible_floor (kid);
    keep_depressible_floor (kid);
  } else if (kid->i == 14 && kid->just_hanged)
    update_depressible_floor (kid, -5, -7);
  else keep_depressible_floor (kid);

  /* ceiling loose floor shaking and release */
  if (kid->i == 13 && kid->hit_ceiling) {
    ctb = survey (_tb, pos, &kid->f, &nc, &ptb, &np)->fg;
    ctf = survey (_tf, pos, &kid->f, &nc, &ptf, &np)->fg;
    shake_loose_floor_row (&ptb);
    if (ctb == LOOSE_FLOOR) release_loose_floor (&ptb);
    if (ctf == LOOSE_FLOOR) release_loose_floor (&ptf);
  }

  /* loose floor shaking */
  survey (_mbo, pos, &kid->f, &nc, &pmbo, &np);
  if (kid->i == 17) shake_loose_floor_row (&pmbo);
}

bool
is_kid_vjump (struct frame *f)
{
  int i;
  for (i = 0; i < KID_VJUMP_FRAMESET_NMEMB; i ++)
    if (f->b == kid_vjump_frameset[i].frame) return true;
  return false;
}

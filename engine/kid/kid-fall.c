/*
  kid-fall.c -- kid fall module;

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

struct frameset kid_fall_frameset[KID_FALL_FRAMESET_NMEMB];

static void init_kid_fall_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_fall_13, *kid_fall_14, *kid_fall_15,
  *kid_fall_16, *kid_fall_17;

static void
init_kid_fall_frameset (void)
{
  struct frameset frameset[KID_FALL_FRAMESET_NMEMB] =
    {{kid_fall_13,+0,+0},{kid_fall_14,+0,+5},{kid_fall_15,+0,+10},
     {kid_fall_16,+0,+11},{kid_fall_17,+0,+20}};

  memcpy (&kid_fall_frameset, &frameset,
          KID_FALL_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_fall (void)
{
  /* bitmaps */
  kid_fall_13 = load_bitmap (KID_FALL_13);
  kid_fall_14 = load_bitmap (KID_FALL_14);
  kid_fall_15 = load_bitmap (KID_FALL_15);
  kid_fall_16 = load_bitmap (KID_FALL_16);
  kid_fall_17 = load_bitmap (KID_FALL_17);

  /* frameset */
  init_kid_fall_frameset ();
}

void
unload_kid_fall (void)
{
  al_destroy_bitmap (kid_fall_13);
  al_destroy_bitmap (kid_fall_14);
  al_destroy_bitmap (kid_fall_15);
  al_destroy_bitmap (kid_fall_16);
  al_destroy_bitmap (kid_fall_17);
}

void
kid_fall (void)
{
  kid.oaction = kid.action;
  kid.action = kid_fall;
  kid.f.flip = (kid.f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_fall) kid->i = -1;

  kid->i++;

  kid->fo.b = kid_fall_frameset[kid->i > 4 ? 4 : kid->i].frame;
  kid->fo.dx = kid_fall_frameset[kid->i > 4 ? 4 : kid->i].dx;
  kid->fo.dy = kid_fall_frameset[kid->i > 4 ? 4 : kid->i].dy;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos ptf, pbf, pm, np;
  enum confg cbf, cbb, ctf, cm;

  int dir = (kid->f.dir == LEFT) ? -1 : +1;

  /* ensure kid's proximity for hang */
  ctf = survey (_tf, pos, &kid->f, &nc, &ptf, &np)->fg;
  if (kid->i == 0 && crel (&ptf, 0, dir)->fg != NO_FLOOR
      && crel (&ptf, 0, dir)->fg != WALL
      && kid->oaction == kid_walk) {
    to_next_place_edge (&kid->f, &kid->f, kid->fo.b, _tf, pos, 0, false, 0);
  } else if (ctf != NO_FLOOR) inertia = 0;
  else {
    if (kid->oaction == kid_run_jump) kid->fo.dx = -16, kid->fo.dy= +6;
    if (kid->oaction == kid_jump) kid->fo.dx = -16, kid->fo.dy = +6;
  }

  /* put kid in front of the floor */
  cbf = survey (_bf, pos, &kid->f, &nc, &pbf, &np)->fg;
  cbb = survey (_bb, pos, &kid->f, &nc, &np, &np)->fg;
  bool should_move_to_front = cbf == NO_FLOOR && cbb != NO_FLOOR;
  bool should_move_to_back = cbf != NO_FLOOR && cbb == NO_FLOOR;
  coord_f cf = (should_move_to_front) ? _bb : _bf;

  if (kid->i == 0 && (should_move_to_front || should_move_to_back)) {
    next_frame (&kid->f, &kid->f, kid->fo.b, +0, 0);
    int dirm = dir;
    dirm *= (should_move_to_back) ? -1 : +1;
    int dx = 0;

    enum confg t;

    do {
      dx += dirm;
      kid->f.c.x += dirm;
      nframe (&kid->f, &kid->f.c);
      t = survey (cf, pos, &kid->f, &nc, &np, &np)->fg;
    } while (t != NO_FLOOR && abs (dx) < PLACE_WIDTH);

    kid->f.c.x += -dirm * 12;
    kid->f.c.y += 6;
  }

  /* hang */
  if (kid->i > 2 && can_hang (&kid->f) && shift_key && ! hang_limit) {
    sample_hang_on_fall = true;
    kid_hang ();
    return false;
  }

  /* turn run */
  if (kid->oaction == kid_turn_run) {
    if (kid->f.b != kid_turn_run_frameset[8].frame)
      kid->f.dir = (kid->f.dir == LEFT) ? RIGHT : LEFT;
    kid->f.flip = (kid->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
    kid->f.c.x += (kid->f.dir == LEFT) ? +12 : -12;
    kid->f.c.y += 12;
  }

  /* land on ground */
  struct frame nf; next_frame (&kid->f, &nf, kid->f.b, 0, 34);
  struct pos npmbo, pmbo, npmbo_nf;
  enum confg cmbo;
  survey (_mbo, pos, &nf, &nc, &np, &npmbo_nf);
  cmbo = survey (_mbo, pos, &kid->f, &nc, &pmbo, &npmbo)->fg;
  cm = survey (_m, pos, &kid->f, &nc, &pm, &np)->fg;

  struct pos pr;
  struct loose_floor *l =
    loose_floor_at_pos (prel (&pm, &pr, -1, +0));

  if (kid->i > 0
      && cmbo != NO_FLOOR
      && npmbo.floor != npmbo_nf.floor
      && ! (l && l->action == FALL_LOOSE_FLOOR && cm == LOOSE_FLOOR)) {
    inertia = 0;

    kid->f.c.y = 63 * pbf.floor + 15;
    kid->f.c.x += (kid->f.dir == LEFT) ? -6 : +6;
    kid->f.b = kid_normal_00;

    cbf = survey (_bf, pos, &kid->f, &nc, &pbf, &np)->fg;

    /* ensure kid isn't colliding */
    if (cbf == WALL || cbf == DOOR)
      to_next_place_edge (&kid->f, &kid->f, kid->fo.b, _bf, pos, 0, true, -1);

    if (kid->i >= 8) {
      kid_current_lives--;
      uncouch_slowly = true;
    } else uncouch_slowly = false;
    if (kid->i > 3) sample_hit_ground = true;
    shake_loose_floor_row (&pbf);
    kid_couch ();
    return false;
  }

  if (kid->i == 1) kid->fo.dx = -inertia;
  if (kid->i > 1) kid->fo.dx = -inertia;
  if (kid->i > 4) {
    int speed = +21 + 3 * (kid->i - 5);
    kid->fo.dy = (speed > 33) ? 33 : speed;
  }

  /* collision */
  next_frame (&kid->f, &nf, kid->fo.b, kid->fo.dx, kid->fo.dy);
  cbf = survey (_bf, pos, &kid->f, &nc, &pbf, &np)->fg;
  if (cbf == WALL || (nf.dir == LEFT && cbf == DOOR)) {
    to_next_place_edge (&nf, &kid->f, kid->fo.b, _bf, pos, 0, true, -1);
    kid->fo.dx = 0; kid->fo.dy = 0;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  clear_depressible_floor (kid);
}

bool
is_kid_fall (struct frame *f)
{
  return f->b == kid_fall_13
    || f->b == kid_fall_14
    || f->b == kid_fall_15
    || f->b == kid_fall_16
    || f->b == kid_fall_17;
}

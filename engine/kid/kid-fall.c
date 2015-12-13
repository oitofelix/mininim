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

#include <stdio.h>
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
static void place_in_initial_fall (struct anim *kid);

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
kid_fall (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_fall;
  kid->f.flip = (kid->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame (&kid->f, &kid->f, &kid->fo);
  physics_out (kid);
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
  struct coord nc;
  struct pos np, pbf, ptf, pmt, pmtf, pmtb, pbb,
    npmbo, npmbo_nf;
  enum confg cmbo;
  struct frame nf;
  struct frame_offset fo;

  bool hang_back = ((kid->f.dir == LEFT) ? right_key : left_key)
    && ! up_key && shift_key;

  bool hang_front = shift_key && ! hang_back;

  int dir = (kid->f.dir == LEFT) ? -1 : +1;

  if (kid->oaction == kid_jump) {
    next_frame (&kid->f, &kid->f, &kid->fo);
    kid->f.c.x += dir * 8;
    kid->f.c.y += 6;
  } else if (kid->oaction == kid_run_jump) {
    next_frame (&kid->f, &kid->f, &kid->fo);
    kid->f.c.x += dir * 12;
    kid->f.c.y += 6;
  }

  if (kid->i == 0
      && kid->oaction != kid_hang_wall
      && kid->oaction != kid_hang_free) {
    next_frame (&kid->f, &kid->f, &kid->fo);

    int dirf = (kid->f.dir == LEFT) ? -1 : +1;
    int dirb = (kid->f.dir == LEFT) ? +1 : -1;
    survey (_mt, pos, &kid->f, &nc, &pmt, &np);
    prel (&pmt, &pmtf, +0, dirf);
    prel (&pmt, &pmtb, +0, dirb);

    if (! is_strictly_traversable (&pmt)
        || ! is_strictly_traversable (&pmtf)
        || ! is_strictly_traversable (&pmtb))
      place_in_initial_fall (kid);
  }

  /* help kid hang */
  survey (_tf, pos, &kid->f, &nc, &ptf, &np);
  survey (_bb, pos, &kid->f, &nc, &pbb, &np);
  if (is_hangable_con (&ptf)
      || is_hangable_pos (&pbb, kid->f.dir))
    kid->inertia = 0;

  /* fall speed */
  if (kid->i > 0)
    kid->fo.dx = -kid->inertia;
  if (kid->i > 4) {
    int speed = +21 + 3 * (kid->i - 5);
    kid->fo.dy = (speed > 33) ? 33 : speed;
  }

  printf ("inertia: %i\n", kid->inertia);

  /* collision */
  if (is_colliding (&kid->f, &kid->fo, +0, false))
    kid->fo.dx = 0;

  /* hang front */
  if (kid->i > 2 && can_hang (&kid->f, false)
      && hang_front && ! hang_limit) {
    sample_hang_on_fall = true;
    kid_hang (kid);
    return false;
  }

  /* hang back */
  if (kid->i > 2 && can_hang (&kid->f, true)
      && hang_back && ! hang_limit) {
    sample_hang_on_fall = true;
    kid_turn (kid);
    return false;
  }

  /* land on ground */
  cmbo = survey (_mbo, pos, &kid->f, &nc, &np, &npmbo)->fg;
  fo.b = kid->f.b;
  fo.dx = 0;
  fo.dy = 34;
  next_frame (&kid->f, &nf, &fo);
  survey (_mbo, pos, &nf, &nc, &np, &npmbo_nf);

  if (kid->i > 2
      && cmbo != NO_FLOOR
      && npmbo.floor != npmbo_nf.floor) {
    kid->inertia = 0;

    if (is_colliding (&kid->f, &kid->fo, +16, false))
      kid->f.c.x += (kid->f.dir == LEFT) ? +16 : -16;

    survey (_bf, pos, &kid->f, &nc, &pbf, &np);
    pos2view (&pbf, &pbf);
    kid->fo.b = kid_couch_frameset[0].frame;
    kid->fo.dx = kid->fo.dy = 0;
    kid->f.c.room = pbf.room;
    kid->f.c.x += (kid->f.dir == LEFT) ? -6 : +6;
    kid->f.c.y = PLACE_HEIGHT * pbf.floor + 27;
    kid->f.b = kid_couch_frameset[0].frame;

    if (kid->i >= 8) {
      kid_current_lives--;
      uncouch_slowly = true;
    } else uncouch_slowly = false;
    if (kid->i > 3) sample_hit_ground = true;
    shake_loose_floor_row (&pbf);
    kid_couch (kid);
    return false;
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

static void
place_in_initial_fall (struct anim *kid)
{
  struct coord nc;
  struct pos np, pmt, pmtf, pmtb;
  struct pos fall_pos;

  int dirf = (kid->f.dir == LEFT) ? -1 : +1;
  int dirb = (kid->f.dir == LEFT) ? +1 : -1;
  survey (_mt, pos, &kid->f, &nc, &pmt, &np);
  prel (&pmt, &pmtf, +0, dirf);
  prel (&pmt, &pmtb, +0, dirb);

  fall_pos.room = -1;

  if (is_strictly_traversable (&pmt)) fall_pos = pmt;
  else if (is_strictly_traversable (&pmtf)) fall_pos = pmtf;
  else if (is_strictly_traversable (&pmtb)) fall_pos = pmtb;

  if (fall_pos.room != - 1)
    place_frame (&kid->f, &kid->f, kid_fall_frameset[0].frame,
                 &fall_pos,
                 (kid->f.dir == LEFT) ? PLACE_WIDTH - 12 : +16,
                 (kid->f.dir == LEFT) ? 23 : 27);
}

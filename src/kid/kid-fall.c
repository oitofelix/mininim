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

#include "mininim.h"

struct frameset kid_fall_frameset[KID_FALL_FRAMESET_NMEMB];

static void init_kid_fall_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_fall_00, *kid_fall_01, *kid_fall_02,
  *kid_fall_03, *kid_fall_04;

static void
init_kid_fall_frameset (void)
{
  struct frameset frameset[KID_FALL_FRAMESET_NMEMB] =
    {{kid_fall_00,+0,+0},{kid_fall_01,+0,+5},{kid_fall_02,+0,+10},
     {kid_fall_03,+0,+11},{kid_fall_04,+0,+20}};

  memcpy (&kid_fall_frameset, &frameset,
          KID_FALL_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_fall (void)
{
  /* bitmaps */
  kid_fall_00 = load_bitmap (KID_FALL_00);
  kid_fall_01 = load_bitmap (KID_FALL_01);
  kid_fall_02 = load_bitmap (KID_FALL_02);
  kid_fall_03 = load_bitmap (KID_FALL_03);
  kid_fall_04 = load_bitmap (KID_FALL_04);

  /* frameset */
  init_kid_fall_frameset ();
}

void
unload_kid_fall (void)
{
  destroy_bitmap (kid_fall_00);
  destroy_bitmap (kid_fall_01);
  destroy_bitmap (kid_fall_02);
  destroy_bitmap (kid_fall_03);
  destroy_bitmap (kid_fall_04);
}

void
kid_fall (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_fall;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_fall) {
    k->j = k->i;
    k->i = -1;
  }

  k->i++;

  k->fo.b = kid_fall_frameset[k->i > 4 ? 4 : k->i].frame;
  k->fo.dx = kid_fall_frameset[k->i > 4 ? 4 : k->i].dx;
  k->fo.dy = kid_fall_frameset[k->i > 4 ? 4 : k->i].dy;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct coord nc;
  struct pos np, pbb, pbf, pmt,
    npmbo, npmbo_nf;
  struct frame nf;
  struct frame_offset fo;

  bool hang_back = ((k->f.dir == LEFT) ? k->key.right : k->key.left)
    && ! k->key.up && k->key.shift
    && (k->i < 9 || k->float_timer)
    && k->current_lives > 0;

  bool hang_front = k->key.shift && ! hang_back
    && (k->i < 9 || k->float_timer)
    && k->current_lives > 0;

  int dir = (k->f.dir == LEFT) ? -1 : +1;

  survey (_bf, pos, &k->f, &nc, &pbf, &np);
  survey (_bb, pos, &k->f, &nc, &pbb, &np);

  if (k->oaction == kid_jump && k->j == 10) {
    k->fo.dx = -4;
    k->fo.dy = +8;
  } else if (k->oaction == kid_run_jump && k->j == 10) {
    k->fo.dx = -8;
    k->fo.dy = +4;
  } else if (k->oaction == kid_turn_run)
    k->fo.dx = +20;
  else if (k->oaction == kid_couch && k->collision) {
    k->collision = false;
    k->f.c.x += dir * 16;
  } else if (k->i == 0
             && k->oaction != kid_hang_free
             && k->oaction != kid_hang_wall
             && k->oaction != kid_climb) {
    place_kid_in_initial_fall (k);
    k->inertia = 0;
  }

  /* fall speed */
  if (k->i > 0)
    k->fo.dx = -k->inertia;
  if (k->i > 4) {
    int speed = +21 + 3 * (k->i - 5);
    k->fo.dy = (speed > 33) ? 33 : speed;
  }

  if (k->i == 4) k->fo.dx += +4;

  /* floating */
  if (k->float_timer) {
    if (k->float_timer < SEC2CYC (16)) {
      k->fo.dx = -1;
      k->fo.dy = +3;
    } else {
      k->float_timer = 0;
      k->i = (k->i > 4) ? 4 : k->i;
    }
  }

  /* printf ("inertia: %i\n", k->inertia); */

  /* collision */
  if (is_colliding (&k->f, &k->fo, +0, false, &k->ci)
      && (k->ci.t != MIRROR || ! k->crossing_mirror)) {
    k->inertia = 0;
    k->fo.dx = +0;
    uncollide (&k->f, &k->fo, &k->fo, +0, false, &k->ci);
  }

  /* hang front */
  if (k->i > 4 && can_hang (&k->f, false, &k->hang_pos)
      && hang_front && ! k->hang_limit) {
    k->hit_by_loose_floor = false;
    stop_sample (k->sample, scream_sample);
    play_sample (hang_on_fall_sample, k->f.c.room);
    kid_hang (k);
    return false;
  }

  /* hang back */
  if (k->i > 4 && can_hang (&k->f, true, &k->hang_pos)
      && hang_back && ! k->hang_limit) {
    k->hit_by_loose_floor = false;
    stop_sample (k->sample, scream_sample);
    play_sample (hang_on_fall_sample, k->f.c.room);
    kid_turn (k);
    return false;
  }

  /* land on ground */
  fo.b = kid_fall_frameset[k->i > 4 ? 4 : k->i].frame;
  fo.dx = kid_fall_frameset[k->i > 4 ? 4 : k->i].dx;
  fo.dy = kid_fall_frameset[k->i > 4 ? 4 : k->i].dy;

  if (k->float_timer) fo.dy = 14;
  else {
    if (k->i > 0) fo.dx = -k->inertia;
    if (k->i > 4) {
      int speed = +21 + 3 * (k->i - 5);
      fo.dy = (speed > 33) ? 33 : speed;
    }
    fo.dy += 8;
  }

  survey (_mbo, pos, &k->f, &nc, &np, &npmbo);
  next_frame (&k->f, &nf, &fo);
  survey (_mbo, pos, &nf, &nc, &np, &npmbo_nf);

  if (k->i > 1 &&
      ! is_strictly_traversable (&npmbo)
      && npmbo.floor != npmbo_nf.floor) {
    k->inertia = k->cinertia = 0;

    survey (_bf, pos, &k->f, &nc, &pbf, &np);
    /* pos2view (&pbf, &pbf); */
    k->fo.b = kid_couch_frameset[0].frame;
    k->fo.dx = k->fo.dy = 0;
    k->f.c.room = pbf.room;
    k->f.c.y = PLACE_HEIGHT * pbf.floor + 27;
    k->f.c.x += dir * 4;
    k->f.b = kid_couch_frameset[0].frame;

    shake_loose_floor_row (&pbf);

    if (k->i >= 8 && ! k->immortal
        && ! k->fall_immune
        && ! k->float_timer) {
      k->hurt = true;
      k->splash = true;
      k->current_lives--;

      if (k->i >= 10) k->current_lives = 0;

      if (k->current_lives > 0) {
        play_sample (hit_ground_harm_sample, k->f.c.room);
        k->uncouch_slowly = true;
      }
      if (k->id == current_kid_id) {
        mr.flicker = 2;
        mr.color = get_flicker_blood_color ();
      }
    } else if (k->i > 3 && ! k->float_timer) {
      play_sample (hit_ground_sample, k->f.c.room);
      k->hurt = false;
    } else k->hurt = false;

    survey (_mt, pos, &k->f, &nc, &pmt, &np);
    if (k->current_lives <= 0) {
      stop_sample (k->sample, scream_sample);
      k->p = pmt;
      if (con (&pmt)->fg == SPIKES_FLOOR
          && ! spikes_floor_at_pos (&pmt)->inactive)
        kid_die_spiked (k);
      else {
        play_sample (hit_ground_fatal_sample, k->f.c.room);
        kid_die_suddenly (k);
        k->death_reason = FALL_DEATH;
      }
    }
    else {
      stop_sample (k->sample, scream_sample);
      kid_couch (k);
    }

    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  /* depressible floors */
  clear_depressible_floor (k);

  /* sound */
  if (k->i == 10
      && ! k->float_timer
      && k->current_lives > 0)
    k->sample = play_sample (scream_sample, k->f.c.room);
}

bool
is_kid_fall (struct frame *f)
{
  return f->b == kid_fall_00
    || f->b == kid_fall_01
    || f->b == kid_fall_02
    || f->b == kid_fall_03
    || f->b == kid_fall_04;
}

void
place_kid_in_initial_fall (struct anim *k)
{
  struct coord nc;
  struct pos np, pbf, pmbo, pbb;
  struct pos fall_pos;

  survey (_bf, pos, &k->f, &nc, &pbf, &np);
  survey (_mbo, pos, &k->f, &nc, &pmbo, &np);
  survey (_bb, pos, &k->f, &nc, &pbb, &np);

  fall_pos.room = -1;

  if (is_strictly_traversable (&pmbo))
    fall_pos = pmbo;
  else if (is_strictly_traversable (&pbf))
    fall_pos = pbf;
  else if (is_strictly_traversable (&pbb))
    fall_pos = pbb;

  if (fall_pos.room != - 1)
    place_frame (&k->f, &k->f, kid_fall_frameset[0].frame,
                 &fall_pos,
                 (k->f.dir == LEFT) ? +24 : +10,
                 (k->f.dir == LEFT) ? 18 : 18);
}

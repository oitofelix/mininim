/*
  kid-fall.c -- kid fall module;

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
kid_fall (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_fall;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_fall) {
    k->j = k->i;
    k->i = -1;
  }

  k->i++;

  int i = k->i > 4 ? 4 : k->i;
  k->fo.b = actor_bitmap (k, "KID", "FALL", i);
  k->fo.dx = actor_bitmap_dx (k, "KID", "FALL", i);
  k->fo.dy = actor_bitmap_dy (k, "KID", "FALL", i);

  return true;
}

static bool
physics_in (struct actor *k)
{
  bool hang_back = ((k->f.dir == LEFT) ? k->key.right : k->key.left)
    && ! k->key.up && k->key.shift
    && (k->i < 9 || k->float_timer)
    && k->current_lives > 0;

  bool hang_front = k->key.shift && ! hang_back
    && (k->i < 9 || k->float_timer)
    && k->current_lives > 0;

  /* fall speed */
  int inertia = k->inertia;

  int speed;
  if (k->i <= 4 || k->float_timer) speed = 0;
  else speed = +21 + 3 * (k->i - 5);

  if (k->float_timer && k->float_timer < FLOAT_TIMER_MAX) {
    /* floating */
    k->fo.dx = -k->inertia / 2 - 2;
    k->fo.dy = +5;
  } else {
    if (k->float_timer) k->i = (k->i > 4) ? 4 : k->i;
    if (k->i > 0) k->fo.dx = -k->inertia;
    if (k->i > 4) k->fo.dy = (speed > 33) ? 33 : speed;
    if (k->i == 4) k->fo.dx += +4;
  }

  if (k->oaction == kid_jump
      && (k->j == 10 || k->j == 11)) {
    k->fo.dx = -4;
    k->fo.dy = +8;
  } else if (k->oaction == kid_run_jump && k->j == 10) {
    k->fo.dx = -8;
    k->fo.dy = +4;
  } else if (k->oaction == kid_turn_run) {
    k->fo.dx = (k->float_timer > 0) ? +26 : +20;
    uncollide (&k->f, &k->fo, _bb, +0, +0, &k->fo, NULL);
  } else if (k->oaction == kid_crouch && k->collision) {
    k->collision = false;
    k->fo.dx += -16;
  } else if (k->oaction == kid_crouch) {
    k->fo.dx += -12;
  } else if (k->i == 0
             && k->oaction != kid_normal
             && k->oaction != kid_hang_free
             && k->oaction != kid_hang_non_free
             && k->oaction != kid_climb) {
    place_kid_in_initial_fall (k);
    k->inertia = 0;
  }

  /* printf ("inertia: %i\n", k->inertia); */

  /* collision */
  uncollide (&k->f, &k->fo, _bf, -8, +0, &k->fo, NULL);
  uncollide (&k->f, &k->fo, _tf, -8, +0, &k->fo, NULL);

  /* hang front */
  if (k->i > 4 && can_hang (k, false, &k->hang_pos)
      && hang_front && ! k->hang_limit) {
    k->hit_by_loose_floor = false;
    stop_audio_instance (&scream_audio, NULL, k->id);
    play_audio (&hang_on_fall_audio, NULL, k->id);
    kid_haptic (k, KID_HAPTIC_HANG);
    kid_hang (k);
    return false;
  }

  /* hang back */
  if (movements == NATIVE_MOVEMENTS
      && k->i > 4 && can_hang (k, true, &k->hang_pos)
      && hang_back && ! k->hang_limit) {
    k->hit_by_loose_floor = false;
    stop_audio_instance (&scream_audio, NULL, k->id);
    play_audio (&hang_on_fall_audio, NULL, k->id);
    kid_haptic (k, KID_HAPTIC_HANG);
    kid_turn (k);
    return false;
  }

  /* land on ground */
  struct frame_offset fo;

  int i = k->i > 4 ? 4 : k->i;
  fo.b = actor_bitmap (k, "KID", "FALL", i);
  fo.dx = actor_bitmap_dx (k, "KID", "FALL", i);
  fo.dy = actor_bitmap_dy (k, "KID", "FALL", i);

  if (k->float_timer) fo.dy = 14;
  else {
    if (k->i > 0) fo.dx = -k->inertia;
    if (k->i > 4)
      fo.dy = (speed > 33) ? 33 : speed;
    fo.dy += 8;
  }

  struct pos pbf, pbf_nf, pmbo, pmbo_nf, pmt;
  struct frame nf;
  surveyo (_bf, -8, +0, pos, &k->f, NULL, &pbf, NULL);
  surveyo (_mbo, +0, speed > 22 ? -8 : 0, pos, &k->f, NULL, NULL, &pmbo);
  next_frame (&k->f, &nf, &fo);
  surveyo (_mbo, +0, speed > 22 ? -8 : 0, pos, &nf, NULL, NULL, &pmbo_nf);
  surveyo (_bf, -8, +0, pos, &nf, NULL, &pbf_nf, NULL);

  if (k->i > 1 &&
      ! is_traversable (&pmbo)
      && pmbo.floor != pmbo_nf.floor) {
    k->inertia = k->cinertia = 0;
    k->fo.dx = k->fo.dy = 0;
    k->fo.b = actor_bitmap (k, "KID", "CROUCH", 0);
    k->f.b = actor_bitmap (k, "KID", "CROUCH", 0);

    place_on_the_ground (&k->f, &k->f.c);

    /* survey (_bf, pos, &k->f, NULL, &pbf, NULL); */
    /* new_coord (&k->f.c, k->f.c.l, pbf.room, */
    /*            k->f.c.x, */
    /*            PLACE_HEIGHT * pbf.floor + 27); */

    move_frame (&k->f, _bf, -8, inertia, inertia);

    shake_loose_floor_row (&pbf);

    if (k->i >= 8 && ! k->immortal
        && ! k->fall_immune
        && ! k->float_timer) {
      k->hurt = true;
      k->splash = true;
      k->current_lives--;

      if (k->i >= 10) k->current_lives = 0;

      if (k->current_lives > 0) {
        play_audio (&hit_ground_harm_audio, NULL, k->id);
        k->uncrouch_slowly = true;
        kid_haptic (k, KID_HAPTIC_HARM);
      }
      if (k->id == current_kid_id) {
        mr.flicker = 2;
        mr.color = get_flicker_blood_color ();
      }
    } else if (k->i > 3 && ! k->float_timer) {
      play_audio (&hit_ground_audio, NULL, k->id);
      kid_haptic (k, KID_HAPTIC_STRONG_COLLISION);
      k->hurt = false;
    } else k->hurt = false;

    survey (_mt, pos, &k->f, NULL, &pmt, NULL);
    if (k->current_lives <= 0) {
      stop_audio_instance (&scream_audio, NULL, k->id);
      k->p = pmt;
      if (fg (&pmt) == SPIKES_FLOOR
          && ! spikes_floor_at_pos (&pmt)->inactive)
        kid_die_spiked (k);
      else {
        play_audio (&hit_ground_fatal_audio, NULL, k->id);
        kid_die_suddenly (k);
        k->death_reason = FALL_DEATH;
      }
    } else {
      stop_audio_instance (&scream_audio, NULL, k->id);
      kid_crouch (k);
    }

    return false;
  } else if (! is_immediately_accessible_pos (&pbf_nf, &pbf, &k->f)
             && ! k->float_timer) {
    k->fo.dx = 0;
    k->inertia = k->cinertia = 0;
    move_frame (&k->f, _bf, +0, -4, -4);
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  /* depressible floors */
  clear_depressible_floor (k);

  /* sound */
  if (k->i == 10
      && ! k->float_timer
      && k->current_lives > 0) {
    play_audio (&scream_audio, NULL, k->id);
    kid_haptic (k, KID_HAPTIC_SCREAM);
    if (scream) {
      if (! kid_scream) {
        fprintf (stderr, "In MININIM, the kid screams...\n");
        kid_scream = true;
      }
    }
  }
}

void
place_kid_in_initial_fall (struct actor *k)
{
  struct pos pbf, pmbo, pbb;
  struct pos fall_pos;

  survey (_bf, pos, &k->f, NULL, &pbf, NULL);
  survey (_mbo, pos, &k->f, NULL, &pmbo, NULL);
  survey (_bb, pos, &k->f, NULL, &pbb, NULL);

  invalid_pos (&fall_pos);

  if (is_traversable (&pmbo))
    fall_pos = pmbo;
  else if (is_traversable (&pbf))
    fall_pos = pbf;
  else if (is_traversable (&pbb))
    fall_pos = pbb;

  if (is_valid_pos (&fall_pos)) {
    int dx = (k->f.dir == LEFT) ? +20 : +14;
    int dy = (k->f.dir == LEFT) ? 18 : 18;
    place_actor (k, &fall_pos, dx, dy, "KID", "FALL", 0);
  }
}

/*
  guard-fall.c -- guard fall module;

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

static bool flow (struct actor *g);
static bool physics_in (struct actor *g);
static void physics_out (struct actor *g);
static void place_in_initial_fall (struct actor *g);

void
guard_fall (struct actor *g)
{
  g->oaction = g->action;
  /* fall is detected after actor's next frame has been selected */
  g->oi = g->i - 1;
  g->action = guard_fall;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct actor *g)
{
  if (g->oaction != guard_fall) g->i = -1;

  g->i++;

  int i = g->i > 4 ? 4 : g->i;
  g->fo.b = actor_bitmap (g, NULL, "FALL", i);
  g->fo.dx = actor_dx (g, NULL, "FALL", i);
  g->fo.dy = actor_dy (g, NULL, "FALL", i);

  if (g->i == 0) g->j = 28;
  if (g->i == 1) g->j = 32;
  if (g->i == 2 && g->type == SHADOW) g->j = 29;

  select_actor_xframe (g, NULL, "SWORD", g->j);

  if (g->i == 0) g->xf.dx = -12, g->xf.dy = +4;
  if (g->i >= 2 && g->type != SHADOW
      && g->type != SKELETON) g->xf.b = NULL;

  if (g->type == SKELETON) g->xf.dy += -3;
  if (g->type == SHADOW && g->i < 2) g->xf.dy += -2;

  return true;
}

static bool
physics_in (struct actor *g)
{
  struct pos pbf, pmt, pmtf, pmtb,
    npmbo, npmbo_nf;
  struct frame nf;
  struct frame_offset fo;

  if (g->i == 0) {
    next_frame (&g->f, &g->f, &g->fo);

    int dirf = (g->f.dir == LEFT) ? -1 : +1;
    int dirb = (g->f.dir == LEFT) ? +1 : -1;
    survey (_mt, pos, &g->f, NULL, &pmt, NULL);
    prel (&pmt, &pmtf, +0, dirf);
    prel (&pmt, &pmtb, +0, dirb);

    if (! is_traversable (&pmt)
        || ! is_traversable (&pmtf)
        || ! is_traversable (&pmtb))
      place_in_initial_fall (g);
  }

  /* fall speed */
  if (g->i > 0)
    g->fo.dx = -g->inertia;
  if (g->i > 4) {
    int speed = +21 + 3 * (g->i - 5);
    g->fo.dy = (speed > 33) ? 33 : speed;
  }

  /* land on ground */
  int i = g->i > 4 ? 4 : g->i;
  fo.b = actor_bitmap (g, NULL, "FALL", i);
  fo.dx = actor_dx (g, NULL, "FALL", i);
  fo.dy = actor_dy (g, NULL, "FALL", i);

  if (g->i > 0) g->fo.dx = -g->inertia;
  if (g->i > 2) {
    int speed = +21 + 3 * (g->i - 5);
    fo.dy = (speed > 33) ? 33 : speed;
  }
  fo.dy += 8;

  survey (_mbo, pos, &g->f, NULL, NULL, &npmbo);
  next_frame (&g->f, &nf, &fo);
  survey (_mbo, pos, &nf, NULL, NULL, &npmbo_nf);

  if (g->i > 2
      && ! is_traversable (&npmbo)
      && npmbo.floor != npmbo_nf.floor) {
    g->inertia = g->cinertia = 0;

    /* collision */
    uncollide (&g->f, &g->fo, _bf, -8, +0, &g->fo, NULL);
    uncollide (&g->f, &g->fo, _tf, -8, +0, &g->fo, NULL);

    survey (_bf, pos, &g->f, NULL, &pbf, NULL);
    /* pos2view (&pbf, &pbf); */
    g->f.b = actor_bitmap (g, NULL, "SWORD_NORMAL", 0);
    place_on_the_ground (&g->f, &g->f.c);

    shake_loose_floor_row (&pbf);

    if (g->i >= 8 && ! g->immortal
        && ! g->fall_immune) {
      g->hurt = true;
      g->splash = true;
      g->current_hp--;

      if (g->i >= 10) g->current_hp = 0;

      if (g->current_hp > 0) g->uncrouch_slowly = true;
    } else if (g->i > 3) {
      play_audio (&hit_ground_audio, NULL, g->id);
      g->hurt = false;
    } else g->hurt = false;

    stop_audio_instance (&scream_audio, NULL, g->id);

    survey (_mt, pos, &g->f, NULL, &pmt, NULL);
    g->p = pmt;

    if (fg (&pmt) == SPIKES_FLOOR
        && ! spikes_floor_at_pos (&pmt)->inactive)
      guard_die_spiked (g);
    else if (g->current_hp <= 0) {
      play_audio (&hit_ground_fatal_audio, NULL, g->id);
      guard_die_suddenly (g);
      g->death_reason = FALL_DEATH;
    } else {
      place_on_the_ground (&g->f, &g->f.c);
      g->f.c.y++;
      guard_sword_normal (g);
    }

    return false;
  }

  return true;
}

static void
physics_out (struct actor *g)
{
  /* depressible floors */
  clear_depressible_floor (g);

  /* sound */
  if (g->i == 10
      && g->current_hp > 0) {
    play_audio (&scream_audio, NULL, g->id);
    if (scream)
      switch (g->type) {
      case GUARD:
        if (! guard_scream) {
          fprintf (stderr, "In MININIM, the guard screams...\n");
          guard_scream = true;
        }
        break;
      case FAT:
        if (! fat_scream) {
          fprintf (stderr, "In MININIM, the fat guard screams...\n");
          fat_scream = true;
        }
        break;
      case VIZIER:
        if (! vizier_scream) {
          fprintf (stderr, "In MININIM, the vizier screams...\n");
          vizier_scream = true;
        }
        break;
      case SKELETON:
        if (! skeleton_scream) {
          fprintf (stderr, "In MININIM, the skeleton screams...\n");
          skeleton_scream = true;
        }
        break;
      case SHADOW:
        if (! shadow_scream) {
          fprintf (stderr, "In MININIM, the shadow screams...\n");
          shadow_scream = true;
        }
        break;
      default: break;
      }
  }
}

static void
place_in_initial_fall (struct actor *g)
{
  struct pos pmt, pmtf, pmtb;
  struct pos fall_pos;

  int dirf = (g->f.dir == LEFT) ? -1 : +1;
  int dirb = (g->f.dir == LEFT) ? +1 : -1;
  survey (_mt, pos, &g->f, NULL, &pmt, NULL);
  prel (&pmt, &pmtf, +0, dirf);
  prel (&pmt, &pmtb, +0, dirb);

  invalid_pos (&fall_pos);

  if (is_traversable (&pmt)) fall_pos = pmt;
  else if (is_traversable (&pmtf)) fall_pos = pmtf;
  else if (is_traversable (&pmtb)) fall_pos = pmtb;

  if (is_valid_pos (&fall_pos)) {
    int dx = (g->f.dir == LEFT) ? PLACE_WIDTH - 12 : +6;
    int dy = (g->f.dir == LEFT) ? 23 : 27;
    place_actor (g, &fall_pos, dx, dy, NULL, "FALL", 0);
  }
}

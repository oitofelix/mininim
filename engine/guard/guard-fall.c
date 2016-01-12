/*
  guard-fall.c -- guard fall module;

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
#include "engine/kid/kid.h"
#include "guard.h"

struct frameset guard_fall_frameset[GUARD_FALL_FRAMESET_NMEMB];

static void init_guard_fall_frameset (void);
static bool flow (struct anim *g);
static bool physics_in (struct anim *g);
static void physics_out (struct anim *g);
static void place_in_initial_fall (struct anim *g);

ALLEGRO_BITMAP *guard_fall_01, *guard_fall_02, *guard_fall_03;

static void
init_guard_fall_frameset (void)
{
  struct frameset frameset[GUARD_FALL_FRAMESET_NMEMB] =
    {{guard_fall_01,+0,+0},{guard_fall_02,+0,+5},{guard_fall_03,+0,+10}};

  memcpy (&guard_fall_frameset, &frameset,
          GUARD_FALL_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_guard_fall (void)
{
  /* bitmaps */
  guard_fall_01 = load_bitmap (GUARD_FALL_01);
  guard_fall_02 = load_bitmap (GUARD_FALL_02);
  guard_fall_03 = load_bitmap (GUARD_FALL_03);

  /* frameset */
  init_guard_fall_frameset ();
}

void
unload_guard_fall (void)
{
  al_destroy_bitmap (guard_fall_01);
  al_destroy_bitmap (guard_fall_02);
  al_destroy_bitmap (guard_fall_03);
}

void
guard_fall (struct anim *g)
{
  g->oaction = g->action;
  g->action = guard_fall;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct anim *g)
{
  if (g->oaction != guard_fall) g->i = -1;

  g->i++;

  g->fo.b = guard_fall_frameset[g->i > 2 ? 2 : g->i].frame;
  g->fo.dx = guard_fall_frameset[g->i > 2 ? 2 : g->i].dx;
  g->fo.dy = guard_fall_frameset[g->i > 2 ? 2 : g->i].dy;

  if (g->i == 0) g->j = 28;
  if (g->i == 1) g->j = 32;

  select_xframe (&g->xf, sword_frameset, g->j);

  if (g->i == 0) g->xf.dx = -12, g->xf.dy = +4;
  if (g->i >= 2) g->xf.b = NULL;

  return true;
}

static bool
physics_in (struct anim *g)
{
  struct coord nc;
  struct pos np, pbf, pmt, pmtf, pmtb,
    npmbo, npmbo_nf;
  struct frame nf;
  struct frame_offset fo;

  if (g->i == 0) {
    next_frame (&g->f, &g->f, &g->fo);

    int dirf = (g->f.dir == LEFT) ? -1 : +1;
    int dirb = (g->f.dir == LEFT) ? +1 : -1;
    survey (_mt, pos, &g->f, &nc, &pmt, &np);
    prel (&pmt, &pmtf, +0, dirf);
    prel (&pmt, &pmtb, +0, dirb);

    if (! is_strictly_traversable (&pmt)
        || ! is_strictly_traversable (&pmtf)
        || ! is_strictly_traversable (&pmtb))
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
  fo.b = guard_fall_frameset[g->i > 2 ? 2 : g->i].frame;
  fo.dx = guard_fall_frameset[g->i > 2 ? 2 : g->i].dx;
  fo.dy = guard_fall_frameset[g->i > 2 ? 2 : g->i].dy;

  if (g->i > 0) g->fo.dx = -g->inertia;
  if (g->i > 2) {
    int speed = +21 + 3 * (g->i - 5);
    fo.dy = (speed > 33) ? 33 : speed;
  }
  fo.dy += 6;

  survey (_mbo, pos, &g->f, &nc, &np, &npmbo);
  next_frame (&g->f, &nf, &fo);
  survey (_mbo, pos, &nf, &nc, &np, &npmbo_nf);

  if (g->i > 2
      && ! is_strictly_traversable (&npmbo)
      && npmbo.floor != npmbo_nf.floor) {
    g->inertia = g->cinertia = 0;

    if (is_colliding (&g->f, &g->fo, +16, false, &g->ci)) {
      if (g->ci.t != WALL)
        g->f.c.x += (g->f.dir == LEFT) ? +16 : -16;
      else
        g->f.c.x += (g->f.dir == LEFT) ? +8 : -8;
    }

    survey (_bf, pos, &g->f, &nc, &pbf, &np);
    /* pos2view (&pbf, &pbf); */
    g->fo.b = guard_vigilant_frameset[0].frame;
    g->fo.dx = g->fo.dy = 0;
    g->f.c.room = pbf.room;
    g->f.c.x += (g->f.dir == LEFT) ? -6 : +6;
    g->f.c.y = PLACE_HEIGHT * pbf.floor + 27;
    g->f.b = guard_vigilant_frameset[0].frame;

    shake_loose_floor_row (&pbf);

    if (g->i >= 8 && ! g->immortal
        && ! g->fall_immune) {
      g->hurt = true;
      g->splash = true;
      g->current_lives--;

      if (g->i >= 10) g->current_lives = 0;

      if (g->current_lives > 0) g->uncouch_slowly = true;
    } else if (g->i > 3) {
      play_sample (hit_ground_sample, g->f.c.room);
      g->hurt = false;
    } else g->hurt = false;

    stop_sample (g->sample, scream_sample);

    survey (_mt, pos, &g->f, &nc, &pmt, &np);
    g->p = pmt;

    if (con (&pmt)->fg == SPIKES_FLOOR)
      guard_die_spiked (g);
    else if (g->current_lives <= 0) {
      play_sample (hit_ground_fatal_sample, g->f.c.room);
      guard_die_suddenly (g);
    } else {
      place_on_the_ground (&g->f, &g->f.c);
      g->f.c.y++;
      guard_vigilant (g);
    }

    return false;
  }

  return true;
}

static void
physics_out (struct anim *g)
{
  /* depressible floors */
  clear_depressible_floor (g);

  /* sound */
  if (g->i == 10
      && g->current_lives > 0)
    g->sample = play_sample (scream_sample, g->f.c.room);
}

bool
is_guard_fall (struct frame *f)
{
  return f->b == guard_fall_01
    || f->b == guard_fall_02
    || f->b == guard_fall_03;
}

static void
place_in_initial_fall (struct anim *g)
{
  struct coord nc;
  struct pos np, pmt, pmtf, pmtb;
  struct pos fall_pos;

  int dirf = (g->f.dir == LEFT) ? -1 : +1;
  int dirb = (g->f.dir == LEFT) ? +1 : -1;
  survey (_mt, pos, &g->f, &nc, &pmt, &np);
  prel (&pmt, &pmtf, +0, dirf);
  prel (&pmt, &pmtb, +0, dirb);

  fall_pos.room = -1;

  if (is_strictly_traversable (&pmt)) fall_pos = pmt;
  else if (is_strictly_traversable (&pmtf)) fall_pos = pmtf;
  else if (is_strictly_traversable (&pmtb)) fall_pos = pmtb;

  if (fall_pos.room != - 1)
    place_frame (&g->f, &g->f, guard_fall_frameset[0].frame,
                 &fall_pos,
                 (g->f.dir == LEFT) ? PLACE_WIDTH - 12 : +6,
                 (g->f.dir == LEFT) ? 23 : 27);
}

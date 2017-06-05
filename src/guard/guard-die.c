/*
  guard-die.c -- guard die module;

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

void
guard_resurrect (struct actor *g)
{
  g->splash = false;
  g->invisible = false;
  if (g->current_hp > 0) return;
  struct pos pm;
  survey (_m, pos, &g->f, NULL, &pm, NULL);
  g->current_hp = g->total_hp;
  g->death_reason = NO_DEATH;
  g->action = guard_normal;
  g->glory_sample = false;
  int dx = g->f.dir == LEFT ? +16 : +22;
  int dy = +14;
  place_actor (g, &pm, dx, dy, NULL, "NORMAL", 0);
  place_on_the_ground (&g->f, &g->f.c);
  if (fg (&g->p) == SPIKES_FLOOR)
    spikes_floor_at_pos (&g->p)->inactive = false;
}

void
raise_skeleton (struct actor *s)
{
  s->oaction = s->action;
  s->action = raise_skeleton;
  s->f.flip = (s->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (s->oaction != raise_skeleton) {
    play_audio (&skeleton_audio, NULL, s->id);

    int dx = (s->f.dir == LEFT) ? +12 : +0;
    int dy = +43;
    place_actor (s, &s->p, dx, dy, "SKELETON", "DIE", 2);

    s->i = 2;
  }

  if (s->i-- == 0) {
    guard_normal (s);
    return;
  }

  s->fo.b = actor_bitmap (s, "SKELETON", "DIE", s->i);
  s->fo.dx = -actor_dx (s, "SKELETON", "DIE", s->i + 1);
  s->fo.dy = -actor_dy (s, "SKELETON", "DIE", s->i + 1);

  next_frame (&s->f, &s->f, &s->fo);
}

void
guard_die_spiked (struct actor *g)
{
  if (fg (&g->p) != SPIKES_FLOOR) {
    guard_die_properly (g);
    return;
  }

  g->oaction = g->action;
  g->oi = g->i;
  g->action = guard_die_spiked;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  assert (fg (&g->p) == SPIKES_FLOOR);
  struct spikes_floor *s = spikes_floor_at_pos (&g->p);

  if (s->i != 4 || s->state != 5 || ! s->inactive) {
    s->i = 4;
    s->state = 5;
    s->inactive = true;
    register_changed_pos (&g->p);
  }

  if (g->oaction != guard_die_spiked) {
    g->splash = true;
    g->death_reason = SPIKES_DEATH;

    if (g->type == SKELETON)
      play_audio (&skeleton_audio, NULL, g->id);
    else play_audio (&spiked_audio, NULL, g->id);

    if (! g->glory_sample) {
      struct actor *k = get_actor_by_id (g->enemy_id);
      play_audio (&glory_audio, NULL, g->id);
      kid_haptic (k, KID_HAPTIC_SUCCESS);
      g->glory_sample = true;
    }
  }

  g->current_hp = 0;

  int dy;
  if (g->type == SKELETON) dy = +45;
  else dy = (g->f.dir == LEFT) ? +32 : +31;

  int dx = (g->f.dir == LEFT) ? +8 : +9;
  place_actor (g, &g->p, dx, dy, NULL, "DIE", 6);

  g->xf.b = NULL;
}

void
guard_die_chomped (struct actor *g)
{
  if (fg (&g->p) != CHOMPER) {
    guard_die_properly (g);
    return;
  }

  g->oaction = g->action;
  g->oi = g->i;
  g->action = guard_die_chomped;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  int dx, dy;

  if (g->type == SHADOW) {
    dx = (g->f.dir == LEFT) ? -8 : -7;
    dy = +47;
  } else {
    dx = (g->f.dir == LEFT) ? -10 : -13;
    dy = (g->type == SKELETON) ? +45 : +43;
  }

  place_actor (g, &g->p, dx, dy, NULL, "DIE", 7);

  if (g->oaction != guard_die_chomped
      && ! g->glory_sample) {
    struct actor *k = get_actor_by_id (g->enemy_id);
    play_audio (&glory_audio, NULL, g->id);
    kid_haptic (k, KID_HAPTIC_SUCCESS);
    g->glory_sample = true;
  }

  g->current_hp = 0;
  g->xf.b = NULL;
}

void
guard_die_suddenly (struct actor *g)
{
  enum tile_fg f = fg (&g->p);
  if ( f == SPIKES_FLOOR || f == CHOMPER) {
    guard_die_properly (g);
    return;
  }

  g->oaction = g->action;
  g->oi = g->i;
  g->action = guard_die_suddenly;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  int dx = (g->f.dir == LEFT) ? +9 : +4;
  int dy = (g->type == SKELETON) ? +44 : +47;
  place_actor (g, &g->p, dx, dy, NULL, "DIE", 5);

  if (g->oaction != guard_die_suddenly
      && ! g->glory_sample) {
    struct actor *k = get_actor_by_id (g->enemy_id);
    play_audio (&glory_audio, NULL, g->id);
    kid_haptic (k, KID_HAPTIC_SUCCESS);
    g->glory_sample = true;
  }

  g->current_hp = 0;
  g->xf.b = NULL;

  g->hit_by_loose_floor = false;

  /* fall */
  if (is_falling (&g->f, _m, +0, +0)) {
    guard_fall (g);
    return;
  }

  /* depressible floors */
  update_depressible_floor (g, -12, -28);
}

void
guard_die (struct actor *g)
{
  g->oaction = g->action;
  g->oi = g->i;
  g->action = guard_die;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct actor *g)
{
  if (g->oaction != guard_die) {
    g->i = -1, g->j = 0;
    if (g->type == SKELETON)
      play_audio (&skeleton_audio, NULL, g->id);

    if (! g->glory_sample) {
      struct actor *k = get_actor_by_id (g->enemy_id);
      play_audio (&glory_audio, NULL, g->id);
      kid_haptic (k, KID_HAPTIC_SUCCESS);
      g->glory_sample = true;
    }

    g->xf.b = NULL;
  }

  g->current_hp = 0;

  g->i = g->i < 5 ? g->i + 1 : 5;

  select_actor_frame (g, NULL, "DIE", g->i);

  if (g->j >= 1) g->fo.dx = g->fo.dy = 0;
  if (g->i == 5) g->j = 1;
  if (g->j == 1) g->j++;

  return true;
}

static bool
physics_in (struct actor *g)
{
  /* collision */
  uncollide (&g->f, &g->fo, _bb, +0, +0, &g->fo, NULL);

  /* fall */
  if (is_falling (&g->f, _m, +0, +0)) {
    guard_fall (g);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *g)
{
  /* depressible floors */
  if (g->i == 0) update_depressible_floor (g, -6, -12);
  else if (g->i == 1) update_depressible_floor (g, -5, -12);
  else if (g->i == 2) update_depressible_floor (g, -4, -13);
  else if (g->i == 3) update_depressible_floor (g, -2, -18);
  else if (g->i == 4) update_depressible_floor (g, 0, -27);
  else if (g->i == 5) update_depressible_floor (g, -12, -28);
  else keep_depressible_floor (g);
}

void
guard_die_properly (struct actor *g)
{
  switch (fg (&g->p)) {
  case SPIKES_FLOOR: guard_die_spiked (g); break;
  case CHOMPER: guard_die_chomped (g); break;
  default: guard_die_suddenly (g); break;
  }
}

bool
is_guard_dead (struct actor *g)
{
  return g->action == guard_die
    || g->action == guard_die_suddenly
    || g->action == guard_die_spiked
    || g->action == guard_die_chomped;
}

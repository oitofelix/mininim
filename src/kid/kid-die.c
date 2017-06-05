/*
  kid-die.c -- kid die module;

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
kid_resurrect (struct actor *k)
{
  k->splash = false;
  k->sword_immune = 16;
  k->invisible = false;
  if (k->current_hp > 0) return;
  struct pos pm;
  survey (_m, pos, &k->f, NULL, &pm, NULL);
  k->current_hp = k->total_hp;
  k->death_reason = NO_DEATH;
  k->death_timer = 0;
  k->action = kid_normal;

  int dx = k->f.dir == LEFT ? +16 : +16;
  int dy = +15;
  place_actor (k, &pm, dx, dy, "KID", "NORMAL", 0);

  if (fg (&k->p) == SPIKES_FLOOR)
    spikes_floor_at_pos (&k->p)->inactive = false;
  stop_audio_instance (&death_audio, NULL, k->id);
  stop_audio_instance (&fight_death_audio, NULL, k->id);
  stop_audio_instance (&success_suspense_audio, NULL, k->id);
}

void
kid_die_spiked (struct actor *k)
{
  if (fg (&k->p) != SPIKES_FLOOR) {
    kid_die_properly (k);
    return;
  }

  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_die_spiked;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  assert (fg (&k->p) == SPIKES_FLOOR);
  struct spikes_floor *s = spikes_floor_at_pos (&k->p);

  if (s->i != 4 || s->state != 5 || ! s->inactive) {
    s->i = 4;
    s->state = 5;
    s->inactive = true;
    register_changed_pos (&k->p);
  }

  if (k->oaction != kid_die_spiked) {
    k->current_hp = 0;
    k->splash = true;
    k->death_reason = SPIKES_DEATH;

    if (k->id == current_kid_id) {
      mr.flicker = 2;
      mr.color = get_flicker_blood_color ();
      kid_haptic (k, KID_HAPTIC_DEATH);
    }
    play_audio (&spiked_audio, NULL, k->id);
  }

  int dx = (k->f.dir == LEFT) ? +8 : +9;
  int dy = (k->f.dir == LEFT) ? +32 : +31;
  place_actor (k, &k->p, dx, dy, "KID", "DIE", 6);

  if (k->oaction != kid_die_spiked) kill_kid_shadows (k);

  k->xf.b = NULL;
}

void
kid_die_chomped (struct actor *k)
{
  if (fg (&k->p) != CHOMPER) {
    kid_die_properly (k);
    return;
  }

  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_die_chomped;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (k->oaction != kid_die_chomped) {
    k->current_hp = 0;
    k->splash = true;
    k->death_reason = CHOMPER_DEATH;

    if (k->id == current_kid_id) {
      mr.flicker = 2;
      mr.color = get_flicker_blood_color ();
      kid_haptic (k, KID_HAPTIC_DEATH);
    }
    play_audio (&chomped_audio, NULL, k->id);
  }

  struct rect r; actor_rect (&r, k, "KID", "DIE", "CHOMPED");
  place_actor (k, &k->p, r.c.x, r.c.y, "KID", "DIE", 7);

  if (k->oaction != kid_die_chomped) kill_kid_shadows (k);

  k->xf.b = NULL;
}

void
kid_die_suddenly (struct actor *k)
{
  enum tile_fg f = fg (&k->p);
  if (f == SPIKES_FLOOR || f == CHOMPER) {
    kid_die_properly (k);
    return;
  }

  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_die_suddenly;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  k->current_hp = 0;

  k->f.b = actor_bitmap (k, "KID", "DIE", 5);
  place_on_the_ground_alternative (&k->f, &k->f.c, PLACE_HEIGHT);

  if (k->oaction != kid_die_suddenly) {
    move_frame (&k->f, _tf, +0, +4, +0);

    survey (_m, pos, &k->f, NULL, &k->p, NULL);

    kill_kid_shadows (k);

    kid_haptic (k, KID_HAPTIC_DEATH);
  }

  k->xf.b = NULL;

  k->hit_by_loose_floor = false;

  /* fall */
  if (is_falling (&k->f, _m, +0, +0)) {
    kid_fall (k);
    return;
  }

  /* depressible floors */
  update_depressible_floor (k, -12, -28);
}

void
kid_die (struct actor *k)
{
  if (k->action == kid_fall) {
    k->current_hp = 0;
    return;
  }

  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_die;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_die) {
    k->i = -1, k->j = 0;
    k->xf.b = NULL;
  }

  k->current_hp = 0;

  k->i = k->i < 5 ? k->i + 1 : 5;

  select_actor_frame (k, "KID", "DIE", k->i);

  if (k->j >= 1) k->fo.dx = k->fo.dy = 0;
  if (k->i == 5) k->j = 1;
  if (k->j == 1) {
    k->j++;
    kill_kid_shadows (k);
  }

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* place on ground */
  if (k->i == 5) place_on_the_ground_alternative
                   (&k->f, &k->f.c, PLACE_HEIGHT);

  /* collision */
  uncollide (&k->f, &k->fo, _bb, +0, +0, &k->fo, NULL);

  /* fall */
  if (is_falling (&k->f, _m, +0, +0)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  /* haptic */
  if (k->i == 0) kid_haptic (k, KID_HAPTIC_DEATH);

  /* depressible floors */
  if (k->i == 0) update_depressible_floor (k, -5, -11);
  else if (k->i == 1) update_depressible_floor (k, -4, -11);
  else if (k->i == 2) update_depressible_floor (k, -3, -14);
  else if (k->i == 3) update_depressible_floor (k, -4, -15);
  else if (k->i == 4) update_depressible_floor (k, -3, -21);
  else if (k->i == 5) update_depressible_floor (k, -12, -28);
  else keep_depressible_floor (k);
}

bool
is_kid_dead (struct actor *k)
{
  return k->action == kid_die
    || k->action == kid_die_suddenly
    || k->action == kid_die_spiked
    || k->action == kid_die_chomped;
}

void
kill_kid_shadows (struct actor *k)
{
  struct pos pmt;

  int i;
  for (i = 0; i < actor_nmemb; i++) {
    struct actor *ks = &actor[i];

    if (ks->type == KID
        && ks->shadow_of == k->id
        && ks->controllable
        && ! is_kid_dead (ks)) {
      survey (_mt, pos, &ks->f, NULL, &pmt, NULL);
      ks->p = pmt;
      kid_die (ks);
    }
  }
}

void
kid_die_properly (struct actor *k)
{
  switch (fg (&k->p)) {
  case SPIKES_FLOOR: kid_die_spiked (k); break;
  case CHOMPER: kid_die_chomped (k); break;
  default: kid_die_suddenly (k); break;
  }
}

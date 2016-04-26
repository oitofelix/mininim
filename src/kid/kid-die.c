/*
  kid-die.c -- kid die module;

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

struct frameset kid_die_frameset[KID_DIE_FRAMESET_NMEMB];

static void init_kid_die_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_die_00, *kid_die_01, *kid_die_02, *kid_die_03,
  *kid_die_04, *kid_die_05, *kid_die_spiked_00, *kid_die_chopped_00;

static void
init_kid_die_frameset (void)
{
  struct frameset frameset[KID_DIE_FRAMESET_NMEMB] =
    {{kid_die_00,-1,0},{kid_die_01,+0,0},{kid_die_02,-3,+1},
     {kid_die_03,-2,+2},{kid_die_04,+0,+3},{kid_die_05,+2,+1}};

  memcpy (&kid_die_frameset, &frameset,
          KID_DIE_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_die (void)
{
  /* bitmaps */
  kid_die_00 = load_bitmap (KID_DIE_00);
  kid_die_01 = load_bitmap (KID_DIE_01);
  kid_die_02 = load_bitmap (KID_DIE_02);
  kid_die_03 = load_bitmap (KID_DIE_03);
  kid_die_04 = load_bitmap (KID_DIE_04);
  kid_die_05 = load_bitmap (KID_DIE_05);
  kid_die_spiked_00 = load_bitmap (KID_DIE_SPIKED_00);
  kid_die_chopped_00 = load_bitmap (KID_DIE_CHOPPED_00);

  /* frameset */
  init_kid_die_frameset ();
}

void
unload_kid_die (void)
{
  destroy_bitmap (kid_die_00);
  destroy_bitmap (kid_die_01);
  destroy_bitmap (kid_die_02);
  destroy_bitmap (kid_die_03);
  destroy_bitmap (kid_die_04);
  destroy_bitmap (kid_die_05);
  destroy_bitmap (kid_die_spiked_00);
  destroy_bitmap (kid_die_chopped_00);
}

void
kid_resurrect (struct anim *k)
{
  struct coord nc; struct pos np, pm;
  survey (_m, pos, &k->f, &nc, &pm, &np);
  k->current_lives = k->total_lives;
  k->death_reason = NO_DEATH;
  k->action = kid_normal;
  place_frame (&k->f, &k->f, kid_normal_00,
               &pm, k->f.dir == LEFT ? +16 : +16, +15);
  reset_murder_spikes_floor (k->id);
  stop_sample (k->sample, death_sample);
  stop_sample (k->sample, fight_death_sample);
  stop_sample (k->sample, success_suspense_sample);
}

void
kid_die_spiked (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_die_spiked;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (k->oaction != kid_die_spiked) {
    k->current_lives = 0;
    k->splash = true;
    k->death_reason = SPIKES_DEATH;

    struct spikes_floor *s = spikes_floor_at_pos (&k->p);
    s->i = 4;
    s->state = 5;
    s->inactive = true;
    s->murdered_anim = k->id;

    if (k->id == current_kid_id) {
      mr.flicker = 2;
      mr.color = get_flicker_blood_color ();
    }
    play_sample (spiked_sample, k->f.c.room);
  }

  if (k->oaction != kid_die_spiked) {
    place_frame (&k->f, &k->f, kid_die_spiked_00,
                 &k->p, (k->f.dir == LEFT)
                 ? +8 : +9, (k->f.dir == LEFT) ? +32 : +31);
    kill_kid_shadows (k);
  }

  k->xf.b = NULL;
}

void
kid_die_chopped (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_die_chopped;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  k->current_lives = 0;

  if (k->oaction != kid_die_chopped) {
    place_frame (&k->f, &k->f, kid_die_chopped_00,
                 &k->p, (k->f.dir == LEFT)
                 ? -8 : -7, +47);
    kill_kid_shadows (k);
  }

  k->xf.b = NULL;
}

void
kid_die_suddenly (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_die_suddenly;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  k->current_lives = 0;

  if (k->oaction != kid_die_suddenly) {
    place_frame (&k->f, &k->f, kid_die_05,
                 &k->p, (k->f.dir == LEFT)
                 ? +9 : +4, +47);

    kill_kid_shadows (k);
  }

  k->xf.b = NULL;

  k->hit_by_loose_floor = false;

  /* collision */
  uncollide (&k->f, &k->fo, &k->fo, +0, false, &k->ci);
  uncollide (&k->f, &k->fo, &k->fo, +0, true, &k->ci);

  /* fall */
  struct coord nc;
  struct pos np, pm;
  survey (_m, pos, &k->f, &nc, &pm, &np);
  if (is_strictly_traversable (&pm)) {
    kid_fall (k);
    return;
  }

  /* depressible floors */
  update_depressible_floor (k, -12, -28);
}

void
kid_die (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_die;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_die) {
    place_frame (&k->f, &k->f, kid_die_frameset[0].frame,
                 &k->p, (k->f.dir == LEFT)
                 ? +13 : +21, +18);
    k->i = -1, k->j = 0;
    k->xf.b = NULL;
  }

  k->current_lives = 0;

  k->i = k->i < 5 ? k->i + 1 : 5;

  select_frame (k, kid_die_frameset, k->i);

  if (k->j >= 1) k->fo.dx = k->fo.dy = 0;
  if (k->i == 5) k->j = 1;
  if (k->j == 1) {
    k->j++;
    kill_kid_shadows (k);
  }

  return true;
}

static bool
physics_in (struct anim *k)
{
  /* collision */
  uncollide (&k->f, &k->fo, &k->fo, +0, false, &k->ci);
  uncollide (&k->f, &k->fo, &k->fo, +0, true, &k->ci);

  /* fall */
  struct coord nc;
  struct pos np, pm;
  survey (_m, pos, &k->f, &nc, &pm, &np);
  if (is_strictly_traversable (&pm)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
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
is_kid_dead (struct frame *f)
{
  int i;
  for (i = 0; i < KID_DIE_FRAMESET_NMEMB; i++)
    if (f->b == kid_die_frameset[i].frame) return true;

  if (f->b == kid_die_spiked_00) return true;
  if (f->b == kid_die_chopped_00) return true;

  return false;
}

bool
is_kid_chopped (struct frame *f)
{
  if (f->b == kid_die_chopped_00) return true;

  return false;
}

void
kill_kid_shadows (struct anim *k)
{
  struct coord nc; struct pos np, pmt;

  int i;
  for (i = 0; i < anima_nmemb; i++) {
    struct anim *ks = &anima[i];

    if (ks->type == KID
        && ks->shadow_of == k->id
        && ks->controllable
        && ! is_kid_dead (&ks->f)) {
      survey (_mt, pos, &ks->f, &nc, &pmt, &np);
      ks->p = pmt;
      kid_die (ks);
    }
  }
}

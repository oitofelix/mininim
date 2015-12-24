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

#include "prince.h"
#include "kernel/video.h"
#include "kernel/keyboard.h"
#include "engine/anim.h"
#include "engine/physics.h"
#include "engine/door.h"
#include "engine/potion.h"
#include "engine/sword.h"
#include "engine/loose-floor.h"
#include "engine/spikes-floor.h"
#include "kid.h"

struct frameset kid_die_frameset[KID_DIE_FRAMESET_NMEMB];

static void init_kid_die_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_die_01, *kid_die_02, *kid_die_03, *kid_die_04,
  *kid_die_05, *kid_die_06, *kid_die_spiked_00, *kid_die_chopped_00;

static void
init_kid_die_frameset (void)
{
  struct frameset frameset[KID_DIE_FRAMESET_NMEMB] =
    {{kid_die_01,-1,0},{kid_die_02,+0,0},{kid_die_03,-3,+1},
     {kid_die_04,-2,+2},{kid_die_05,+0,+3},{kid_die_06,+2,+1}};

  memcpy (&kid_die_frameset, &frameset,
          KID_DIE_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_die (void)
{
  /* bitmaps */
  kid_die_01 = load_bitmap (KID_DIE_01);
  kid_die_02 = load_bitmap (KID_DIE_02);
  kid_die_03 = load_bitmap (KID_DIE_03);
  kid_die_04 = load_bitmap (KID_DIE_04);
  kid_die_05 = load_bitmap (KID_DIE_05);
  kid_die_06 = load_bitmap (KID_DIE_06);
  kid_die_spiked_00 = load_bitmap (KID_DIE_SPIKED_00);
  kid_die_chopped_00 = load_bitmap (KID_DIE_CHOPPED_00);

  /* frameset */
  init_kid_die_frameset ();
}

void
unload_kid_die (void)
{
  al_destroy_bitmap (kid_die_01);
  al_destroy_bitmap (kid_die_02);
  al_destroy_bitmap (kid_die_03);
  al_destroy_bitmap (kid_die_04);
  al_destroy_bitmap (kid_die_05);
  al_destroy_bitmap (kid_die_06);
  al_destroy_bitmap (kid_die_spiked_00);
  al_destroy_bitmap (kid_die_chopped_00);
}

void
kid_resurrect (struct anim *kid)
{
  struct coord nc; struct pos np, pm;
  survey (_m, pos, &kid->f, &nc, &pm, &np);
  kid->current_lives = kid->total_lives;
  kid->action = kid_normal;
  place_frame (&kid->f, &kid->f, kid_normal_00,
               &pm, kid->f.dir == LEFT ? +16 : +16, +15);
  reset_murder_spikes_floor (kid->id);
}

void
kid_die_spiked (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_die_spiked;
  kid->f.flip = (kid->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  kid->current_lives = 0;

  if (kid->oaction != kid_die_spiked) {
    place_frame (&kid->f, &kid->f, kid_die_spiked_00,
                 &kid->p, (kid->f.dir == LEFT)
                 ? +8 : +9, (kid->f.dir == LEFT) ? +32 : +31);
    kill_kid_shadows (kid);
  }
}

void
kid_die_chopped (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_die_chopped;
  kid->f.flip = (kid->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  kid->current_lives = 0;

  if (kid->oaction != kid_die_chopped) {
    place_frame (&kid->f, &kid->f, kid_die_chopped_00,
                 &kid->p, (kid->f.dir == LEFT)
                 ? -8 : -7, +47);
    kill_kid_shadows (kid);
  }
}

void
kid_die_suddenly (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_die_suddenly;
  kid->f.flip = (kid->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  kid->current_lives = 0;

  if (kid->oaction != kid_die_suddenly) {
    place_frame (&kid->f, &kid->f, kid_die_06,
                 &kid->p, (kid->f.dir == LEFT)
                 ? +9 : +4, +47);

    kill_kid_shadows (kid);
  }

  kid->hit_by_loose_floor = false;
}

void
kid_die (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_die;
  kid->f.flip = (kid->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame (&kid->f, &kid->f, &kid->fo);
  physics_out (kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_die) {
    place_frame (&kid->f, &kid->f, kid_die_frameset[0].frame,
                 &kid->p, (kid->f.dir == LEFT)
                 ? +13 : +21, +18);
    kid->i = -1, kid->j = 0;
  }

  kid->current_lives = 0;

  kid->i = kid->i < 5 ? kid->i + 1 : 5;

  select_frame (kid, kid_die_frameset, kid->i);

  if (kid->j >= 1) kid->fo.dx = kid->fo.dy = 0;
  if (kid->i == 5) kid->j = 1;
  if (kid->j == 1) {
    kid->j++;
    kill_kid_shadows (kid);
  }

  return true;
}

static bool
physics_in (struct anim *kid)
{
  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  if (kid->i == 0) update_depressible_floor (kid, -5, -11);
  else if (kid->i == 1) update_depressible_floor (kid, -4, -11);
  else if (kid->i == 2) update_depressible_floor (kid, -3, -14);
  else if (kid->i == 3) update_depressible_floor (kid, -4, -15);
  else if (kid->i == 4) update_depressible_floor (kid, -3, -21);
  else if (kid->i == 5) update_depressible_floor (kid, -1, -33);
  else keep_depressible_floor (kid);
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

void
kill_kid_shadows (struct anim *k)
{
  struct coord nc; struct pos np, pmt;

  int i;
  for (i = 0; i < kid_nmemb; i++) {
    struct anim *ks = &kid[i];

    if (ks->shadow_of == k->id
        && ! is_kid_dead (&ks->f)) {
      survey (_mt, pos, &ks->f, &nc, &pmt, &np);
      ks->p = pmt;
      kid_die (ks);
    }
  }
}

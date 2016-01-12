/*
  guard-die.c -- guard die module;

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
#include "engine/spikes-floor.h"
#include "engine/kid/kid.h"
#include "guard.h"

struct frameset guard_die_frameset[GUARD_DIE_FRAMESET_NMEMB];

static void init_guard_die_frameset (void);
static bool flow (struct anim *g);
static bool physics_in (struct anim *g);
static void physics_out (struct anim *g);

ALLEGRO_BITMAP *guard_die_01, *guard_die_02, *guard_die_03, *guard_die_04,
  *guard_die_05, *guard_die_06, *guard_die_spiked_00, *guard_die_chopped_00;

static void
init_guard_die_frameset (void)
{
  struct frameset frameset[GUARD_DIE_FRAMESET_NMEMB] =
    {{guard_die_01,-1,0},{guard_die_02,+0,0},{guard_die_03,-3,+1},
     {guard_die_04,-2,+2},{guard_die_05,+0,+0},{guard_die_06,-2,+0}};

  memcpy (&guard_die_frameset, &frameset,
          GUARD_DIE_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_guard_die (void)
{
  /* bitmaps */
  guard_die_01 = load_bitmap (GUARD_DIE_01);
  guard_die_02 = load_bitmap (GUARD_DIE_02);
  guard_die_03 = load_bitmap (GUARD_DIE_03);
  guard_die_04 = load_bitmap (GUARD_DIE_04);
  guard_die_05 = load_bitmap (GUARD_DIE_05);
  guard_die_06 = load_bitmap (GUARD_DIE_06);
  guard_die_spiked_00 = load_bitmap (GUARD_DIE_SPIKED_00);
  guard_die_chopped_00 = load_bitmap (GUARD_DIE_CHOPPED_00);

  /* frameset */
  init_guard_die_frameset ();
}

void
unload_guard_die (void)
{
  al_destroy_bitmap (guard_die_01);
  al_destroy_bitmap (guard_die_02);
  al_destroy_bitmap (guard_die_03);
  al_destroy_bitmap (guard_die_04);
  al_destroy_bitmap (guard_die_05);
  al_destroy_bitmap (guard_die_06);
  al_destroy_bitmap (guard_die_spiked_00);
  al_destroy_bitmap (guard_die_chopped_00);
}

void
guard_die_spiked (struct anim *g)
{
  g->oaction = g->action;
  g->action = guard_die_spiked;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (g->oaction != guard_die_spiked) {
    g->current_lives = 0;
    g->splash = true;

    struct spikes_floor *s = spikes_floor_at_pos (&g->p);
    s->i = 4;
    s->state = 5;
    s->inactive = true;
    s->murdered_anim = g->id;

    play_sample (spiked_sample, g->f.c.room);
  }

  if (g->oaction != guard_die_spiked)
    place_frame (&g->f, &g->f, guard_die_spiked_00,
                 &g->p, (g->f.dir == LEFT)
                 ? +8 : +9, (g->f.dir == LEFT) ? +32 : +31);
}

void
guard_die_chopped (struct anim *g)
{
  g->oaction = g->action;
  g->action = guard_die_chopped;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  g->current_lives = 0;

  if (g->oaction != guard_die_chopped)
    place_frame (&g->f, &g->f, guard_die_chopped_00,
                 &g->p, (g->f.dir == LEFT)
                 ? -8 : -7, +47);
}

void
guard_die_suddenly (struct anim *g)
{
  g->oaction = g->action;
  g->action = guard_die_suddenly;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  g->current_lives = 0;

  if (g->oaction != guard_die_suddenly)
    place_frame (&g->f, &g->f, guard_die_06,
                 &g->p, (g->f.dir == LEFT)
                 ? +9 : +4, +47);

  g->hit_by_loose_floor = false;

  /* fall */
  struct coord nc;
  struct pos np, pm;
  survey (_m, pos, &g->f, &nc, &pm, &np);
  if (is_strictly_traversable (&pm)) {
    /* guard_fall (g); */
    return;
  }

  /* depressible floors */
  update_depressible_floor (g, -3, -40);
}

void
guard_die (struct anim *g)
{
  g->oaction = g->action;
  g->action = guard_die;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct anim *g)
{
  if (g->oaction != guard_die) {
    place_frame (&g->f, &g->f, guard_die_frameset[0].frame,
                 &g->p, (g->f.dir == LEFT)
                 ? +13 : +21, +17);
    g->i = -1, g->j = 0;
  }

  g->current_lives = 0;

  g->i = g->i < 5 ? g->i + 1 : 5;

  select_frame (g, guard_die_frameset, g->i);

  if (g->j >= 1) g->fo.dx = g->fo.dy = 0;
  if (g->i == 5) g->j = 1;
  if (g->j == 1) g->j++;

  return true;
}

static bool
physics_in (struct anim *g)
{
  /* fall */
  struct coord nc;
  struct pos np, pm;
  survey (_m, pos, &g->f, &nc, &pm, &np);
  if (is_strictly_traversable (&pm)) {
    /* guard_fall (g); */
    return false;
  }

  return true;
}

static void
physics_out (struct anim *g)
{
  /* depressible floors */
  if (g->i == 0) update_depressible_floor (g, -6, -12);
  else if (g->i == 1) update_depressible_floor (g, -5, -12);
  else if (g->i == 2) update_depressible_floor (g, -4, -13);
  else if (g->i == 3) update_depressible_floor (g, -2, -18);
  else if (g->i == 4) update_depressible_floor (g, 0, -27);
  else if (g->i == 5) update_depressible_floor (g, -3, -40);
  else keep_depressible_floor (g);
}

bool
is_guard_dead (struct frame *f)
{
  int i;
  for (i = 0; i < GUARD_DIE_FRAMESET_NMEMB; i++)
    if (f->b == guard_die_frameset[i].frame) return true;

  if (f->b == guard_die_spiked_00) return true;
  if (f->b == guard_die_chopped_00) return true;

  return false;
}

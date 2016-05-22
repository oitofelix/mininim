/*
  kid-walkf.c -- guard walk forward module;

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

struct frameset guard_walkf_frameset[GUARD_WALKF_FRAMESET_NMEMB];
struct frameset fat_guard_walkf_frameset[GUARD_WALKF_FRAMESET_NMEMB];
struct frameset vizier_walkf_frameset[GUARD_WALKF_FRAMESET_NMEMB];
struct frameset skeleton_walkf_frameset[GUARD_WALKF_FRAMESET_NMEMB];
struct frameset shadow_walkf_frameset[GUARD_WALKF_FRAMESET_NMEMB];

static void init_guard_walkf_frameset (void);
static void init_fat_guard_walkf_frameset (void);
static void init_vizier_walkf_frameset (void);
static void init_skeleton_walkf_frameset (void);
static void init_shadow_walkf_frameset (void);
static bool flow (struct anim *g);
static bool physics_in (struct anim *g);
static void physics_out (struct anim *g);

/* guard */
ALLEGRO_BITMAP *guard_walkf_00, *guard_walkf_01, *guard_walkf_02;

/* fat guard */
ALLEGRO_BITMAP *fat_guard_walkf_00, *fat_guard_walkf_01, *fat_guard_walkf_02;

/* vizier */
ALLEGRO_BITMAP *vizier_walkf_00, *vizier_walkf_01, *vizier_walkf_02;

/* skeleton */
ALLEGRO_BITMAP *skeleton_walkf_00, *skeleton_walkf_01, *skeleton_walkf_02;

/* shadow */
ALLEGRO_BITMAP *shadow_walkf_00, *shadow_walkf_01, *shadow_walkf_02;

static void
init_guard_walkf_frameset (void)
{
  struct frameset frameset[GUARD_WALKF_FRAMESET_NMEMB] =
    {{guard_walkf_00,-7,0},{guard_walkf_01,-8,0},
     {guard_walkf_02,-1,0}};

  memcpy (&guard_walkf_frameset, &frameset,
          GUARD_WALKF_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_fat_guard_walkf_frameset (void)
{
  struct frameset frameset[GUARD_WALKF_FRAMESET_NMEMB] =
    {{fat_guard_walkf_00,-7,0},{fat_guard_walkf_01,-8,0},
     {fat_guard_walkf_02,-1,0}};

  memcpy (&fat_guard_walkf_frameset, &frameset,
          GUARD_WALKF_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_vizier_walkf_frameset (void)
{
  struct frameset frameset[GUARD_WALKF_FRAMESET_NMEMB] =
    {{vizier_walkf_00,-7,0},{vizier_walkf_01,-8,0},
     {vizier_walkf_02,-1,0}};

  memcpy (&vizier_walkf_frameset, &frameset,
          GUARD_WALKF_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_skeleton_walkf_frameset (void)
{
  struct frameset frameset[GUARD_WALKF_FRAMESET_NMEMB] =
    {{skeleton_walkf_00,-7,0},{skeleton_walkf_01,-8,0},
     {skeleton_walkf_02,-1,0}};

  memcpy (&skeleton_walkf_frameset, &frameset,
          GUARD_WALKF_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_shadow_walkf_frameset (void)
{
  struct frameset frameset[GUARD_WALKF_FRAMESET_NMEMB] =
    {{shadow_walkf_00,-7,0},{shadow_walkf_01,-8,0},
     {shadow_walkf_02,-1,0}};

  memcpy (&shadow_walkf_frameset, &frameset,
          GUARD_WALKF_FRAMESET_NMEMB * sizeof (struct frameset));
}

struct frameset *
get_guard_walkf_frameset (enum anim_type t)
{
  switch (t) {
  case GUARD: default: return guard_walkf_frameset;
  case FAT_GUARD: return fat_guard_walkf_frameset;
  case VIZIER: return vizier_walkf_frameset;
  case SKELETON: return skeleton_walkf_frameset;
  case SHADOW: return shadow_walkf_frameset;
  }
}

void
load_guard_walkf (void)
{
  /* guard */
  guard_walkf_00 = load_bitmap (GUARD_WALKF_00);
  guard_walkf_01 = load_bitmap (GUARD_WALKF_01);
  guard_walkf_02 = load_bitmap (GUARD_WALKF_02);

  /* fat guard */
  fat_guard_walkf_00 = load_bitmap (FAT_GUARD_WALKF_00);
  fat_guard_walkf_01 = load_bitmap (FAT_GUARD_WALKF_01);
  fat_guard_walkf_02 = load_bitmap (FAT_GUARD_WALKF_02);

  /* vizier */
  vizier_walkf_00 = load_bitmap (VIZIER_WALKF_00);
  vizier_walkf_01 = load_bitmap (VIZIER_WALKF_01);
  vizier_walkf_02 = load_bitmap (VIZIER_WALKF_02);

  /* skeleton */
  skeleton_walkf_00 = load_bitmap (SKELETON_WALKF_00);
  skeleton_walkf_01 = load_bitmap (SKELETON_WALKF_01);
  skeleton_walkf_02 = load_bitmap (SKELETON_WALKF_02);

  /* shadow */
  shadow_walkf_00 = load_bitmap (SHADOW_WALKF_00);
  shadow_walkf_01 = load_bitmap (SHADOW_WALKF_01);
  shadow_walkf_02 = load_bitmap (SHADOW_WALKF_02);

  /* frameset */
  init_guard_walkf_frameset ();
  init_fat_guard_walkf_frameset ();
  init_vizier_walkf_frameset ();
  init_skeleton_walkf_frameset ();
  init_shadow_walkf_frameset ();
}

void
unload_guard_walkf (void)
{
  /* guard */
  destroy_bitmap (guard_walkf_00);
  destroy_bitmap (guard_walkf_01);
  destroy_bitmap (guard_walkf_02);

  /* fat guard */
  destroy_bitmap (fat_guard_walkf_00);
  destroy_bitmap (fat_guard_walkf_01);
  destroy_bitmap (fat_guard_walkf_02);

  /* vizier */
  destroy_bitmap (vizier_walkf_00);
  destroy_bitmap (vizier_walkf_01);
  destroy_bitmap (vizier_walkf_02);

  /* skeleton */
  destroy_bitmap (skeleton_walkf_00);
  destroy_bitmap (skeleton_walkf_01);
  destroy_bitmap (skeleton_walkf_02);

  /* shadow */
  destroy_bitmap (shadow_walkf_00);
  destroy_bitmap (shadow_walkf_01);
  destroy_bitmap (shadow_walkf_02);
}

void
guard_walkf (struct anim *g)
{
  g->oaction = g->action;
  g->action = guard_walkf;
  g->f.flip = (g->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct anim *g)
{
  if (g->oaction != guard_walkf) g->i = -1;

  if (g->i == 2) {
    guard_vigilant (g);
    return false;
  }

  struct frameset *frameset = get_guard_walkf_frameset (g->type);
  select_frame (g, frameset, g->i + 1);

  if (g->i == 0) g->j = 8;
  if (g->i >= 1) g->j = 4;

  select_xframe (&g->xf, sword_frameset, g->j);

  if (g->i == 1) g->xf.dx = -6, g->xf.dy = +8;
  if (g->i == 2) g->xf.dx = -9, g->xf.dy = +11;

  if (g->type == SKELETON) g->xf.dy += -3;
  if (g->type == SHADOW) g->xf.dy += -2;

  return true;
}

static bool
physics_in (struct anim *g)
{
  struct coord nc; struct pos np, pbf, pmbo, pbb;

  /* collision */
  uncollide (&g->f, &g->fo, &g->fo, +0, false, &g->ci);

  /* fall */
  survey (_bf, pos, &g->f, &nc, &pbf, &np);
  survey (_mbo, pos, &g->f, &nc, &pmbo, &np);
  survey (_bb, pos, &g->f, &nc, &pbb, &np);
  if (is_strictly_traversable (&pbf)
      || is_strictly_traversable (&pmbo)
      /* || is_strictly_traversable (&pbb) */
      ) {
    g->xf.b = NULL;
    guard_fall (g);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *g)
{
  /* depressible floors */
  if (g->i == 2) update_depressible_floor (g, -3, -38);
  else keep_depressible_floor (g);
}

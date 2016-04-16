/*
  guard-walkb.c -- guard walk backward module;

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

struct frameset guard_walkb_frameset[GUARD_WALKB_FRAMESET_NMEMB];
struct frameset fat_guard_walkb_frameset[GUARD_WALKB_FRAMESET_NMEMB];
struct frameset vizier_walkb_frameset[GUARD_WALKB_FRAMESET_NMEMB];
struct frameset skeleton_walkb_frameset[GUARD_WALKB_FRAMESET_NMEMB];
struct frameset shadow_walkb_frameset[GUARD_WALKB_FRAMESET_NMEMB];

static void init_guard_walkb_frameset (void);
static void init_fat_guard_walkb_frameset (void);
static void init_vizier_walkb_frameset (void);
static void init_skeleton_walkb_frameset (void);
static void init_shadow_walkb_frameset (void);
static bool flow (struct anim *g);
static bool physics_in (struct anim *g);
static void physics_out (struct anim *g);

/* guard */
ALLEGRO_BITMAP *guard_walkb_00, *guard_walkb_01;

/* fat guard */
ALLEGRO_BITMAP *fat_guard_walkb_00, *fat_guard_walkb_01;

/* vizier */
ALLEGRO_BITMAP *vizier_walkb_00, *vizier_walkb_01;

/* skeleton */
ALLEGRO_BITMAP *skeleton_walkb_00, *skeleton_walkb_01;

/* shadow */
ALLEGRO_BITMAP *shadow_walkb_00, *shadow_walkb_01;

static void
init_guard_walkb_frameset (void)
{
  struct frameset frameset[GUARD_WALKB_FRAMESET_NMEMB] =
    {{guard_walkb_00,+2,0},{guard_walkb_01,+10,0}};

  memcpy (&guard_walkb_frameset, &frameset,
          GUARD_WALKB_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_fat_guard_walkb_frameset (void)
{
  struct frameset frameset[GUARD_WALKB_FRAMESET_NMEMB] =
    {{fat_guard_walkb_00,+2,0},{fat_guard_walkb_01,+10,0}};

  memcpy (&fat_guard_walkb_frameset, &frameset,
          GUARD_WALKB_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_vizier_walkb_frameset (void)
{
  struct frameset frameset[GUARD_WALKB_FRAMESET_NMEMB] =
    {{vizier_walkb_00,+2,0},{vizier_walkb_01,+10,0}};

  memcpy (&vizier_walkb_frameset, &frameset,
          GUARD_WALKB_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_skeleton_walkb_frameset (void)
{
  struct frameset frameset[GUARD_WALKB_FRAMESET_NMEMB] =
    {{skeleton_walkb_00,+2,0},{skeleton_walkb_01,+10,0}};

  memcpy (&skeleton_walkb_frameset, &frameset,
          GUARD_WALKB_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_shadow_walkb_frameset (void)
{
  struct frameset frameset[GUARD_WALKB_FRAMESET_NMEMB] =
    {{shadow_walkb_00,+2,0},{shadow_walkb_01,+10,0}};

  memcpy (&shadow_walkb_frameset, &frameset,
          GUARD_WALKB_FRAMESET_NMEMB * sizeof (struct frameset));
}

struct frameset *
get_guard_walkb_frameset (enum anim_type t)
{
  switch (t) {
  case GUARD: default: return guard_walkb_frameset;
  case FAT_GUARD: return fat_guard_walkb_frameset;
  case VIZIER: return vizier_walkb_frameset;
  case SKELETON: return skeleton_walkb_frameset;
  case SHADOW: return shadow_walkb_frameset;
  }
}

void
load_guard_walkb (void)
{
  /* guard */
  guard_walkb_00 = load_bitmap (GUARD_WALKB_00);
  guard_walkb_01 = load_bitmap (GUARD_WALKB_01);

  /* fat guard */
  fat_guard_walkb_00 = load_bitmap (FAT_GUARD_WALKB_00);
  fat_guard_walkb_01 = load_bitmap (FAT_GUARD_WALKB_01);

  /* vizier */
  vizier_walkb_00 = load_bitmap (VIZIER_WALKB_00);
  vizier_walkb_01 = load_bitmap (VIZIER_WALKB_01);

  /* skeleton */
  skeleton_walkb_00 = load_bitmap (SKELETON_WALKB_00);
  skeleton_walkb_01 = load_bitmap (SKELETON_WALKB_01);

  /* shadow */
  shadow_walkb_00 = load_bitmap (SHADOW_WALKB_00);
  shadow_walkb_01 = load_bitmap (SHADOW_WALKB_01);

  /* frameset */
  init_guard_walkb_frameset ();
  init_fat_guard_walkb_frameset ();
  init_vizier_walkb_frameset ();
  init_skeleton_walkb_frameset ();
  init_shadow_walkb_frameset ();
}

void
unload_guard_walkb (void)
{
  /* guard */
  destroy_bitmap (guard_walkb_00);
  destroy_bitmap (guard_walkb_01);

  /* fat_guard */
  destroy_bitmap (fat_guard_walkb_00);
  destroy_bitmap (fat_guard_walkb_01);

  /* vizier */
  destroy_bitmap (vizier_walkb_00);
  destroy_bitmap (vizier_walkb_01);

  /* skeleton */
  destroy_bitmap (skeleton_walkb_00);
  destroy_bitmap (skeleton_walkb_01);

  /* shadow */
  destroy_bitmap (shadow_walkb_00);
  destroy_bitmap (shadow_walkb_01);
}

void
guard_walkb (struct anim *g)
{
  g->oaction = g->action;
  g->action = guard_walkb;
  g->f.flip = (g->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct anim *g)
{
  if (g->oaction != guard_walkb) g->i = -1;

  if (g->i == 1) {
    guard_vigilant (g);
    return false;
  }

  struct frameset *frameset = get_guard_walkb_frameset (g->type);
  select_frame (g, frameset, g->i + 1);

  if (g->i == 0) g->j = 10;
  if (g->i == 1) g->j = 3;

  select_xframe (&g->xf, sword_frameset, g->j);

  if (g->j == 10) g->xf.dx = -10, g->xf.dy = +16;

  if (g->type == SKELETON) g->xf.dy += -3;
  if (g->type == SHADOW) g->xf.dy += -2;

  return true;
}

static bool
physics_in (struct anim *g)
{
  struct coord nc; struct pos np, pbf, pmbo, pbb;

  /* collision */
  uncollide (&g->f, &g->fo, &g->fo, +0, true, &g->ci);

  /* fall */
  survey (_bf, pos, &g->f, &nc, &pbf, &np);
  survey (_mbo, pos, &g->f, &nc, &pmbo, &np);
  survey (_bb, pos, &g->f, &nc, &pbb, &np);
  if ((is_strictly_traversable (&pbf)
       || is_strictly_traversable (&pmbo)
       || is_strictly_traversable (&pbb))) {
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
  if (g->i == 0) update_depressible_floor (g, 0, -34);
  else if (g->i == 1) update_depressible_floor (g, -1, -27);
  else keep_depressible_floor (g);
}

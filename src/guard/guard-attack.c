/*
  guard-attack.c -- guard attack module;

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

struct frameset guard_attack_frameset[GUARD_ATTACK_FRAMESET_NMEMB];
struct frameset fat_guard_attack_frameset[GUARD_ATTACK_FRAMESET_NMEMB];
struct frameset vizier_attack_frameset[GUARD_ATTACK_FRAMESET_NMEMB];
struct frameset skeleton_attack_frameset[GUARD_ATTACK_FRAMESET_NMEMB];
struct frameset shadow_attack_frameset[GUARD_ATTACK_FRAMESET_NMEMB];

static void init_guard_attack_frameset (void);
static void init_fat_guard_attack_frameset (void);
static void init_vizier_attack_frameset (void);
static void init_skeleton_attack_frameset (void);
static void init_shadow_attack_frameset (void);
static bool flow (struct anim *g);
static bool physics_in (struct anim *g);
static void physics_out (struct anim *g);

/* guard */
ALLEGRO_BITMAP *guard_attack_00, *guard_attack_01,
  *guard_attack_02, *guard_attack_03, *guard_attack_04,
  *guard_attack_05, *guard_attack_06, *guard_attack_07,
  *guard_attack_counter_defense_00, *guard_attack_counter_defense_01,
  *guard_attack_defended;

/* fat guard */
ALLEGRO_BITMAP *fat_guard_attack_00, *fat_guard_attack_01,
  *fat_guard_attack_02, *fat_guard_attack_03, *fat_guard_attack_04,
  *fat_guard_attack_05, *fat_guard_attack_06, *fat_guard_attack_07,
  *fat_guard_attack_counter_defense_00, *fat_guard_attack_counter_defense_01,
  *fat_guard_attack_defended;

/* vizier */
ALLEGRO_BITMAP *vizier_attack_00, *vizier_attack_01,
  *vizier_attack_02, *vizier_attack_03, *vizier_attack_04,
  *vizier_attack_05, *vizier_attack_06, *vizier_attack_07,
  *vizier_attack_counter_defense_00, *vizier_attack_counter_defense_01,
  *vizier_attack_defended;

/* skeleton */
ALLEGRO_BITMAP *skeleton_attack_00, *skeleton_attack_01,
  *skeleton_attack_02, *skeleton_attack_03, *skeleton_attack_04,
  *skeleton_attack_05, *skeleton_attack_06, *skeleton_attack_07,
  *skeleton_attack_counter_defense_00, *skeleton_attack_counter_defense_01,
  *skeleton_attack_defended;

/* shadow */
ALLEGRO_BITMAP *shadow_attack_00, *shadow_attack_01,
  *shadow_attack_02, *shadow_attack_03, *shadow_attack_04,
  *shadow_attack_05, *shadow_attack_06, *shadow_attack_07,
  *shadow_attack_counter_defense_00, *shadow_attack_counter_defense_01,
  *shadow_attack_defended;

static void
init_guard_attack_frameset (void)
{
  struct frameset frameset[GUARD_ATTACK_FRAMESET_NMEMB] =
    {{guard_attack_00,-6,0},{guard_attack_01,+0,0},
     {guard_attack_02,-3,0},{guard_attack_03,-6,0},
     {guard_attack_04,-7,0},{guard_attack_05,+6,0},
     {guard_attack_06,+9,0},{guard_attack_07,+7,0},
     {guard_attack_counter_defense_00,+0,0},
     {guard_attack_counter_defense_01,+6,0}};

  memcpy (&guard_attack_frameset, &frameset,
          GUARD_ATTACK_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_fat_guard_attack_frameset (void)
{
  struct frameset frameset[GUARD_ATTACK_FRAMESET_NMEMB] =
    {{fat_guard_attack_00,-6,0},{fat_guard_attack_01,+0,0},
     {fat_guard_attack_02,-3,0},{fat_guard_attack_03,-6,0},
     {fat_guard_attack_04,-7,0},{fat_guard_attack_05,+6,0},
     {fat_guard_attack_06,+9,0},{fat_guard_attack_07,+7,0},
     {fat_guard_attack_counter_defense_00,+0,0},
     {fat_guard_attack_counter_defense_01,+6,0}};

  memcpy (&fat_guard_attack_frameset, &frameset,
          GUARD_ATTACK_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_vizier_attack_frameset (void)
{
  struct frameset frameset[GUARD_ATTACK_FRAMESET_NMEMB] =
    {{vizier_attack_00,-6,0},{vizier_attack_01,+0,0},
     {vizier_attack_02,-3,0},{vizier_attack_03,-6,0},
     {vizier_attack_04,-7,0},{vizier_attack_05,+6,0},
     {vizier_attack_06,+9,0},{vizier_attack_07,+7,0},
     {vizier_attack_counter_defense_00,+0,0},
     {vizier_attack_counter_defense_01,+6,0}};

  memcpy (&vizier_attack_frameset, &frameset,
          GUARD_ATTACK_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_skeleton_attack_frameset (void)
{
  struct frameset frameset[GUARD_ATTACK_FRAMESET_NMEMB] =
    {{skeleton_attack_00,-6,0},{skeleton_attack_01,+0,0},
     {skeleton_attack_02,-3,0},{skeleton_attack_03,-6,0},
     {skeleton_attack_04,-7,0},{skeleton_attack_05,+6,0},
     {skeleton_attack_06,+9,0},{skeleton_attack_07,+7,0},
     {skeleton_attack_counter_defense_00,+0,0},
     {skeleton_attack_counter_defense_01,+6,0}};

  memcpy (&skeleton_attack_frameset, &frameset,
          GUARD_ATTACK_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_shadow_attack_frameset (void)
{
  struct frameset frameset[GUARD_ATTACK_FRAMESET_NMEMB] =
    {{shadow_attack_00,-6,0},{shadow_attack_01,+0,0},
     {shadow_attack_02,-3,0},{shadow_attack_03,-6,0},
     {shadow_attack_04,-7,0},{shadow_attack_05,+6,0},
     {shadow_attack_06,+9,0},{shadow_attack_07,+7,0},
     {shadow_attack_counter_defense_00,+0,0},
     {shadow_attack_counter_defense_01,+6,0}};

  memcpy (&shadow_attack_frameset, &frameset,
          GUARD_ATTACK_FRAMESET_NMEMB * sizeof (struct frameset));
}

struct frameset *
get_guard_attack_frameset (enum anim_type t)
{
  switch (t) {
  case GUARD: default: return guard_attack_frameset;
  case FAT_GUARD: return fat_guard_attack_frameset;
  case VIZIER: return vizier_attack_frameset;
  case SKELETON: return skeleton_attack_frameset;
  case SHADOW: return shadow_attack_frameset;
  }
}

ALLEGRO_BITMAP *
get_guard_attack_defended_bitmap (enum anim_type t)
{
  switch (t) {
  case GUARD: default: return guard_attack_defended;
  case FAT_GUARD: return fat_guard_attack_defended;
  case VIZIER: return vizier_attack_defended;
  case SKELETON: return skeleton_attack_defended;
  case SHADOW: return shadow_attack_defended;
  }
}

void
load_guard_attack (void)
{
  /* guard */
  guard_attack_00 = load_bitmap (GUARD_ATTACK_00);
  guard_attack_01 = load_bitmap (GUARD_ATTACK_01);
  guard_attack_02 = load_bitmap (GUARD_ATTACK_02);
  guard_attack_03 = load_bitmap (GUARD_ATTACK_03);
  guard_attack_04 = load_bitmap (GUARD_ATTACK_04);
  guard_attack_05 = load_bitmap (GUARD_ATTACK_05);
  guard_attack_06 = load_bitmap (GUARD_ATTACK_06);
  guard_attack_07 = load_bitmap (GUARD_ATTACK_07);
  guard_attack_counter_defense_00 = load_bitmap (GUARD_ATTACK_COUNTER_DEFENSE_00);
  guard_attack_counter_defense_01 = load_bitmap (GUARD_ATTACK_COUNTER_DEFENSE_01);
  guard_attack_defended = load_bitmap (GUARD_ATTACK_DEFENDED);

  /* fat guard */
  fat_guard_attack_00 = load_bitmap (FAT_GUARD_ATTACK_00);
  fat_guard_attack_01 = load_bitmap (FAT_GUARD_ATTACK_01);
  fat_guard_attack_02 = load_bitmap (FAT_GUARD_ATTACK_02);
  fat_guard_attack_03 = load_bitmap (FAT_GUARD_ATTACK_03);
  fat_guard_attack_04 = load_bitmap (FAT_GUARD_ATTACK_04);
  fat_guard_attack_05 = load_bitmap (FAT_GUARD_ATTACK_05);
  fat_guard_attack_06 = load_bitmap (FAT_GUARD_ATTACK_06);
  fat_guard_attack_07 = load_bitmap (FAT_GUARD_ATTACK_07);
  fat_guard_attack_counter_defense_00 = load_bitmap (FAT_GUARD_ATTACK_COUNTER_DEFENSE_00);
  fat_guard_attack_counter_defense_01 = load_bitmap (FAT_GUARD_ATTACK_COUNTER_DEFENSE_01);
  fat_guard_attack_defended = load_bitmap (FAT_GUARD_ATTACK_DEFENDED);

  /* vizier */
  vizier_attack_00 = load_bitmap (VIZIER_ATTACK_00);
  vizier_attack_01 = load_bitmap (VIZIER_ATTACK_01);
  vizier_attack_02 = load_bitmap (VIZIER_ATTACK_02);
  vizier_attack_03 = load_bitmap (VIZIER_ATTACK_03);
  vizier_attack_04 = load_bitmap (VIZIER_ATTACK_04);
  vizier_attack_05 = load_bitmap (VIZIER_ATTACK_05);
  vizier_attack_06 = load_bitmap (VIZIER_ATTACK_06);
  vizier_attack_07 = load_bitmap (VIZIER_ATTACK_07);
  vizier_attack_counter_defense_00 = load_bitmap (VIZIER_ATTACK_COUNTER_DEFENSE_00);
  vizier_attack_counter_defense_01 = load_bitmap (VIZIER_ATTACK_COUNTER_DEFENSE_01);
  vizier_attack_defended = load_bitmap (VIZIER_ATTACK_DEFENDED);

  /* skeleton */
  skeleton_attack_00 = load_bitmap (SKELETON_ATTACK_00);
  skeleton_attack_01 = load_bitmap (SKELETON_ATTACK_01);
  skeleton_attack_02 = load_bitmap (SKELETON_ATTACK_02);
  skeleton_attack_03 = load_bitmap (SKELETON_ATTACK_03);
  skeleton_attack_04 = load_bitmap (SKELETON_ATTACK_04);
  skeleton_attack_05 = load_bitmap (SKELETON_ATTACK_05);
  skeleton_attack_06 = load_bitmap (SKELETON_ATTACK_06);
  skeleton_attack_07 = load_bitmap (SKELETON_ATTACK_07);
  skeleton_attack_counter_defense_00 = load_bitmap (SKELETON_ATTACK_COUNTER_DEFENSE_00);
  skeleton_attack_counter_defense_01 = load_bitmap (SKELETON_ATTACK_COUNTER_DEFENSE_01);
  skeleton_attack_defended = load_bitmap (SKELETON_ATTACK_DEFENDED);

  /* shadow */
  shadow_attack_00 = load_bitmap (SHADOW_ATTACK_00);
  shadow_attack_01 = load_bitmap (SHADOW_ATTACK_01);
  shadow_attack_02 = load_bitmap (SHADOW_ATTACK_02);
  shadow_attack_03 = load_bitmap (SHADOW_ATTACK_03);
  shadow_attack_04 = load_bitmap (SHADOW_ATTACK_04);
  shadow_attack_05 = load_bitmap (SHADOW_ATTACK_05);
  shadow_attack_06 = load_bitmap (SHADOW_ATTACK_06);
  shadow_attack_07 = load_bitmap (SHADOW_ATTACK_07);
  shadow_attack_counter_defense_00 = load_bitmap (SHADOW_ATTACK_COUNTER_DEFENSE_00);
  shadow_attack_counter_defense_01 = load_bitmap (SHADOW_ATTACK_COUNTER_DEFENSE_01);
  shadow_attack_defended = load_bitmap (SHADOW_ATTACK_DEFENDED);

  /* frameset */
  init_guard_attack_frameset ();
  init_fat_guard_attack_frameset ();
  init_vizier_attack_frameset ();
  init_skeleton_attack_frameset ();
  init_shadow_attack_frameset ();
}

void
unload_guard_attack (void)
{
  /* guard */
  destroy_bitmap (guard_attack_00);
  destroy_bitmap (guard_attack_01);
  destroy_bitmap (guard_attack_02);
  destroy_bitmap (guard_attack_03);
  destroy_bitmap (guard_attack_04);
  destroy_bitmap (guard_attack_05);
  destroy_bitmap (guard_attack_06);
  destroy_bitmap (guard_attack_07);
  destroy_bitmap (guard_attack_counter_defense_00);
  destroy_bitmap (guard_attack_counter_defense_01);
  destroy_bitmap (guard_attack_defended);

  /* fat guard */
  destroy_bitmap (fat_guard_attack_00);
  destroy_bitmap (fat_guard_attack_01);
  destroy_bitmap (fat_guard_attack_02);
  destroy_bitmap (fat_guard_attack_03);
  destroy_bitmap (fat_guard_attack_04);
  destroy_bitmap (fat_guard_attack_05);
  destroy_bitmap (fat_guard_attack_06);
  destroy_bitmap (fat_guard_attack_07);
  destroy_bitmap (fat_guard_attack_counter_defense_00);
  destroy_bitmap (fat_guard_attack_counter_defense_01);
  destroy_bitmap (fat_guard_attack_defended);

  /* vizier */
  destroy_bitmap (vizier_attack_00);
  destroy_bitmap (vizier_attack_01);
  destroy_bitmap (vizier_attack_02);
  destroy_bitmap (vizier_attack_03);
  destroy_bitmap (vizier_attack_04);
  destroy_bitmap (vizier_attack_05);
  destroy_bitmap (vizier_attack_06);
  destroy_bitmap (vizier_attack_07);
  destroy_bitmap (vizier_attack_counter_defense_00);
  destroy_bitmap (vizier_attack_counter_defense_01);
  destroy_bitmap (vizier_attack_defended);

  /* skeleton */
  destroy_bitmap (skeleton_attack_00);
  destroy_bitmap (skeleton_attack_01);
  destroy_bitmap (skeleton_attack_02);
  destroy_bitmap (skeleton_attack_03);
  destroy_bitmap (skeleton_attack_04);
  destroy_bitmap (skeleton_attack_05);
  destroy_bitmap (skeleton_attack_06);
  destroy_bitmap (skeleton_attack_07);
  destroy_bitmap (skeleton_attack_counter_defense_00);
  destroy_bitmap (skeleton_attack_counter_defense_01);
  destroy_bitmap (skeleton_attack_defended);

  /* shadow */
  destroy_bitmap (shadow_attack_00);
  destroy_bitmap (shadow_attack_01);
  destroy_bitmap (shadow_attack_02);
  destroy_bitmap (shadow_attack_03);
  destroy_bitmap (shadow_attack_04);
  destroy_bitmap (shadow_attack_05);
  destroy_bitmap (shadow_attack_06);
  destroy_bitmap (shadow_attack_07);
  destroy_bitmap (shadow_attack_counter_defense_00);
  destroy_bitmap (shadow_attack_counter_defense_01);
  destroy_bitmap (shadow_attack_defended);
}

void
guard_attack (struct anim *g)
{
  g->oaction = g->action;
  g->action = guard_attack;
  g->f.flip = (g->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct anim *g)
{
  if (g->oaction != guard_attack) {
    /* if (g->id == 0) */
    /*   printf ("guard_attack: dx = %i, g->i = %i, g->f.b = %p\n", */
    /*           g->f.c.x - g->of.c.x, g->i, g->f.b); */

    g->i = -1;
    g->attack_defended = 0;
    g->counter_attacked = 0;
    g->counter_defense = 0;
    g->of = g->f;
  }

  if (g->oaction == guard_defense) g->i = 1;

  if (g->i < 7 && g->counter_defense == 2) {
    g->i = 7;
    /* g->f = g->of; */
  }

  if (g->i == 9) {
    g->attack_defended = 0;
    g->counter_defense = 0;
    g->counter_attacked = 0;
    g->hurt_enemy_in_counter_attack = false;
    guard_defense (g);
    return false;
  } else if (g->i == 7
             && g->counter_defense != 2) {
    g->attack_defended = 0;
    g->counter_defense = 0;
    g->counter_attacked = 0;
    g->hurt_enemy_in_counter_attack = false;
    guard_vigilant (g);
    return false;
  }

  if (g->i == 4 && g->attack_defended)
    g->f.c.x += (g->f.dir == LEFT) ? +9 : -9;

  if (g->i == 3 && g->attack_defended)
    g->i = 4;

  struct frameset *frameset = get_guard_attack_frameset (g->type);
  select_frame (g, frameset, g->i + 1);

  if (g->i == 0) g->j = 12;
  if (g->i == 1) g->j = 0;
  if (g->i == 2) g->j = 5;
  if (g->i == 3) g->j = 1;
  if (g->i == 4) g->j = 2;
  if (g->i == 5) g->j = 6;
  if (g->i == 6) g->j = 7;
  if (g->i == 7) g->j = 3;
  if (g->i == 8) g->j = 9;
  if (g->i == 9) g->j = 10;

  select_xframe (&g->xf, sword_frameset, g->j);

  if (g->j == 1) g->xf.dy = +9;
  if (g->j == 7) g->xf.dx = +0, g->xf.dy = +23;
  if (g->j == 10) g->xf.dx = -10, g->xf.dy = +16;

  if (g->i == 5 && g->attack_defended) g->fo.dx = -2;

  if (g->type == SKELETON) g->xf.dy += -3;
  if (g->type == SHADOW) g->xf.dy += -2;

  return true;
}

static bool
physics_in (struct anim *g)
{
  struct coord nc; struct pos np, pbf, pmbo, pbb;

  /* collision */
  uncollide (&g->f, &g->fo, &g->fo, +8, false, &g->ci);

  /* fall */
  survey (_bf, pos, &g->f, &nc, &pbf, &np);
  survey (_mbo, pos, &g->f, &nc, &pmbo, &np);
  survey (_bb, pos, &g->f, &nc, &pbb, &np);
  if (is_strictly_traversable (&pbf)
      || is_strictly_traversable (&pmbo)
      || is_strictly_traversable (&pbb)) {
    g->xf.b = NULL;
    guard_fall (g);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *g)
{
  /* sound */
  if (g->i == 4) play_sample (sword_attack_sample, NULL, g->id);

  /* depressible floors */
  if (g->i == 3) update_depressible_floor (g, -4, -41);
  else if (g->i == 4) update_depressible_floor (g, -9, -48);
  else if (g->i == 6) update_depressible_floor (g, -3, -33);
  else if (g->i == 7) update_depressible_floor (g, -1, -24);
  else keep_depressible_floor (g);
}

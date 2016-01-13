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
#include "engine/fight.h"
#include "engine/kid/kid.h"
#include "guard.h"

struct frameset guard_attack_frameset[GUARD_ATTACK_FRAMESET_NMEMB];

static void init_guard_attack_frameset (void);
static bool flow (struct anim *g);
static bool physics_in (struct anim *g);
static void physics_out (struct anim *g);

ALLEGRO_BITMAP *guard_attack_01, *guard_attack_02,
  *guard_attack_03, *guard_attack_04, *guard_attack_05,
  *guard_attack_06, *guard_attack_07, *guard_attack_08,
  *guard_attack_counter_defense_01, *guard_attack_counter_defense_02,
  *guard_attack_defended;

static void
init_guard_attack_frameset (void)
{
  struct frameset frameset[GUARD_ATTACK_FRAMESET_NMEMB] =
    {{guard_attack_01,-6,0},{guard_attack_02,+0,0},
     {guard_attack_03,-3,0},{guard_attack_04,-6,0},
     {guard_attack_05,-7,0},{guard_attack_06,+6,0},
     {guard_attack_07,+9,0},{guard_attack_08,+7,0},
     {guard_attack_counter_defense_01,+0,0},
     {guard_attack_counter_defense_02,+6,0}};

  memcpy (&guard_attack_frameset, &frameset,
          GUARD_ATTACK_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_guard_attack (void)
{
  /* bitmaps */
  guard_attack_01 = load_bitmap (GUARD_ATTACK_01);
  guard_attack_02 = load_bitmap (GUARD_ATTACK_02);
  guard_attack_03 = load_bitmap (GUARD_ATTACK_03);
  guard_attack_04 = load_bitmap (GUARD_ATTACK_04);
  guard_attack_05 = load_bitmap (GUARD_ATTACK_05);
  guard_attack_06 = load_bitmap (GUARD_ATTACK_06);
  guard_attack_07 = load_bitmap (GUARD_ATTACK_07);
  guard_attack_08 = load_bitmap (GUARD_ATTACK_08);
  guard_attack_counter_defense_01 = load_bitmap (GUARD_ATTACK_COUNTER_DEFENSE_01);
  guard_attack_counter_defense_02 = load_bitmap (GUARD_ATTACK_COUNTER_DEFENSE_02);

  guard_attack_defended = load_bitmap (GUARD_ATTACK_DEFENDED);

  /* frameset */
  init_guard_attack_frameset ();
}

void
unload_guard_attack (void)
{
  al_destroy_bitmap (guard_attack_01);
  al_destroy_bitmap (guard_attack_02);
  al_destroy_bitmap (guard_attack_03);
  al_destroy_bitmap (guard_attack_04);
  al_destroy_bitmap (guard_attack_05);
  al_destroy_bitmap (guard_attack_06);
  al_destroy_bitmap (guard_attack_07);
  al_destroy_bitmap (guard_attack_08);
  al_destroy_bitmap (guard_attack_counter_defense_01);
  al_destroy_bitmap (guard_attack_counter_defense_02);

  al_destroy_bitmap (guard_attack_defended);
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

  select_frame (g, guard_attack_frameset, g->i + 1);

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

  return true;
}

static bool
physics_in (struct anim *g)
{
  struct coord nc; struct pos np, pbf, pmbo, pbb;

  /* collision */
  if (is_colliding (&g->f, &g->fo, +PLACE_WIDTH, false, &g->ci)
      && g->i == 0) {
    guard_vigilant (g);
    return false;
  }

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
  if (g->i == 4) play_sample (sword_attack_sample, g->f.c.room);

  /* depressible floors */
  if (g->i == 3) update_depressible_floor (g, -4, -41);
  else if (g->i == 4) update_depressible_floor (g, -9, -48);
  else if (g->i == 6) update_depressible_floor (g, -3, -33);
  else if (g->i == 7) update_depressible_floor (g, -1, -24);
  else keep_depressible_floor (g);
}

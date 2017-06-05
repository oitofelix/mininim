/*
  guard-sword-normal.c -- guard sword normal module;

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
guard_sword_normal (struct actor *g)
{
  g->oaction = g->action;
  g->oi = g->i;
  g->action = guard_sword_normal;
  g->f.flip = (g->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct actor *g)
{
  struct pos pmt;

  bool normal = g->key.down;
  bool walkf = ((g->f.dir == RIGHT) && g->key.right)
    || ((g->f.dir == LEFT) && g->key.left);
  bool walkb = ((g->f.dir == RIGHT) && g->key.left)
    || ((g->f.dir == LEFT) && g->key.right);
  bool defense = g->key.up && ! g->key.shift
    && ! g->key.left && ! g->key.right;
  bool attack = g->key.shift && ! g->key.up
    && ! g->key.left && ! g->key.right;

  g->enemy_defended_my_attack = 0;
  g->enemy_counter_attacked_myself = 0;
  g->i_counter_defended = 0;
  g->hurt_enemy_in_counter_attack = false;
  g->attack_range_far = false;
  g->attack_range_near = false;

  if (g->oaction != guard_sword_normal) {
    g->i = -1;
    g->i_initiated_attack = false;
  }

  if (g->oaction == guard_sword_normal) {

    if (g->refraction <= 0) {
      /* walkf */
      if (walkf) {
        guard_sword_walkf (g);
        return false;
      }

      /* attack */
      if (attack) {
        guard_sword_attack (g);
        return false;
      }
    }

    /* death */
    if (g->current_hp <= 0) {
      survey (_mt, pos, &g->f, NULL, &pmt, NULL);
      g->p = pmt;
      guard_die (g);
      return false;
    }

    /* normal */
    if (g->i == 2 && normal) {
      guard_normal (g);
      return false;
    }

    /* walkb */
    if (walkb) {
      guard_sword_walkb (g);
      return false;
    }

    /* defense */
    if (defense) {
      guard_sword_defense (g);
      return false;
    }
  }

  select_actor_frame (g, NULL, "SWORD_NORMAL", g->i == 2 ? g->i : g->i + 1);
  select_actor_xframe (g, NULL, "SWORD", 4);

  return true;
}

static bool
physics_in (struct actor *g)
{
  /* collision */
  uncollide_static_fight (g);

  /* fall */
  if (is_falling (&g->f, _mbo, +0, +0)) {
    guard_fall (g);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *g)
{
  /* depressible floors */
  if (g->i == 0) update_depressible_floor (g, -1, -27);
  else keep_depressible_floor (g);
}

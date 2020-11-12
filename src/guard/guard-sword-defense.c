/*
  guard-sword-defense.c -- guard sword defense module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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
guard_sword_defense (struct actor *g)
{
  g->oaction = g->action;
  g->oi = g->i;
  g->action = guard_sword_defense;
  g->f.flip = (g->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct actor *g)
{
  if (g->oaction != guard_sword_defense) {
    g->i = -1;

    struct actor *ge = get_actor_by_id (g->enemy_id);
    if (ge && g->i_counter_defended)
      ge->enemy_defended_my_attack = 1;

    g->enemy_defended_my_attack = 0;
    g->i_counter_defended = 0;
    g->enemy_counter_attacked_myself = 0;
    g->hurt_enemy_in_counter_attack = false;
  }

  struct actor *ke = get_actor_by_id (g->enemy_id);
  if (g->i == 1) {
    guard_sword_attack (g);
    return false;
  } else if (g->i == 0 && ke && ke->enemy_defended_my_attack == 2
           && ke->enemy_counter_attacked_myself != 2) {
    if (ke->i_initiated_attack) g->angry = 60;
    guard_sword_walkb (g);
    return false;
  } else if (g->i == 0 && ! (ke && ke->enemy_defended_my_attack == 2)) {
    guard_sword_normal (g);
    return false;
  }

  select_actor_frame (g, NULL, "SWORD_DEFENSE", g->i + 1);

  if (g->i == 0) g->j = 11;

  select_actor_xframe (g, NULL, "SWORD", g->j);

  if (g->i == 1) g->xf.b = NULL;

  /* if (g->id == 0) */
  /*   eprintf ("guard_sword_defense: g->i = %i, g->fo.dx = %i\n", */
  /*            g->i, g->fo.dx); */

  return true;
}

static bool
physics_in (struct actor *g)
{
  /* collision */
  uncollide_back_fight (g);

  /* fall */
  if (is_falling (&g->f, _mbo, +0, +0)) {
    g->xf.b = NULL;
    guard_fall (g);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *g)
{
  /* place on the ground */
  place_on_the_ground (&g->f, &g->f.c);

  /* depressible floors */
  keep_depressible_floor (g);
}

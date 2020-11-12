/*
  guard-attack.c -- guard attack module;

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
guard_sword_attack (struct actor *g)
{
  g->oaction = g->action;
  g->oi = g->i;
  g->action = guard_sword_attack;
  g->f.flip = (g->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct actor *g)
{
  if (g->oaction != guard_sword_attack) {
    /* if (g->id == 0) */
    /*   printf ("guard_sword_attack: dx = %i, g->i = %i, g->f.b = %p\n", */
    /*           g->f.c.x - g->of.c.x, g->i, g->f.b); */

    g->i = -1;
    g->of = g->f;
    g->angry = 0;
    if (g->oaction == guard_sword_normal)
      g->i_initiated_attack = true;
  }

  if (g->oaction == guard_sword_defense) g->i = 1;

  if (g->i < 7 && g->i_counter_defended == 2) {
    g->i = 7;
    /* g->f = g->of; */
  }

  if (g->i == 9) {
    guard_sword_defense (g);
    return false;
  } else if (g->i == 7
             && g->i_counter_defended != 2) {
    guard_sword_normal (g);
    return false;
  }

  /* TODO: query video mode for these offsets */
  if (g->i == 4 && g->placed_at_attack_frame)
    move_frame (&g->f, _tb, +0, +8, +8);

  select_actor_frame (g, NULL, "SWORD_ATTACK", g->i + 1);

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

  select_actor_xframe (g, NULL, "SWORD", g->j);

  return true;
}

static bool
physics_in (struct actor *g)
{
  /* collision */
  uncollide_front_fight (g);

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

  /* sound */
  if (g->i == 4
      && ! search_audio_instance (&harm_audio, 0, NULL, g->enemy_id))
    play_audio (&sword_attack_audio, NULL, g->id);

  /* depressible floors */
  if (g->i == 3) update_depressible_floor (g, -4, -41);
  else if (g->i == 4) update_depressible_floor (g, -9, -48);
  else if (g->i == 6) update_depressible_floor (g, -3, -33);
  else if (g->i == 7) update_depressible_floor (g, -1, -24);
  else keep_depressible_floor (g);
}

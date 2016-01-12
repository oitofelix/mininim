/*
  guard-normal.c -- guard normal module;

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
#include "engine/level.h"
#include "engine/door.h"
#include "engine/potion.h"
#include "engine/sword.h"
#include "engine/loose-floor.h"
#include "engine/level-door.h"
#include "guard.h"

static bool flow (struct anim *g);
static bool physics_in (struct anim *g);
static void physics_out (struct anim *g);

ALLEGRO_BITMAP *guard_normal_00;

void
load_guard_normal (void)
{
  guard_normal_00 = load_bitmap (GUARD_NORMAL_00);
}

void
unload_guard_normal (void)
{
  al_destroy_bitmap (guard_normal_00);
}

void
guard_normal (struct anim *g)
{
  g->oaction = g->action;
  g->action = guard_normal;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct anim *g)
{
  struct coord nc;
  struct pos np, pmt;

  if (g->oaction == guard_normal
      && g->current_lives <= 0) {
    survey (_mt, pos, &g->f, &nc, &pmt, &np);
    g->p = pmt;
    /* guard_die (g); */
    return false;
  }

  g->fo.b = guard_normal_00;
  g->fo.dx = g->fo.dy = +0;

  select_xframe (&g->xf, sword_frameset, 30);

  return true;
}

static bool
physics_in (struct anim *g)
{
  struct coord nc; struct pos np, pmbo, pbb;

  /* inertia */
  g->inertia = g->cinertia = 0;

  /* collision */
  /* if (kid_door_split_collision (g)) return false; */

  /* fall */
  survey (_mbo, pos, &g->f, &nc, &pmbo, &np);
  survey (_bb, pos, &g->f, &nc, &pbb, &np);
  if (is_strictly_traversable (&pmbo)
      && is_strictly_traversable (&pbb)) {
    /* guard_fall (g); */
    return false;
  }

  return true;
}

static void
physics_out (struct anim *g)
{
  /* depressible floors */
  keep_depressible_floor (g);
}

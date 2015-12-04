/*
  kid-misstep.c -- kid misstep module;

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
#include "kid.h"

struct frameset kid_misstep_frameset[KID_MISSTEP_FRAMESET_NMEMB];

static void init_kid_misstep_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

static void
init_kid_misstep_frameset (void)
{
  struct frameset frameset[KID_MISSTEP_FRAMESET_NMEMB] =
    {{kid_walk_01,-1,0},{kid_walk_02,-1,0},{kid_walk_03,+0,0},
     {kid_walk_04,-8,0},{kid_walk_05,-7,0},{kid_walk_06,-4,0},
     {kid_jump_14,+8,0},{kid_couch_10,+8,0},{kid_couch_11,+3,0},
     {kid_couch_12,+0,0},{kid_couch_13,+4,0}};

  memcpy (&kid_misstep_frameset, &frameset,
          KID_MISSTEP_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_misstep (void)
{
  /* frameset */
  init_kid_misstep_frameset ();
}

void
unload_kid_misstep (void)
{
  /* for symmetry */
}

void
kid_misstep (void)
{
  kid.oaction = kid.action;
  kid.action = kid_misstep;
  kid.f.flip = (kid.f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_misstep) kid->i = -1, misstep = true;;

  if (kid->i == 10) {
    kid_normal ();
    return false;
  }

  select_frame (kid, kid_misstep_frameset, kid->i + 1);

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np;
  enum confg cmba;

  /* fall */
  cmba = survey (_mba, pos, &kid->f, &nc, &np, &np)->fg;
  if (cmba == NO_FLOOR) {
    kid_fall ();
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  struct coord nc; struct pos pmbo, np;

  /* depressible floors */
  keep_depressible_floor (kid);

  /* loose floor shaking */
  survey (_mbo, pos, &kid->f, &nc, &pmbo, &np);
  if (kid->i == 8) shake_loose_floor_row (&pmbo);

  /* sound */
  if (kid->i == 7) sample_step = true;
}

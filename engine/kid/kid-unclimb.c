/*
  kid-unclimb.c -- kid unclimb module;

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

static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

void
load_kid_unclimb (void)
{
  return;
}

void
unload_kid_unclimb (void)
{
  return;
}

void
kid_unclimb (void)
{
  kid.oaction = kid.action;
  kid.action = kid_unclimb;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_unclimb) kid->i = 14;
  if (kid->oaction == kid_climb) kid->i = 3;

  if (kid->i == 0) {
    /* in this function the hang_pos variable holds the position of
       the hanged construction instead of the usual hangable position
       facing it */
    prel (&hang_pos, &hang_pos, +1, (kid->f.dir == LEFT) ? +1 : -1);
    pos2view (&hang_pos, &hang_pos);
    kid_hang ();
    return false;
  }

  if (kid->i == 14) {
    kid->f.b = kid_climb_frameset[13].frame;
    kid->f.c.x = PLACE_WIDTH * hang_pos.place + 18;
    kid->f.c.y = PLACE_HEIGHT * hang_pos.floor + 25;
  }

  kid->i--;

  kid->fo.b = kid_climb_frameset[kid->i].frame;
  kid->fo.dx = -kid_climb_frameset[kid->i + 1].dx;
  kid->fo.dy = -kid_climb_frameset[kid->i + 1].dy;

  if (kid->oaction == kid_climb) kid->fo.dx += +2;
  if (kid->i == 1) kid->fo.dx += -3;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np;
  enum confg ctf;

  /* fall */
  ctf = survey (_tf, pos, &kid->f, &nc, &np, &np)->fg;
  if (ctf == NO_FLOOR) {
    kid_fall ();
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  clear_depressible_floor (kid);
  press_depressible_floor (&hang_pos);
}

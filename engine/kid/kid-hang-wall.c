/*
  kid-hang-wall.c -- kid hang wall module;

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
load_kid_hang_wall (void)
{
  /* for symmetry */
}

void
unload_kid_hang_wall (void)
{
  /* for symmetry */
}

void
kid_hang_wall (void)
{
  kid.oaction = kid.action;
  kid.action = kid_hang_wall;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_hang_wall)
    kid->i = 3, kid->wait = 0, kid->reverse = false;

  int dir = (kid->f.dir == LEFT) ? -1 : +1;
  enum confg hanged_con = crel (&hang_pos, -1, dir)->fg;

  if ((! shift_key && (kid->reverse || kid->i > 4))
      || hang_limit || hanged_con == NO_FLOOR) {
    if (con (&hang_pos)->fg == NO_FLOOR) {
      kid_fall ();
      hang_limit = false;
      return false;
    }
    kid->f.b = kid_vjump_frameset[13].frame;
    kid->f.c.x += (kid->f.dir == LEFT) ? +12 : -12;
    kid->f.c.y = PLACE_HEIGHT * hang_pos.floor - 8;
    kid_vjump ();
    hang_limit = false;
    return false;
  } if (up_key && ! hang_limit) {
    kid_climb ();
    return false;
  }

  if (! kid->reverse && kid->i < 6) kid->i++;
  else if (! kid->reverse && kid->i == 6) kid->reverse = true;
  else if (kid->reverse && kid->i > 4 && kid->wait == 0)
    kid->i--, kid->wait = 1;
  else if (kid->reverse && kid->i >= 4 && kid->wait > 0) kid->wait--;

  kid->fo.b = kid_hang_frameset[kid->i].frame;
  kid->fo.dx = (kid->reverse) ? -kid_hang_frameset[kid->i + 1].dx
    : kid_hang_frameset[kid->i].dx;
  kid->fo.dy = (kid->reverse) ? -kid_hang_frameset[kid->i + 1].dy
    : kid_hang_frameset[kid->i].dy;

  if (kid->f.b == kid_hang_14) kid->fo.dx = +0, kid->fo.dy = +1;

  if (kid->reverse && kid->wait == 0) {
    kid->fo.dx = 0;
    kid->fo.dy = 0;
  }

  return true;
}

static bool
physics_in (struct anim *kid)
{
  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  clear_depressible_floor (kid);
  int dir = (kid->f.dir == LEFT) ? -1 : +1;
  struct pos hanged_con_pos;
  prel (&hang_pos, &hanged_con_pos, -1, dir);
  press_depressible_floor (&hanged_con_pos);

  /* sound */
  if (! kid->reverse && kid->i == 4 && shift_key && ! up_key
      && ! hang_limit)
    sample_hit_wall = true;
}

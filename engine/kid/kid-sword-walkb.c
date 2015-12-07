/*
  kid-sword-walkb.c -- kid sword walk backward module;

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

struct frameset kid_sword_walkb_frameset[KID_SWORD_WALKB_FRAMESET_NMEMB];

static void init_kid_sword_walkb_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_sword_walkb_10, *kid_sword_walkb_07;

static void
init_kid_sword_walkb_frameset (void)
{
  struct frameset frameset[KID_SWORD_WALKB_FRAMESET_NMEMB] =
    {{kid_sword_walkb_10,-1,0},{kid_sword_walkb_07,+11,0}};

  memcpy (&kid_sword_walkb_frameset, &frameset,
          KID_SWORD_WALKB_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_sword_walkb (void)
{
  /* bitmaps */
  kid_sword_walkb_10 = load_bitmap (KID_SWORD_WALKB_10);
  kid_sword_walkb_07 = load_bitmap (KID_SWORD_WALKB_07);

  /* frameset */
  init_kid_sword_walkb_frameset ();
}

void
unload_kid_sword_walkb (void)
{
  al_destroy_bitmap (kid_sword_walkb_10);
  al_destroy_bitmap (kid_sword_walkb_07);
}

void
kid_sword_walkb (void)
{
  kid.oaction = kid.action;
  kid.action = kid_sword_walkb;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

void
kid_sword_walkb_collision (void)
{
  kid.action = kid_sword_walkb_collision;

  kid.f.c.y = PLACE_HEIGHT * collision_pos.floor + 19;
  kid.f.c.x = (kid.f.dir == LEFT)
    ? PLACE_WIDTH * (collision_pos.place + 2) + 8
    : PLACE_WIDTH * (collision_pos.place - 1) - 8;
  kid.f.b = kid_sword_walkb_frameset[0].frame;

  kid_sword_walkb ();
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_sword_walkb) kid->i = -1;

  if (kid->i == 1) {
    kid_sword_normal ();
    return false;
  }

  if (kid->i == -1) kid->j = 10;
  if (kid->i == 0) kid->j = 17;

  if (kid->f.b == kid_sword_attack_frameset[5].frame) kid->i = 0;

  select_frame (kid, kid_sword_walkb_frameset, kid->i + 1);
  select_xframe (kid, sword_frameset, kid->j);

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np;
  enum confg cbf, cmbo, cbb;

  /* fall */
  cbf = survey (_bf, pos, &kid->f, &nc, &np, &np)->fg;
  cmbo = survey (_mbo, pos, &kid->f, &nc, &np, &np)->fg;
  cbb = survey (_bb, pos, &kid->f, &nc, &np, &np)->fg;
  if (cbf == NO_FLOOR || cmbo == NO_FLOOR || cbb == NO_FLOOR) {
    kid->xframe = NULL;
    kid_fall ();
    return false;
  }

  /* collision */
  if (is_colliding (&kid->f, &kid->fo, true)) {
    kid_sword_walkf_collision ();
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  if (kid->i == 1) update_depressible_floor (kid, -1, -24);
  else keep_depressible_floor (kid);
}

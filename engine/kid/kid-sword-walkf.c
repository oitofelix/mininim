/*
  kid-sword-walkf.c -- kid sword walk forward module;

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

struct frameset kid_sword_walkf_frameset[KID_SWORD_WALKF_FRAMESET_NMEMB];

static void init_kid_sword_walkf_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_sword_walkf_14, *kid_sword_walkf_15;

static void
init_kid_sword_walkf_frameset (void)
{
  struct frameset frameset[KID_SWORD_WALKF_FRAMESET_NMEMB] =
    {{kid_sword_walkf_14,-14,0},{kid_sword_walkf_15,-4,0}};

  memcpy (&kid_sword_walkf_frameset, &frameset,
          KID_SWORD_WALKF_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_sword_walkf (void)
{
  /* bitmaps */
  kid_sword_walkf_14 = load_bitmap (KID_SWORD_WALKF_14);
  kid_sword_walkf_15 = load_bitmap (KID_SWORD_WALKF_15);

  /* frameset */
  init_kid_sword_walkf_frameset ();
}

void
unload_kid_sword_walkf (void)
{
  al_destroy_bitmap (kid_sword_walkf_14);
  al_destroy_bitmap (kid_sword_walkf_15);
}

void
kid_sword_walkf (void)
{
  kid.oaction = kid.action;
  kid.action = kid_sword_walkf;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_sword_walkf) kid->i = -1;

  if (kid->i == 1) {
    kid_sword_normal ();
    return false;
  }

  select_frame (kid, kid_sword_walkf_frameset, kid->i + 1);
  select_xframe (kid, sword_frameset, 13);

  if (kid->i == 0) kid->xdx = -19;
  if (kid->i == 1) kid->xdx = -21;

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
  int w = al_get_bitmap_width (kid->xframe);
  if (is_colliding (&kid->f, _tf, pos, 0, false, -kid->fo.dx + w / 2))
    to_collision_edge (&kid->f, kid->fo.b, _tf, pos, 0, false, -kid->fo.dx + w / 2);

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  if (kid->i == 1) update_depressible_floor (kid, -7, -35);
  else keep_depressible_floor (kid);
}

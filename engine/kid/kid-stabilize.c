/*
  kid-stabilize.c -- kid stabilize module;

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

struct frameset kid_stabilize_frameset[KID_STABILIZE_FRAMESET_NMEMB];

static void init_kid_stabilize_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_stabilize_05, *kid_stabilize_06, *kid_stabilize_07,
  *kid_stabilize_08;

static void
init_kid_stabilize_frameset (void)
{
  struct frameset frameset[KID_STABILIZE_FRAMESET_NMEMB] =
    {{kid_stabilize_05,-3,0},{kid_stabilize_06,-4,0},
     {kid_stabilize_07,+2,0},{kid_stabilize_08,+4,0}};

  memcpy (&kid_stabilize_frameset, &frameset,
          KID_STABILIZE_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_stabilize (void)
{
  /* bitmaps */
  kid_stabilize_05 = load_bitmap (KID_STABILIZE_05);
  kid_stabilize_06 = load_bitmap (KID_STABILIZE_06);
  kid_stabilize_07 = load_bitmap (KID_STABILIZE_07);
  kid_stabilize_08 = load_bitmap (KID_STABILIZE_08);

  /* frameset */
  init_kid_stabilize_frameset ();
}

void
unload_kid_stabilize (void)
{
  al_destroy_bitmap (kid_stabilize_05);
  al_destroy_bitmap (kid_stabilize_06);
  al_destroy_bitmap (kid_stabilize_07);
  al_destroy_bitmap (kid_stabilize_08);
}

void
kid_stabilize (void)
{
  kid.oaction = kid.action;
  kid.action = kid_stabilize;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

void
kid_stabilize_collision (void)
{
  kid.action = kid_stabilize_collision;
  place_frame (&kid.f, &kid.f, kid_stabilize_frameset[0].frame,
               &collision_pos, (kid.f.dir == LEFT)
               ? PLACE_WIDTH + 12 : -PLACE_WIDTH + 28, +17);
  kid_stabilize ();
  sample_hit_wall = true;
}

void
kid_stabilize_back_collision (void)
{
  kid.action = kid_stabilize_back_collision;
  place_frame (&kid.f, &kid.f, kid_stabilize_frameset[0].frame,
               &collision_pos, (kid.f.dir == LEFT)
               ? -PLACE_WIDTH + 28 : PLACE_WIDTH + 12, +17);
  kid_stabilize ();
  sample_hit_wall = true;
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_stabilize) {
    kid->i = -1, misstep = false;
    if (kid->oaction == kid_stabilize_collision) {
      kid->i = 1; kid->collision = true;
    } else kid->collision = false;
  }

  if (! turn)
    turn = ((kid->f.dir == RIGHT) && left_key)
      || ((kid->f.dir == LEFT) && right_key);
  bool run = (((kid->f.dir == RIGHT) && right_key)
              || ((kid->f.dir == LEFT) && left_key))
    && ! shift_key;
  bool jump = ((kid->f.dir == RIGHT) && right_key && up_key)
    || ((kid->f.dir == LEFT) && left_key && up_key);
  bool couch = down_key;

  int dc = dist_collision (&kid->f, &kid->fo, false);
  int df = dist_con (&kid->f, _bb, pos, -4, false, NO_FLOOR);

  if (kid->i >= 0 && ! kid->collision) {
    if (couch) {
      kid_couch ();
      return false;
    } else if (jump) {
      kid_jump ();
      return false;
    } else if (turn) {
      kid_turn ();
      turn = false;
      return false;
    } else if (run && dc > PLACE_WIDTH && df > PLACE_WIDTH) {
      kid_start_run ();
      return false;
    }
  }

  if (kid->i == 3) {
    kid_normal ();
    turn = false;
    return false;
  }

  select_frame (kid, kid_stabilize_frameset, kid->i + 1);

  if (kid->f.b == kid_stop_run_frameset[3].frame) kid->fo.dx = -5;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np;
  enum confg cmbo, cbb;

  /* inertia */
  inertia = 0;

  /* fall */
  cmbo = survey (_mbo, pos, &kid->f, &nc, &np, &np)->fg;
  cbb = survey (_bb, pos, &kid->f, &nc, &np, &np)->fg;
  if (cmbo == NO_FLOOR && cbb == NO_FLOOR) {
    kid_fall ();
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  if (kid->collision && kid->i == 1)
    update_depressible_floor (kid, -13, -18);
  else keep_depressible_floor (kid);
}

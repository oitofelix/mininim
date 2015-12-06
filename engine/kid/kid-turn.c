/*
  kid-turn.c -- kid turn module;

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

struct frameset kid_turn_frameset[KID_TURN_FRAMESET_NMEMB];

static void init_kid_turn_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_turn_01, *kid_turn_02, *kid_turn_03, *kid_turn_04;

static void
init_kid_turn_frameset (void)
{
  struct frameset frameset[KID_TURN_FRAMESET_NMEMB] =
    {{kid_turn_01,+2,0},{kid_turn_02,+1,0},
     {kid_turn_03,-3,0},{kid_turn_04,+0,+0}};

  memcpy (&kid_turn_frameset, &frameset,
          KID_TURN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_turn (void)
{
  /* bitmaps */
  kid_turn_01 = load_bitmap (KID_TURN_01);
  kid_turn_02 = load_bitmap (KID_TURN_02);
  kid_turn_03 = load_bitmap (KID_TURN_03);
  kid_turn_04 = load_bitmap (KID_TURN_04);

  /* frameset */
  init_kid_turn_frameset ();
}

void
unload_kid_turn (void)
{
  al_destroy_bitmap (kid_turn_01);
  al_destroy_bitmap (kid_turn_02);
  al_destroy_bitmap (kid_turn_03);
  al_destroy_bitmap (kid_turn_04);
}

void
kid_turn (void)
{
  kid.oaction = kid.action;
  kid.action = kid_turn;
  kid.f.flip = (kid.f.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_turn) kid->i = -1, misstep = false;

  if (! turn)
    turn = ((kid->f.dir == RIGHT) && right_key)
      || ((kid->f.dir == LEFT) && left_key);
  bool run = ((kid->f.dir == RIGHT) ? left_key : right_key)
    && ! shift_key;
  bool jump = ((kid->f.dir == RIGHT) && left_key && up_key)
    || ((kid->f.dir == LEFT) && right_key && up_key);
  bool couch = down_key;

  if (kid->i == 3) {
    kid->f.dir = (kid->f.dir == RIGHT) ? LEFT : RIGHT;
    int dc = dist_collision (&kid->f, _tf, pos, 0, false);
    int df = dist_con (&kid->f, _bf, pos, -4, false, NO_FLOOR);

    if (turn) kid->i = -1, turn = false, kid_turn ();
    else if (couch) kid_couch ();
    else if (jump) kid_jump ();
    else if (run && dc > PLACE_WIDTH && df > PLACE_WIDTH)
      kid_start_run ();
    else kid_stabilize ();

    return false;
  }

  if (kid->f.b == kid_keep_sword_frameset[9].frame) kid->i = 2;

  select_frame (kid, kid_turn_frameset, kid->i + 1);

  if (kid->f.b == kid_stabilize_frameset[0].frame) kid->fo.dx = +6;
  if (kid->f.b == kid_stabilize_frameset[1].frame) kid->fo.dx = +10;
  if (kid->f.b == kid_stabilize_frameset[2].frame) kid->fo.dx = +8;
  if (kid->f.b == kid_stabilize_frameset[3].frame) kid->fo.dx = +4;
  if (kid->f.b == kid_turn_frameset[3].frame) kid->fo.dx = +3;
  if (kid->f.b == kid_keep_sword_frameset[9].frame) kid->fo.dx = -2;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np;
  enum confg cbf, cbb;

  /* fall */
  cbf = survey (_bf, pos, &kid->f, &nc, &np, &np)->fg;
  cbb = survey (_bb, pos, &kid->f, &nc, &np, &np)->fg;
  if (cbf == NO_FLOOR && cbb == NO_FLOOR) {
    if (kid->i > 0) kid->f.dir = (kid->f.dir == RIGHT) ? LEFT : RIGHT;
    kid_fall ();
    return false;
  }

  /* collision */
  kid->f.dir = (kid->f.dir == RIGHT) ? LEFT : RIGHT;
  if (is_colliding (&kid->f, _tf, pos, -4, false, -kid->fo.dx)) {
    to_collision_edge (&kid->f, kid->fo.b, _tf, pos, -4, false, 0);
    kid->fo.dx = 0;
  }
  kid->f.dir = (kid->f.dir == RIGHT) ? LEFT : RIGHT;

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  keep_depressible_floor (kid);
}

bool
is_kid_turn (struct frame *f)
{
  int i;
  for (i = 0; i < KID_TURN_FRAMESET_NMEMB; i++)
    if (f->b == kid_turn_frameset[i].frame) return true;
  return false;
}

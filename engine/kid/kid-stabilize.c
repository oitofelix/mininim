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
kid_stabilize (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_stabilize;
  kid->f.flip = (kid->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame (&kid->f, &kid->f, &kid->fo);
  physics_out (kid);
}

void
kid_stabilize_collision (struct anim *kid)
{
  kid->action = kid_stabilize_collision;
  place_frame (&kid->f, &kid->f, kid_stabilize_frameset[0].frame,
               &kid->ci.p, (kid->f.dir == LEFT)
               ? PLACE_WIDTH + 18 : -PLACE_WIDTH + 24, +17);
  kid_stabilize (kid);
  sample_hit_wall = true;
}

void
kid_stabilize_back_collision (struct anim *kid)
{
  kid->action = kid_stabilize_back_collision;
  place_frame (&kid->f, &kid->f, kid_stabilize_frameset[0].frame,
               &kid->ci.p, (kid->f.dir == LEFT)
               ? -PLACE_WIDTH + 24 : PLACE_WIDTH + 14, +17);
  kid_stabilize (kid);
  sample_hit_wall = true;
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_stabilize) {
    kid->i = -1, kid->misstep = false;
    if (kid->oaction == kid_stabilize_collision) {
      kid->i = 0; kid->collision = true;
    } else kid->collision = false;
    if (kid->oaction == kid_turn) kid->collision = true;
  }

  if (! kid->turn)
    kid->turn = ((kid->f.dir == RIGHT) && left_key)
      || ((kid->f.dir == LEFT) && right_key);
  bool run = (((kid->f.dir == RIGHT) && right_key)
              || ((kid->f.dir == LEFT) && left_key))
    && ! shift_key;
  bool jump = ((kid->f.dir == RIGHT) && right_key && up_key)
    || ((kid->f.dir == LEFT) && left_key && up_key);
  bool couch = down_key;

  int dc = dist_collision (&kid->f, false, &kid->ci);
  int df = dist_fall (&kid->f, false);

  if (kid->i >= 0 && ! kid->collision && kid == current_kid) {
    if (couch) {
      kid_couch (kid);
      return false;
    } else if (jump) {
      kid_jump (kid);
      return false;
    } else if (kid->turn) {
      kid_turn (kid);
      kid->turn = false;
      return false;
    } else if (run && dc > PLACE_WIDTH && df > PLACE_WIDTH) {
      kid_start_run (kid);
      return false;
    }
  }

  if (kid->i == 3) {
    kid_normal (kid);
    kid->turn = false;
    return false;
  }

  select_frame (kid, kid_stabilize_frameset, kid->i + 1);

  if (kid->f.b == kid_stop_run_frameset[3].frame) kid->fo.dx = -5;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np, pmbo, pbb;

  /* inertia */
  kid->inertia = 0;

  /* fall */
  survey (_mbo, pos, &kid->f, &nc, &pmbo, &np);
  survey (_bb, pos, &kid->f, &nc, &pbb, &np);
  if (is_strictly_traversable (&pmbo)
      && is_strictly_traversable (&pbb)) {
    kid_fall (kid);
    return false;
  }

  /* collision */
  if (is_colliding (&kid->f, &kid->fo, +0, false, &kid->ci)
      && ! kid->collision) {
    kid_stabilize_collision (kid);
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

bool
is_kid_stabilize (struct frame *f)
{
  int i;
  for (i = 0; i < KID_STABILIZE_FRAMESET_NMEMB; i++)
    if (f->b == kid_stabilize_frameset[i].frame) return true;
  return false;
}

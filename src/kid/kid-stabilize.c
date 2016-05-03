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

#include "mininim.h"

struct frameset kid_stabilize_frameset[KID_STABILIZE_FRAMESET_NMEMB];

static void init_kid_stabilize_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_stabilize_00, *kid_stabilize_01, *kid_stabilize_02,
  *kid_stabilize_03;

static void
init_kid_stabilize_frameset (void)
{
  struct frameset frameset[KID_STABILIZE_FRAMESET_NMEMB] =
    {{kid_stabilize_00,-3,0},{kid_stabilize_01,-4,0},
     {kid_stabilize_02,+2,0},{kid_stabilize_03,+4,0}};

  memcpy (&kid_stabilize_frameset, &frameset,
          KID_STABILIZE_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_stabilize (void)
{
  /* bitmaps */
  kid_stabilize_00 = load_bitmap (KID_STABILIZE_00);
  kid_stabilize_01 = load_bitmap (KID_STABILIZE_01);
  kid_stabilize_02 = load_bitmap (KID_STABILIZE_02);
  kid_stabilize_03 = load_bitmap (KID_STABILIZE_03);

  /* frameset */
  init_kid_stabilize_frameset ();
}

void
unload_kid_stabilize (void)
{
  destroy_bitmap (kid_stabilize_00);
  destroy_bitmap (kid_stabilize_01);
  destroy_bitmap (kid_stabilize_02);
  destroy_bitmap (kid_stabilize_03);
}

void
kid_stabilize (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_stabilize;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! cutscene && ! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

void
kid_stabilize_collision (struct anim *k)
{
  if (k->action != kid_stabilize)
    play_sample (hit_wall_sample, k->f.c.room);
  k->action = kid_stabilize_collision;
  place_frame (&k->f, &k->f, kid_stabilize_frameset[0].frame,
               &k->ci.p, (k->f.dir == LEFT)
               ? PLACE_WIDTH + 18 : -PLACE_WIDTH + 24, +17);
  kid_stabilize (k);
}

void
kid_stabilize_back_collision (struct anim *k)
{
  if (k->action != kid_stabilize)
    play_sample (hit_wall_sample, k->f.c.room);
  k->action = kid_stabilize_back_collision;
  place_frame (&k->f, &k->f, kid_stabilize_frameset[0].frame,
               &k->ci.p, (k->f.dir == LEFT)
               ? -PLACE_WIDTH + 24 : PLACE_WIDTH + 18, +17);
  kid_stabilize (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_stabilize) {
    k->i = -1, k->misstep = false;
    if (k->oaction == kid_stabilize_collision) {
      k->i = 0; k->collision = true;
    } else k->collision = false;
    if (k->oaction == kid_turn) k->collision = true;
  }

  if (! k->turn)
    k->turn = ((k->f.dir == RIGHT) && k->key.left)
      || ((k->f.dir == LEFT) && k->key.right);
  bool run = (((k->f.dir == RIGHT) && k->key.right)
              || ((k->f.dir == LEFT) && k->key.left))
    && ! k->key.shift;
  bool jump = ((k->f.dir == RIGHT) && k->key.right && k->key.up)
    || ((k->f.dir == LEFT) && k->key.left && k->key.up);
  bool couch = k->key.down;

  int dc = dist_collision (&k->f, false, &k->ci);
  int df = dist_fall (&k->f, false);

  if (k->i >= 0 && ! k->collision) {
    if (couch) {
      kid_couch (k);
      return false;
    } else if (jump) {
      kid_jump (k);
      return false;
    } else if (k->turn) {
      kid_turn (k);
      k->turn = false;
      return false;
    } else if (run && dc > PLACE_WIDTH && df > PLACE_WIDTH) {
      kid_start_run (k);
      return false;
    }
  }

  if (k->i == 3) {
    kid_normal (k);
    k->turn = false;
    return false;
  }

  select_frame (k, kid_stabilize_frameset, k->i + 1);

  if (k->f.b == kid_stop_run_frameset[3].frame) k->fo.dx = -5;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct coord nc; struct pos np, pmbo, pbb;

  /* inertia */
  k->inertia = k->cinertia = 0;

  /* fall */
  survey (_mbo, pos, &k->f, &nc, &pmbo, &np);
  survey (_bb, pos, &k->f, &nc, &pbb, &np);
  if (is_strictly_traversable (&pmbo)
      && is_strictly_traversable (&pbb)) {
    kid_fall (k);
    return false;
  }

  /* collision */
  if (is_colliding (&k->f, &k->fo, +0, false, &k->ci)
      && ! k->collision && k->ci.t != WALL) {
    kid_stabilize_collision (k);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  /* depressible floors */
  if (k->collision && k->i == 1)
    update_depressible_floor (k, -13, -18);
  else keep_depressible_floor (k);
}

bool
is_kid_stabilize (struct frame *f)
{
  int i;
  for (i = 0; i < KID_STABILIZE_FRAMESET_NMEMB; i++)
    if (f->b == kid_stabilize_frameset[i].frame) return true;
  return false;
}

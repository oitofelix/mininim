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

#include "mininim.h"

struct frameset kid_turn_frameset[KID_TURN_FRAMESET_NMEMB];

static void init_kid_turn_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_turn_00, *kid_turn_01, *kid_turn_02, *kid_turn_03;

static void
init_kid_turn_frameset (void)
{
  struct frameset frameset[KID_TURN_FRAMESET_NMEMB] =
    {{kid_turn_00,+2,0},{kid_turn_01,+1,0},
     {kid_turn_02,-3,0},{kid_turn_03,+0,+0}};

  memcpy (&kid_turn_frameset, &frameset,
          KID_TURN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_turn (void)
{
  /* bitmaps */
  kid_turn_00 = load_bitmap (KID_TURN_00);
  kid_turn_01 = load_bitmap (KID_TURN_01);
  kid_turn_02 = load_bitmap (KID_TURN_02);
  kid_turn_03 = load_bitmap (KID_TURN_03);

  /* frameset */
  init_kid_turn_frameset ();
}

void
unload_kid_turn (void)
{
  destroy_bitmap (kid_turn_00);
  destroy_bitmap (kid_turn_01);
  destroy_bitmap (kid_turn_02);
  destroy_bitmap (kid_turn_03);
}

void
kid_turn (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_turn;

  if (k->oaction != kid_turn)
    k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
  k->f.flip = (k->f.dir == LEFT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame_inv = true;
  next_frame (&k->f, &k->f, &k->fo);
  next_frame_inv = false;
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_turn)
    k->i = -1, k->misstep = k->hang = false;

  if (! k->turn)
    k->turn = ((k->f.dir == RIGHT) && k->key.left)
      || ((k->f.dir == LEFT) && k->key.right);
  bool run = ((k->f.dir == RIGHT) ? k->key.right : k->key.left)
    && ! k->key.shift;
  bool jump = ((k->f.dir == RIGHT) && k->key.right && k->key.up)
    || ((k->f.dir == LEFT) && k->key.left && k->key.up);
  bool couch = k->key.down;

  if (k->i == 3) {
    int dc = dist_collision (&k->f, false, &k->ci);
    int df = dist_fall (&k->f, false);

    if (k->hang) kid_hang (k);
    else if (k->turn) {
      k->i = -1; k->turn = false;
      k->action = kid_normal;
      kid_turn (k);
    }
    else if (couch) kid_couch (k);
    else if (jump) kid_jump (k);
    else if (run && dc > PLACE_WIDTH && df > PLACE_WIDTH)
      kid_start_run (k);
    else kid_stabilize (k);

    return false;
  }

  if (k->f.b == kid_keep_sword_frameset[9].frame) k->i = 2;

  /* hang */
  if (k->oaction == kid_fall
      || k->oaction == kid_jump
      || k->oaction == kid_run_jump
      || k->oaction == kid_hang_free
      || k->oaction == kid_hang_wall) {
    k->i = 2, k->hang = true;
    place_frame (&k->f, &k->f, kid_turn_frameset[2].frame,
                 &k->hang_pos, (k->f.dir == LEFT)
                 ? 7 : PLACE_WIDTH, +4);
  }

  select_frame (k, kid_turn_frameset, k->i + 1);

  if (k->f.b == kid_stabilize_frameset[0].frame) k->fo.dx = +6;
  if (k->f.b == kid_stabilize_frameset[1].frame) k->fo.dx = +10;
  if (k->f.b == kid_stabilize_frameset[2].frame) k->fo.dx = +8;
  if (k->f.b == kid_stabilize_frameset[3].frame) k->fo.dx = +4;
  if (k->f.b == kid_turn_frameset[3].frame) k->fo.dx = +3;
  if (k->f.b == kid_keep_sword_frameset[9].frame) k->fo.dx = -2;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct pos pbf, pbb;

  /* collision */
  /* if (! k->hang && kid_door_split_collision (k)) return false; */
  if (is_colliding (&k->f, &k->fo, +0, false, &k->ci)
      && (k->ci.t == DOOR && k->f.dir == RIGHT))
    k->f.c.x += (k->f.dir == LEFT) ? +4 : -4;

  /* fall */
  survey (_bf, pos, &k->f, NULL, &pbf, NULL);
  survey (_bb, pos, &k->f, NULL, &pbb, NULL);
  if (! k->hang
      && is_strictly_traversable (&pbf)
      && is_strictly_traversable (&pbb)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  /* depressible floors */
  keep_depressible_floor (k);
}

bool
is_kid_turn (struct frame *f)
{
  int i;
  for (i = 0; i < KID_TURN_FRAMESET_NMEMB; i++)
    if (f->b == kid_turn_frameset[i].frame) return true;
  return false;
}

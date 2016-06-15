/*
  kid-normal.c -- kid normal module;

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

static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_normal_00;

void
load_kid_normal (void)
{
  kid_normal_00 = load_bitmap (KID_NORMAL_00);
}

void
unload_kid_normal (void)
{
  destroy_bitmap (kid_normal_00);
}

void
kid_normal (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_normal;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! cutscene && ! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  struct pos pbf, pmt;
  survey (_bf, pos, &k->f, NULL, &pbf, NULL);

  k->collision = false;

  bool turn = ((k->f.dir == RIGHT) && k->key.left)
    || ((k->f.dir == LEFT) && k->key.right);
  bool walk = ((k->f.dir == RIGHT) && k->key.right && k->key.shift)
    || ((k->f.dir == LEFT) && k->key.left && k->key.shift);
  bool run = (((k->f.dir == RIGHT) && k->key.right)
              || ((k->f.dir == LEFT) && k->key.left)) && ! walk;
  bool jump = ((k->f.dir == RIGHT) && k->key.right && k->key.up)
    || ((k->f.dir == LEFT) && k->key.left && k->key.up);
  bool couch = k->key.down;
  bool vjump = k->key.up;
  bool drink = is_potion (&pbf) && k->key.shift;
  bool raise_sword = is_sword (&pbf) && k->key.shift;
  bool take_sword = k->key.enter && k->has_sword;

  survey (_mt, pos, &k->f, NULL, &pmt, NULL);
  bool stairs = k->key.up && ! k->key.left && ! k->key.right
    && con (&pmt)->fg == LEVEL_DOOR
    && level_door_at_pos (&pmt)->i == 0
    && k == get_anim_by_id (0);

  if (k->oaction == kid_normal
      && k->current_lives <= 0) {
    survey (_mt, pos, &k->f, NULL, &pmt, NULL);
    k->p = pmt;
    kid_die (k);
    return false;
  }

  if (k->oaction == kid_normal) {
    if (stairs) {
      k->p = pmt;
      kid_stairs (k);
      return false;
    }

    if (couch) {
      kid_couch (k);
      return false;
    }

    if (jump) {
      kid_jump (k);
      return false;
    }

    if (turn) {
      kid_turn (k);
      return false;
    }

    if (vjump) {
      kid_vjump (k);
      return false;
    }
    if (walk) {
      kid_walk (k);
      return false;
    }

    if (run) {
      if (dist_collision (&k->f, false, &k->ci) + 4 < 29)
        kid_walk (k);
      else kid_start_run (k);
      return false;
    }

    if (drink) {
      k->item_pos = pbf;
      place_frame (&k->f, &k->f, kid_couch_frameset[0].frame,
                   &k->item_pos, (k->f.dir == LEFT)
                   ? PLACE_WIDTH + 3 : +9, +27);
      kid_couch (k);
      return false;
    }

    if (raise_sword) {
      k->item_pos = pbf;
      kid_couch (k);
      return false;
    }

    if (take_sword
        && (dist_collision (&k->f, false, &k->ci) > PLACE_WIDTH
            || dist_collision (&k->f, true, &k->ci) > PLACE_WIDTH)) {
      kid_take_sword (k);
      return false;
    }
  }

  k->fo.b = kid_normal_00;
  k->fo.dx = k->fo.dy = +0;

  if (k->f.b == kid_stabilize_frameset[3].frame) k->fo.dx = +2;
  if (k->f.b == kid_walk_frameset[11].frame) k->fo.dx = -1;
  if (k->f.b == kid_jump_frameset[17].frame) k->fo.dx = -2;
  if (k->f.b == kid_couch_frameset[12].frame) k->fo.dx = -2;
  if (k->f.b == kid_vjump_frameset[17].frame) k->fo.dx = +2;
  if (k->f.b == kid_drink_frameset[7].frame) k->fo.dx = +0;
  if (k->f.b == kid_keep_sword_frameset[9].frame) k->fo.dx = +2;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct pos pmbo, pbb;

  /* inertia */
  k->inertia = k->cinertia = 0;

  /* collision */
  if (kid_door_split_collision (k)) return false;

  /* fall */
  survey (_mbo, pos, &k->f, NULL, &pmbo, NULL);
  survey (_bb, pos, &k->f, NULL, &pbb, NULL);
  if (is_strictly_traversable (&pmbo)
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

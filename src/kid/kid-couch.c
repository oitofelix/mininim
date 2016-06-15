/*
  kid-couch.c -- kid couch module;

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

struct frameset kid_couch_frameset[KID_COUCH_FRAMESET_NMEMB];

ALLEGRO_BITMAP *kid_couch_00, *kid_couch_01, *kid_couch_02,
  *kid_couch_03, *kid_couch_04, *kid_couch_05, *kid_couch_06,
  *kid_couch_07, *kid_couch_08, *kid_couch_09, *kid_couch_10,
  *kid_couch_11, *kid_couch_12;

static void init_kid_couch_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

static void
init_kid_couch_frameset (void)
{
  struct frameset frameset[KID_COUCH_FRAMESET_NMEMB] =
    {{kid_couch_00,-3,0},{kid_couch_01,-4,0},{kid_couch_02,+0,0},
     {kid_couch_03,-4,0},{kid_couch_04,-1,0},{kid_couch_05,-4,0},
     {kid_couch_06,+1,0},{kid_couch_07,-2,0},{kid_couch_08,-1,0},
     {kid_couch_09,+0,0},{kid_couch_10,+3,0},{kid_couch_11,+0,0},
     {kid_couch_12,+4,0}};

  memcpy (&kid_couch_frameset, &frameset,
          KID_COUCH_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_couch (void)
{
  /* bitmaps */
  kid_couch_00 = load_bitmap (KID_COUCH_00);
  kid_couch_01 = load_bitmap (KID_COUCH_01);
  kid_couch_02 = load_bitmap (KID_COUCH_02);
  kid_couch_03 = load_bitmap (KID_COUCH_03);
  kid_couch_04 = load_bitmap (KID_COUCH_04);
  kid_couch_05 = load_bitmap (KID_COUCH_05);
  kid_couch_06 = load_bitmap (KID_COUCH_06);
  kid_couch_07 = load_bitmap (KID_COUCH_07);
  kid_couch_08 = load_bitmap (KID_COUCH_08);
  kid_couch_09 = load_bitmap (KID_COUCH_09);
  kid_couch_10 = load_bitmap (KID_COUCH_10);
  kid_couch_11 = load_bitmap (KID_COUCH_11);
  kid_couch_12 = load_bitmap (KID_COUCH_12);

  /* frameset */
  init_kid_couch_frameset ();
}

void
unload_kid_couch (void)
{
  /* bitmaps */
  destroy_bitmap (kid_couch_00);
  destroy_bitmap (kid_couch_01);
  destroy_bitmap (kid_couch_02);
  destroy_bitmap (kid_couch_03);
  destroy_bitmap (kid_couch_04);
  destroy_bitmap (kid_couch_05);
  destroy_bitmap (kid_couch_06);
  destroy_bitmap (kid_couch_07);
  destroy_bitmap (kid_couch_08);
  destroy_bitmap (kid_couch_09);
  destroy_bitmap (kid_couch_10);
  destroy_bitmap (kid_couch_11);
  destroy_bitmap (kid_couch_12);
}

void
kid_couch (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_couch;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

void
kid_couch_collision (struct anim *k)
{
  k->action = kid_couch_collision;
  place_frame (&k->f, &k->f, kid_couch_frameset[0].frame,
               &k->ci.p, (k->f.dir == LEFT)
               ? +PLACE_WIDTH + 24 : -PLACE_WIDTH + 18, +27);
  kid_couch (k);
  play_sample (hit_wall_sample, NULL, k->id);
}

void
kid_couch_suddenly (struct anim *k)
{
  k->action = kid_couch_suddenly;
  struct pos pmt;
  survey (_mt, pos, &k->f, NULL, &pmt, NULL);
  place_frame (&k->f, &k->f, kid_couch_frameset[0].frame,
               &pmt, (k->f.dir == LEFT)
               ? 24 : 18, +27);
  kid_couch (k);
}

static bool
flow (struct anim *k)
{
  struct pos ptf, pbf;
  enum confg ctf;

  if (k->oaction != kid_couch) {
    k->i = -1; k->fall = k->collision = k->misstep = false;
    k->wait = 0;
  }

  if (k->uncouch_slowly) {
    k->wait = 36;
    k->uncouch_slowly = false;
  }

  if (k->oaction == kid_climb)
    k->i = 10;

  if (k->oaction == kid_couch_collision)
    k->collision = true, k->inertia = k->cinertia = 0;

  if (k->oaction == kid_fall) {
    k->fall = true; k->inertia = k->cinertia = 0;
  }

  /* if (k->i > 2 && k->hit_by_loose_floor) */
  /*   k->i = -1; */

  /* unclimb */
  int dir = (k->f.dir == LEFT) ? +1 : -1;
  ctf = survey (_tf, pos, &k->f, NULL, &ptf, NULL)->fg;
  survey (_bf, pos, &k->f, NULL, &pbf, NULL);
  struct pos ph; prel (&pbf, &ph, +1, dir);
  if (k->i == -1
      && ! k->collision
      && ! k->fall
      && ! k->hit_by_loose_floor
      && ! is_valid_pos (&k->item_pos)
      && is_hangable_pos (&ph, k->f.dir)
      && dist_next_place (&k->f, _tf, pos, 0, true) <= 27
      && ! (ctf == DOOR && k->f.dir == LEFT
            && door_at_pos (&ptf)->i > DOOR_CLIMB_LIMIT)) {
    pos2room (&ph, k->f.c.room, &k->hang_pos);
    kid_unclimb (k);
    return false;
  }

  if (k->i == 12) {
    k->hit_by_loose_floor = false;
    kid_normal (k);
    return false;
  }

  if (k->i == 2 && is_valid_pos (&k->item_pos)
      && ! k->collision && ! k->fall) {
    if (is_potion (&k->item_pos)) kid_drink (k);
    else if (is_sword (&k->item_pos)) kid_raise_sword (k);
    else {
      invalid_pos (&k->item_pos); goto no_item;
    }
    return false;
  }

 no_item:
  if (k->i == 2 && k->key.down
      && k->cinertia == 0
      && k->wait-- <= 0
      && ((k->f.dir == LEFT && k->key.left)
          || (k->f.dir == RIGHT && k->key.right))) {
    k->i = 0;
    select_frame (k, kid_couch_frameset, 0);
    return true;
  }

  if (k->i != 2 || (! k->key.down && k->wait-- <= 0))
    k->i++;

  if (k->i == 1 && k->wait > 0 &&
      ((k->fall == true && k->hurt)
       || k->hit_by_loose_floor))
    k->i = 2;

  select_frame (k, kid_couch_frameset, k->i);

  if (k->oaction == kid_climb) k->fo.dx += 7;
  if (k->i > 0 && k->i < 3) k->fo.dx -= k->cinertia;
  if (k->cinertia > 0) k->cinertia--;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct pos pm, pma;
  enum confg cm;

  /* collision */
  if (is_colliding (&k->f, &k->fo, +0, false, &k->ci)
      && k->ci.t != MIRROR) {
    if (k->i <= 2 && k->fall)
      uncollide (&k->f, &k->fo, &k->fo, +0, false, &k->ci);
    else {
      kid_stabilize_collision (k);
      return false;
    }
  } else if (is_colliding (&k->f, &k->fo, +2, false, &k->ci)
             && k->ci.t == MIRROR) {
    if (k->i <= 2)
      uncollide (&k->f, &k->fo, &k->fo, +2, false, &k->ci);
    else {
      kid_stabilize_collision (k);
      return false;
    }
  }

  if (! k->fall && kid_door_split_collision (k)) return false;


  /* fall */
  cm = survey (_m, pos, &k->f, NULL, &pm, NULL)->fg;
  struct loose_floor *l =
    loose_floor_at_pos (prel (&pm, &pma, -1, +0));
  if ((is_strictly_traversable (&pm)
       || (l && l->action == FALL_LOOSE_FLOOR && cm == LOOSE_FLOOR))
      && ! (k->fall && k->i == 0)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  /* depressible floors */
  if (k->i == 0) update_depressible_floor (k, -7, -9);
  else if (k->i == 2) update_depressible_floor (k, -1, -13);
  else if (k->i == 5) update_depressible_floor (k, -19, -20);
  else if (k->i == 7) update_depressible_floor (k, -12, -22);
  else if (k->i == 8) update_depressible_floor (k, -9, -10);
  else if (k->i == 11) update_depressible_floor (k, -6, -12);
  else keep_depressible_floor (k);
}

bool
is_kid_couch (struct frame *f)
{
  int i;
  for (i = 0; i < KID_COUCH_FRAMESET_NMEMB; i++)
    if (f->b == kid_couch_frameset[i].frame) return true;
  return false;
}

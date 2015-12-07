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

struct frameset kid_couch_frameset[KID_COUCH_FRAMESET_NMEMB];

ALLEGRO_BITMAP *kid_couch_01, *kid_couch_02, *kid_couch_03,
  *kid_couch_04, *kid_couch_05, *kid_couch_06, *kid_couch_07,
  *kid_couch_08, *kid_couch_09, *kid_couch_10, *kid_couch_11,
  *kid_couch_12, *kid_couch_13;

static void init_kid_couch_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

static void
init_kid_couch_frameset (void)
{
  struct frameset frameset[KID_COUCH_FRAMESET_NMEMB] =
    {{kid_couch_01,-3,0},{kid_couch_02,-4,0},{kid_couch_03,+0,0},
     {kid_couch_04,-4,0},{kid_couch_05,-1,0},{kid_couch_06,-4,0},
     {kid_couch_07,+1,0},{kid_couch_08,-2,0},{kid_couch_09,-1,0},
     {kid_couch_10,+0,0},{kid_couch_11,+3,0},{kid_couch_12,+0,0},
     {kid_couch_13,+4,0}};

  memcpy (&kid_couch_frameset, &frameset,
          KID_COUCH_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_couch (void)
{
  /* bitmaps */
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
  kid_couch_13 = load_bitmap (KID_COUCH_13);

  /* frameset */
  init_kid_couch_frameset ();
}

void
unload_kid_couch (void)
{
  /* bitmaps */
  al_destroy_bitmap (kid_couch_01);
  al_destroy_bitmap (kid_couch_02);
  al_destroy_bitmap (kid_couch_03);
  al_destroy_bitmap (kid_couch_04);
  al_destroy_bitmap (kid_couch_05);
  al_destroy_bitmap (kid_couch_06);
  al_destroy_bitmap (kid_couch_07);
  al_destroy_bitmap (kid_couch_08);
  al_destroy_bitmap (kid_couch_09);
  al_destroy_bitmap (kid_couch_10);
  al_destroy_bitmap (kid_couch_11);
  al_destroy_bitmap (kid_couch_12);
  al_destroy_bitmap (kid_couch_13);
}

void
kid_couch (void)
{
  kid.oaction = kid.action;
  kid.action = kid_couch;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

void
kid_couch_collision (void)
{
  kid.action = kid_couch_collision;

  kid.f.c.y = PLACE_HEIGHT * collision_pos.floor + 27;
  kid.f.c.x = (kid.f.dir == LEFT)
    ? PLACE_WIDTH * (collision_pos.place + 1) + 24
    : PLACE_WIDTH * (collision_pos.place - 1) + 18;
  kid.f.b = kid_couch_frameset[0].frame;

  kid_couch ();

  sample_hit_wall = true;
}

static bool
flow (struct anim *kid)
{
  struct coord nc; struct pos np, ptf, pbf;
  enum confg ctf;

  if (kid->oaction != kid_couch) {
    kid->i = -1; kid->fall = kid->collision = misstep = false;
    kid->wait = 0;
  }

  if (kid->oaction == kid_climb)
    kid->i = 10, critical_edge = false;

  if (kid->oaction == kid_couch_collision)
    kid->collision = true, inertia = 0;

  if (kid->oaction == kid_fall) {
    kid->fall = true; inertia = 0;
    if (uncouch_slowly) kid->wait = 36;
  }

  /* unclimb */
  int dir = (kid->f.dir == LEFT) ? +1 : -1;
  ctf = survey (_tf, pos, &kid->f, &nc, &ptf, &np)->fg;
  survey (_bf, pos, &kid->f, &nc, &pbf, &np);
  if (kid->i == -1
      && ! kid->collision
      && ! kid->fall
      && item_pos.room == -1
      && crel (&pbf, 0, dir)->fg == NO_FLOOR
      && dist_next_place (&kid->f, _tf, pos, 0, true) < 26
      && ! (ctf == DOOR && kid->f.dir == LEFT
            && door_at_pos (&ptf)->i > DOOR_CLIMB_LIMIT)) {
    prel (&pbf, &hang_pos, +1, (kid->f.dir == LEFT) ? +1 : -1);
    pos2view (&hang_pos, &hang_pos);
    critical_edge =
      (crel (&hang_pos, +1, dir)->fg == NO_FLOOR);
    kid_unclimb ();
    return false;
  }

  if (kid->i == 12) {
    kid_normal ();
    return false;
  }

  if (kid->i == 2 && item_pos.room != -1) {
    if (is_potion (&item_pos)) kid_drink ();
    else if (is_sword (&item_pos)) kid_raise_sword ();
    else {
      item_pos.room = -1; goto no_item;
    }
    return false;
  }

 no_item:
  if (kid->i == 2 && down_key
      && kid->cinertia == 0
      && kid->wait-- <= 0
      && ((kid->f.dir == LEFT && left_key)
          || (kid->f.dir == RIGHT && right_key))) {
    if (! is_colliding (&kid->f, &kid->fo, false))
      kid->f.c.x += (kid->f.dir == LEFT) ? -6 : +6;
    select_frame (kid, kid_couch_frameset, 0);
    return true;
  }

  if (kid->i != 2 || (! down_key && kid->wait-- <= 0))
    kid->i++;

  select_frame (kid, kid_couch_frameset, kid->i);

  if (kid->oaction == kid_climb) kid->fo.dx += 7;
  if (kid->i == 0) kid->cinertia = 2 * inertia;
  if (kid->i > 0 && kid->i < 3) kid->fo.dx -= kid->cinertia;
  if (kid->cinertia > 0) kid->cinertia--;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord m; struct pos np, pm, pml;
  enum confg cm, cml;

  /* fall */
  cm = survey (_m, pos, &kid->f, &m, &pm, &np)->fg;
  prel (&pm, &pml, 0, -1);
  cml = con (&pml)->fg;
  struct loose_floor *l =
    loose_floor_at_pos (prel (&pm, &np, -1, +0));
  if ((cm == NO_FLOOR
       || (l && l->action == FALL_LOOSE_FLOOR && cm == LOOSE_FLOOR))
      && ! (kid->fall && kid->i == 0)
      && ! (kid->f.dir == RIGHT
            && cml == DOOR
            && m.y <= door_grid_tip_y (&pml) - 10)) {
    kid_fall ();
    return false;
  }

  /* wall or door pushes back */
  if ((kid->i == 0 || kid->i > 4)
      && is_colliding (&kid->f, &kid->fo, false)) {
    kid_stabilize_collision ();
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  if (kid->i == 0) update_depressible_floor (kid, -7, -9);
  else if (kid->i == 2) update_depressible_floor (kid, -1, -13);
  else if (kid->i == 5) update_depressible_floor (kid, -19, -20);
  else if (kid->i == 7) update_depressible_floor (kid, -12, -22);
  else if (kid->i == 8) update_depressible_floor (kid, -9, -10);
  else if (kid->i == 11) update_depressible_floor (kid, -6, -12);
  else keep_depressible_floor (kid);
}

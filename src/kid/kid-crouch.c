/*
  kid-crouch.c -- kid crouch module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

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

static bool flow (struct actor *k);
static bool physics_in (struct actor *k);
static void physics_out (struct actor *k);

void
kid_crouch (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_crouch;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

void
kid_crouch_collision (struct actor *k)
{
  kid_haptic (k, KID_HAPTIC_STRONG_COLLISION);
  play_audio (&hit_wall_audio, NULL, k->id);

  k->action = kid_crouch_collision;

  int dx = (k->f.dir == LEFT) ? +50 : -14;
  int dy = +27;
  place_actor (k, &k->ci.kid_p, dx, dy, "KID", "CROUCH", 0);

  /* collision is detected after actor's next frame has been selected */
  k->i--;

  kid_crouch (k);
}

void
kid_crouch_suddenly (struct actor *k)
{
  k->action = kid_crouch_suddenly;
  struct pos pmt;
  survey (_mt, pos, &k->f, NULL, &pmt, NULL);
  int dx = (k->f.dir == LEFT) ? 24 : 18;
  int dy = +27;
  place_actor (k, &pmt, dx, dy, "KID", "CROUCH", 0);
  kid_crouch (k);
}

static bool
flow (struct actor *k)
{
  struct pos ptf, pbf;
  enum tile_fg ctf;

  if (k->oaction != kid_crouch) {
    k->i = -1; k->fall = k->collision = false;
    k->wait = 0;
  }

  if (k->uncrouch_slowly) {
    k->wait = 36;
    k->uncrouch_slowly = false;
  }

  if (k->oaction == kid_climb)
    k->i = 10;

  if (k->oaction == kid_crouch_collision)
    k->collision = true, k->inertia = k->cinertia = 0;

  if (k->oaction == kid_fall) {
    k->fall = true; k->inertia = k->cinertia = 0;
  }

  /* unclimb */
  int dir = (k->f.dir == LEFT) ? +1 : -1;
  survey (_tf, pos, &k->f, NULL, &ptf, NULL);
  ctf = fg (&ptf);
  survey (_bf, pos, &k->f, NULL, &pbf, NULL);
  struct pos ph; prel (&pbf, &ph, +1, dir);
  if (k->i == -1
      && ! k->collision
      && ! k->fall
      && ! k->hit_by_loose_floor
      && ! is_valid_pos (&k->item_pos)
      && is_hangable (&ph, k->f.dir)
      && dist_next_place (&k->f, _tf, pos, 0, true) <= 24
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
    select_actor_frame (k, "KID", "CROUCH", 0);
    return true;
  }

  if (k->i != 2 || (! k->key.down && k->wait-- <= 0))
    k->i++;

  select_actor_frame (k, "KID", "CROUCH", k->i);

  if (k->i > 0 && k->i < 3) k->fo.dx -= k->cinertia;
  if (k->cinertia > 0) {
    request_gamepad_rumble (0.5 - k->cinertia / 6,
                            1.0 / DEFAULT_HZ);

    k->cinertia--;
  }

  return true;
}

static bool
physics_in (struct actor *k)
{
  struct pos pm, pma;
  enum tile_fg cm;

  bool crouch_jump = k->key.down
    && ((k->f.dir == LEFT && k->key.left)
        || (k->f.dir == RIGHT && k->key.right))
    && k->i <= 2;

  /* collision */
  if (k->f.dir == LEFT
      && uncollide (&k->f, &k->fo, _bf, +0, +0, NULL, &k->ci)
      && fg (&k->ci.tile_p) == MIRROR)
    uncollide (&k->f, &k->fo, _bf, +0, +0, &k->fo, &k->ci);
  else if (! is_valid_pos (&k->item_pos))
    uncollide (&k->f, &k->fo, _bf, +0, +0, &k->fo, NULL);
  if (k->key.down && ! crouch_jump) uncollide_static_kid_normal (k);

  /* fall */
  survey (_m, pos, &k->f, NULL, &pm, NULL);
  cm = fg (&pm);
  struct loose_floor *l =
    falling_loose_floor_at_pos (prel (&pm, &pma, -1, +0));
  if ((is_falling (&k->f, _m, +0, +0)
       || (l && l->action == FALL_LOOSE_FLOOR && cm == LOOSE_FLOOR))
      && ! (k->fall && k->i == 0)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  /* place on the ground */
  place_on_the_ground (&k->f, &k->f.c);

  /* depressible floors */
  if (k->i == 0) update_depressible_floor (k, -7, -9);
  else if (k->i == 2) update_depressible_floor (k, -1, -13);
  else if (k->i == 5) update_depressible_floor (k, -19, -20);
  else if (k->i == 7) update_depressible_floor (k, -12, -22);
  else if (k->i == 8) update_depressible_floor (k, -9, -10);
  else if (k->i == 11) update_depressible_floor (k, -6, -12);
  else keep_depressible_floor (k);
}

/*
  kid-raise-sword.c -- kid raise sword module;

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

struct frameset kid_raise_sword_frameset[KID_RAISE_SWORD_FRAMESET_NMEMB];

static void init_kid_raise_sword_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_raise_sword_00, *kid_raise_sword_01,
  *kid_raise_sword_02, *kid_raise_sword_03;

static void
init_kid_raise_sword_frameset (void)
{
  struct frameset frameset[KID_RAISE_SWORD_FRAMESET_NMEMB] =
    {{kid_raise_sword_00,-4,0},{kid_raise_sword_01,+0,0},
     {kid_raise_sword_02,+1,0},{kid_raise_sword_03,-1,0}};

  memcpy (&kid_raise_sword_frameset, &frameset,
          KID_RAISE_SWORD_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_raise_sword (void)
{
  /* bitmaps */
  kid_raise_sword_00 = load_bitmap (KID_RAISE_SWORD_00);
  kid_raise_sword_01 = load_bitmap (KID_RAISE_SWORD_01);
  kid_raise_sword_02 = load_bitmap (KID_RAISE_SWORD_02);
  kid_raise_sword_03 = load_bitmap (KID_RAISE_SWORD_03);

  /* frameset */
  init_kid_raise_sword_frameset ();
}

void
unload_kid_raise_sword (void)
{
  destroy_bitmap (kid_raise_sword_00);
  destroy_bitmap (kid_raise_sword_01);
  destroy_bitmap (kid_raise_sword_02);
  destroy_bitmap (kid_raise_sword_03);
}

void
kid_raise_sword (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_raise_sword;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_raise_sword) k->i = -1, k->wait = 5;

  if (k->i == 0 && k->wait > 0) k->wait--;
  else if (k->i < 3) k->i++;
  else {
    kid_keep_sword (k);
    k->item_pos.room = -1;
    k->has_sword = true;
    return false;
  }

  k->j = 20 + k->i;

  select_frame (k, kid_raise_sword_frameset, k->i);
  select_xframe (&k->xf, sword_frameset, k->j);

  if (k->i == 0 && k->wait < 5) k->fo.dx = 0;

  return true;
}

static bool
physics_in (struct anim *k)
{
  return true;
}

static void
physics_out (struct anim *k)
{
  /* depressible floors */
  keep_depressible_floor (k);

  /* sound */
  if (k->i == 0 && k->wait == 5) {
    if (k->id == current_kid_id) {
      mr.flicker = 8;
      mr.color = get_flicker_raise_sword_color ();
    }
    play_sample (glory_sample, NULL, k->id);
  }

  /* consume sword */
  if (k->i == 0)
    register_con_undo (&undo, &k->item_pos,
                       MIGNORE, MIGNORE, NO_ITEM,
                       false, false, false, false,
                       CHPOS_NONE,
                       "CONSUME SWORD");
}

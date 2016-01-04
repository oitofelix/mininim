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

struct frameset kid_raise_sword_frameset[KID_RAISE_SWORD_FRAMESET_NMEMB];

static void init_kid_raise_sword_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_raise_sword_01, *kid_raise_sword_02,
  *kid_raise_sword_03, *kid_raise_sword_04;

static void
init_kid_raise_sword_frameset (void)
{
  struct frameset frameset[KID_RAISE_SWORD_FRAMESET_NMEMB] =
    {{kid_raise_sword_01,-4,0},{kid_raise_sword_02,+0,0},
     {kid_raise_sword_03,+1,0},{kid_raise_sword_04,-1,0}};

  memcpy (&kid_raise_sword_frameset, &frameset,
          KID_RAISE_SWORD_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_raise_sword (void)
{
  /* bitmaps */
  kid_raise_sword_01 = load_bitmap (KID_RAISE_SWORD_01);
  kid_raise_sword_02 = load_bitmap (KID_RAISE_SWORD_02);
  kid_raise_sword_03 = load_bitmap (KID_RAISE_SWORD_03);
  kid_raise_sword_04 = load_bitmap (KID_RAISE_SWORD_04);

  /* frameset */
  init_kid_raise_sword_frameset ();
}

void
unload_kid_raise_sword (void)
{
  al_destroy_bitmap (kid_raise_sword_01);
  al_destroy_bitmap (kid_raise_sword_02);
  al_destroy_bitmap (kid_raise_sword_03);
  al_destroy_bitmap (kid_raise_sword_04);
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
    video_effect.color = get_flicker_raise_sword_color ();
    start_video_effect (VIDEO_FLICKERING, SECS_TO_VCYCLES (0.5));
    play_sample (glory_sample, k->f.c.room);
  }

  /* consume sword */
  if (k->i == 0) con (&k->item_pos)->ext.item = NO_ITEM;
}

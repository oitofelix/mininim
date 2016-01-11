/*
  kid-sword-defense.c -- kid sword defense module;

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

#include <stdio.h>
#include "prince.h"
#include "kernel/video.h"
#include "kernel/keyboard.h"
#include "engine/anim.h"
#include "engine/physics.h"
#include "engine/door.h"
#include "engine/potion.h"
#include "engine/sword.h"
#include "engine/loose-floor.h"
#include "engine/fight.h"
#include "kid.h"

struct frameset kid_sword_defense_frameset[KID_SWORD_DEFENSE_FRAMESET_NMEMB];

static void init_kid_sword_defense_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_sword_defense_18, *kid_sword_defense_11,
  *kid_sword_defense_12;

static void
init_kid_sword_defense_frameset (void)
{
  struct frameset frameset[KID_SWORD_DEFENSE_FRAMESET_NMEMB] =
    {{kid_sword_defense_18,+0,0},{kid_sword_defense_11,+0,0},
     {kid_sword_defense_12,+0,0}};

  memcpy (&kid_sword_defense_frameset, &frameset,
          KID_SWORD_DEFENSE_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_sword_defense (void)
{
  /* bitmaps */
  kid_sword_defense_18 = load_bitmap (KID_SWORD_DEFENSE_18);
  kid_sword_defense_11 = load_bitmap (KID_SWORD_DEFENSE_11);
  kid_sword_defense_12 = load_bitmap (KID_SWORD_DEFENSE_12);

  /* frameset */
  init_kid_sword_defense_frameset ();
}

void
unload_kid_sword_defense (void)
{
  al_destroy_bitmap (kid_sword_defense_18);
  al_destroy_bitmap (kid_sword_defense_11);
  al_destroy_bitmap (kid_sword_defense_12);
}

void
kid_sword_defense (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_sword_defense;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_sword_defense)
    k->i = -1;

  struct anim *ke = get_anim_by_id (k->enemy_id);
  if (k->i == 2) {
    kid_sword_attack (k);
    return false;
  } else if (k->i == 1 && ke && ke->attack_defended == 2
           && ke->counter_attacked != 2) {
    kid_sword_walkb (k);
    return false;
  } else if (k->i == 1 && ! (ke && ke->attack_defended == 2)) {
    kid_sword_normal (k);
    return false;
  }

  if (k->i == -1) k->j = 28;
  if (k->i == 0) k->j = 14;
  if (k->i == 1) k->j = 15;

  select_frame (k, kid_sword_defense_frameset, k->i + 1);
  select_xframe (&k->xf, sword_frameset, k->j);

  if (k->oaction == kid_sword_attack) k->fo.dx += +8;

  /* if (k->id == 0) */
  /*   printf ("kid_sword_defense: k->i = %i, k->fo.dx = %i\n", */
  /*           k->i, k->fo.dx); */

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct coord nc; struct pos np, pbf, pmbo, pbb;

  /* fall */
  survey (_bf, pos, &k->f, &nc, &pbf, &np);
  survey (_mbo, pos, &k->f, &nc, &pmbo, &np);
  survey (_bb, pos, &k->f, &nc, &pbb, &np);
  if (is_strictly_traversable (&pbf)
      || is_strictly_traversable (&pmbo)
      || is_strictly_traversable (&pbb)) {
    k->xf.b = NULL;
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

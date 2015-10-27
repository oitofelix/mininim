/*
  jaffar.c -- jaffar module;

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

#include <error.h>
#include "kernel/video.h"
#include "anim.h"
#include "jaffar.h"

/* variables */
ALLEGRO_BITMAP *jaffar_normal,
  *jaffar_walk_01, *jaffar_walk_02, *jaffar_walk_03, *jaffar_walk_04, *jaffar_walk_05,
  *jaffar_walk_06, *jaffar_walk_08, *jaffar_walk_09,
  *jaffar_turn_walk_10, *jaffar_turn_walk_11, *jaffar_turn_walk_12,
  *jaffar_turn_walk_13, *jaffar_turn_walk_14, *jaffar_turn_walk_15,
  *jaffar_turn_walk_16, *jaffar_turn_walk_17, *jaffar_turn_walk_18,
  *jaffar_turn_walk_19,
  *jaffar_open_arms_38, *jaffar_open_arms_20,
  *jaffar_raise_arms_21, *jaffar_raise_arms_22, *jaffar_raise_arms_23,
  *jaffar_raise_arms_24, *jaffar_raise_arms_25, *jaffar_raise_arms_26,
  *jaffar_raise_arms_27, *jaffar_raise_arms_28, *jaffar_raise_arms_36,
  *jaffar_raise_arms_37, *jaffar_raise_arms_29,
  *jaffar_lower_arms_30, *jaffar_lower_arms_31, *jaffar_lower_arms_32,
  *jaffar_lower_arms_33, *jaffar_lower_arms_34, *jaffar_lower_arms_35;

void (*draw_jaffar) (void); /* mutable jaffar draw function */
struct anim jaffar; /* jaffar animation object */

void
load_jaffar (void)
{
  jaffar_normal = load_bitmap (JAFFAR_NORMAL);
  jaffar_walk_01 = load_bitmap (JAFFAR_WALK_01);
  jaffar_walk_02 = load_bitmap (JAFFAR_WALK_02);
  jaffar_walk_03 = load_bitmap (JAFFAR_WALK_03);
  jaffar_walk_04 = load_bitmap (JAFFAR_WALK_04);
  jaffar_walk_05 = load_bitmap (JAFFAR_WALK_05);
  jaffar_walk_06 = load_bitmap (JAFFAR_WALK_06);
  jaffar_walk_08 = load_bitmap (JAFFAR_WALK_09);
  jaffar_walk_09 = load_bitmap (JAFFAR_WALK_09);
  jaffar_turn_walk_10 = load_bitmap (JAFFAR_TURN_WALK_10);
  jaffar_turn_walk_11 = load_bitmap (JAFFAR_TURN_WALK_11);
  jaffar_turn_walk_12 = load_bitmap (JAFFAR_TURN_WALK_12);
  jaffar_turn_walk_13 = load_bitmap (JAFFAR_TURN_WALK_13);
  jaffar_turn_walk_14 = load_bitmap (JAFFAR_TURN_WALK_14);
  jaffar_turn_walk_15 = load_bitmap (JAFFAR_TURN_WALK_15);
  jaffar_turn_walk_16 = load_bitmap (JAFFAR_TURN_WALK_16);
  jaffar_turn_walk_17 = load_bitmap (JAFFAR_TURN_WALK_17);
  jaffar_turn_walk_18 = load_bitmap (JAFFAR_TURN_WALK_18);
  jaffar_turn_walk_19 = load_bitmap (JAFFAR_TURN_WALK_19);
  jaffar_open_arms_38 = load_bitmap (JAFFAR_OPEN_ARMS_38);
  jaffar_open_arms_20 = load_bitmap (JAFFAR_OPEN_ARMS_20);
  jaffar_raise_arms_21 = load_bitmap (JAFFAR_RAISE_ARMS_21);
  jaffar_raise_arms_22 = load_bitmap (JAFFAR_RAISE_ARMS_22);
  jaffar_raise_arms_23 = load_bitmap (JAFFAR_RAISE_ARMS_23);
  jaffar_raise_arms_24 = load_bitmap (JAFFAR_RAISE_ARMS_24);
  jaffar_raise_arms_25 = load_bitmap (JAFFAR_RAISE_ARMS_25);
  jaffar_raise_arms_26 = load_bitmap (JAFFAR_RAISE_ARMS_26);
  jaffar_raise_arms_27 = load_bitmap (JAFFAR_RAISE_ARMS_27);
  jaffar_raise_arms_28 = load_bitmap (JAFFAR_RAISE_ARMS_28);
  jaffar_raise_arms_36 = load_bitmap (JAFFAR_RAISE_ARMS_36);
  jaffar_raise_arms_37 = load_bitmap (JAFFAR_RAISE_ARMS_37);
  jaffar_raise_arms_29 = load_bitmap (JAFFAR_RAISE_ARMS_29);
  jaffar_lower_arms_30 = load_bitmap (JAFFAR_LOWER_ARMS_30);
  jaffar_lower_arms_31 = load_bitmap (JAFFAR_LOWER_ARMS_31);
  jaffar_lower_arms_32 = load_bitmap (JAFFAR_LOWER_ARMS_32);
  jaffar_lower_arms_33 = load_bitmap (JAFFAR_LOWER_ARMS_33);
  jaffar_lower_arms_34 = load_bitmap (JAFFAR_LOWER_ARMS_34);
  jaffar_lower_arms_35 = load_bitmap (JAFFAR_LOWER_ARMS_35);
}

void
unload_jaffar (void)
{
  al_destroy_bitmap (jaffar_normal);
  al_destroy_bitmap (jaffar_walk_01);
  al_destroy_bitmap (jaffar_walk_02);
  al_destroy_bitmap (jaffar_walk_03);
  al_destroy_bitmap (jaffar_walk_04);
  al_destroy_bitmap (jaffar_walk_05);
  al_destroy_bitmap (jaffar_walk_06);
  al_destroy_bitmap (jaffar_walk_08);
  al_destroy_bitmap (jaffar_walk_09);
  al_destroy_bitmap (jaffar_turn_walk_10);
  al_destroy_bitmap (jaffar_turn_walk_11);
  al_destroy_bitmap (jaffar_turn_walk_12);
  al_destroy_bitmap (jaffar_turn_walk_13);
  al_destroy_bitmap (jaffar_turn_walk_14);
  al_destroy_bitmap (jaffar_turn_walk_15);
  al_destroy_bitmap (jaffar_turn_walk_16);
  al_destroy_bitmap (jaffar_turn_walk_17);
  al_destroy_bitmap (jaffar_turn_walk_18);
  al_destroy_bitmap (jaffar_turn_walk_19);
  al_destroy_bitmap (jaffar_open_arms_38);
  al_destroy_bitmap (jaffar_open_arms_20);
  al_destroy_bitmap (jaffar_raise_arms_21);
  al_destroy_bitmap (jaffar_raise_arms_22);
  al_destroy_bitmap (jaffar_raise_arms_23);
  al_destroy_bitmap (jaffar_raise_arms_24);
  al_destroy_bitmap (jaffar_raise_arms_25);
  al_destroy_bitmap (jaffar_raise_arms_26);
  al_destroy_bitmap (jaffar_raise_arms_27);
  al_destroy_bitmap (jaffar_raise_arms_28);
  al_destroy_bitmap (jaffar_raise_arms_36);
  al_destroy_bitmap (jaffar_raise_arms_37);
  al_destroy_bitmap (jaffar_raise_arms_29);
  al_destroy_bitmap (jaffar_lower_arms_30);
  al_destroy_bitmap (jaffar_lower_arms_31);
  al_destroy_bitmap (jaffar_lower_arms_32);
  al_destroy_bitmap (jaffar_lower_arms_33);
  al_destroy_bitmap (jaffar_lower_arms_34);
  al_destroy_bitmap (jaffar_lower_arms_35);
}

void
draw_jaffar_normal (void)
{
  draw_jaffar = draw_jaffar_normal;
  jaffar.flip = (jaffar.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  /* coming from walk */
  if (jaffar.frame == jaffar_walk_09)
    draw_anim (&jaffar, jaffar_normal, +0, 0);
  /* comming from lower arms */
  else if (jaffar.frame == jaffar_lower_arms_35)
    draw_anim (&jaffar, jaffar_normal, -2, 0);
  /* coming from normal */
  else if (jaffar.frame == jaffar_normal)
    draw_anim (&jaffar, jaffar_normal, +0, 0);
  else error (-1, 0, "%s: unknown frame (%p)", __func__, jaffar.frame);
}

void
draw_jaffar_walk (void)
{
  draw_jaffar = draw_jaffar_walk;
  jaffar.flip = (jaffar.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  /* comming from turn walk */
  if (jaffar.frame == jaffar_turn_walk_19) {
    draw_anim_inv = true;
    draw_anim (&jaffar, jaffar_walk_01, +17, -1);
    draw_anim_inv = false;
  }
  /* comming from normal */
  else if (jaffar.frame == jaffar_normal)
    draw_anim (&jaffar, jaffar_walk_01, -4, 0);
  /* comming from walk */
  else if (jaffar.frame == jaffar_walk_01)
    draw_anim (&jaffar, jaffar_walk_02, -4, 0);
  else if (jaffar.frame == jaffar_walk_02)
    draw_anim (&jaffar, jaffar_walk_03, -13, 0);
  else if (jaffar.frame == jaffar_walk_03)
    draw_anim (&jaffar, jaffar_walk_04, -1, 0);
  else if (jaffar.frame == jaffar_walk_04)
    draw_anim (&jaffar, jaffar_walk_05, +2, 0);
  else if (jaffar.frame == jaffar_walk_05)
    draw_anim (&jaffar, jaffar_walk_06, -1, 0);
  else if (jaffar.frame == jaffar_walk_06) {
    if (jaffar.repeat > 0) {
      draw_anim (&jaffar, jaffar_walk_01, -2, 0);
      jaffar.repeat--;
    } else
      draw_anim (&jaffar, jaffar_walk_08, -2, 0);
  } else if (jaffar.frame == jaffar_walk_08) {
    draw_anim (&jaffar, jaffar_walk_09, +0, 0);
    draw_jaffar = draw_jaffar_normal;
  } else error (-1, 0, "%s: unknown frame (%p)", __func__, jaffar.frame);
}

void
draw_jaffar_open_arms (void)
{
  draw_jaffar = draw_jaffar_open_arms;
  jaffar.flip = (jaffar.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  /* comming from normal */
  if (jaffar.frame == jaffar_normal)
    draw_anim (&jaffar, jaffar_open_arms_38, -2, 0);
  /* comming from open arms */
  else if (jaffar.frame == jaffar_open_arms_38)
    draw_anim (&jaffar, jaffar_open_arms_20, -5, 0);
  else if (jaffar.frame == jaffar_open_arms_20)
    draw_anim (&jaffar, jaffar_open_arms_20, +0, 0);
  else error (-1, 0, "%s: unknown frame (%p)", __func__, jaffar.frame);
}

void
draw_jaffar_raise_arms (void)
{
  draw_jaffar = draw_jaffar_raise_arms;
  jaffar.flip = (jaffar.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  /* comming from open arms */
  if (jaffar.frame == jaffar_open_arms_20)
    draw_anim (&jaffar, jaffar_raise_arms_21, +0, 0);
  /* comming from raise arms */
  else if (jaffar.frame == jaffar_raise_arms_21)
    draw_anim (&jaffar, jaffar_raise_arms_22, +0, 0);
  else if (jaffar.frame == jaffar_raise_arms_22)
    draw_anim (&jaffar, jaffar_raise_arms_23, +2, 0);
  else if (jaffar.frame == jaffar_raise_arms_23)
    draw_anim (&jaffar, jaffar_raise_arms_24, -1, 0);
  else if (jaffar.frame == jaffar_raise_arms_24)
    draw_anim (&jaffar, jaffar_raise_arms_25, -5, 0);
  else if (jaffar.frame == jaffar_raise_arms_25)
    draw_anim (&jaffar, jaffar_raise_arms_26, +0, 0);
  else if (jaffar.frame == jaffar_raise_arms_26)
    draw_anim (&jaffar, jaffar_raise_arms_27, +9, 0);
  else if (jaffar.frame == jaffar_raise_arms_27)
    draw_anim (&jaffar, jaffar_raise_arms_28, -2, 0);
  else if (jaffar.frame == jaffar_raise_arms_28)
    draw_anim (&jaffar, jaffar_raise_arms_36, +1, 0);
  else if (jaffar.frame == jaffar_raise_arms_36)
    draw_anim (&jaffar, jaffar_raise_arms_37, +0, 0);
  else if (jaffar.frame == jaffar_raise_arms_37)
    draw_anim (&jaffar, jaffar_raise_arms_29, +0, 0);
  else if (jaffar.frame == jaffar_raise_arms_29)
    draw_anim (&jaffar, jaffar_raise_arms_29, +0, 0);
  else error (-1, 0, "%s: unknown frame (%p)", __func__, jaffar.frame);
}

void
draw_jaffar_lower_arms (void)
{
  draw_jaffar = draw_jaffar_lower_arms;
  jaffar.flip = (jaffar.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  /* comming from raise arms */
  if (jaffar.frame == jaffar_raise_arms_29)
    draw_anim (&jaffar, jaffar_lower_arms_30, +2, 0);
  /* comming from lower arms */
  else if (jaffar.frame == jaffar_lower_arms_30)
    draw_anim (&jaffar, jaffar_lower_arms_31, 0, 0);
  else if (jaffar.frame == jaffar_lower_arms_31)
    draw_anim (&jaffar, jaffar_lower_arms_32, -2, 0);
  else if (jaffar.frame == jaffar_lower_arms_32)
    draw_anim (&jaffar, jaffar_lower_arms_33, -2, 0);
  else if (jaffar.frame == jaffar_lower_arms_33)
    draw_anim (&jaffar, jaffar_lower_arms_34, +0, 0);
  else if (jaffar.frame == jaffar_lower_arms_34) {
    draw_anim (&jaffar, jaffar_lower_arms_35, +7, 0);
    draw_jaffar = draw_jaffar_normal;
  } else error (-1, 0, "%s: unknown frame (%p)", __func__, jaffar.frame);
}

void
draw_jaffar_turn_walk (void)
{
  draw_jaffar = draw_jaffar_turn_walk;
  jaffar.flip = (jaffar.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  /* comming from normal */
  if (jaffar.frame == jaffar_normal)
    draw_anim (&jaffar, jaffar_turn_walk_10, -1, +1);
  /* comming from turn walk */
  else if (jaffar.frame == jaffar_turn_walk_10)
    draw_anim (&jaffar, jaffar_turn_walk_11, +1, 0);
  else if (jaffar.frame == jaffar_turn_walk_11)
    draw_anim (&jaffar, jaffar_turn_walk_12, +0, 0);
  else if (jaffar.frame == jaffar_turn_walk_12)
    draw_anim (&jaffar, jaffar_turn_walk_13, -2, 0);
  else if (jaffar.frame == jaffar_turn_walk_13)
    draw_anim (&jaffar, jaffar_turn_walk_14, -1, 0);
  else if (jaffar.frame == jaffar_turn_walk_14)
    draw_anim (&jaffar, jaffar_turn_walk_15, -4, 0);
  else if (jaffar.frame == jaffar_turn_walk_15)
    draw_anim (&jaffar, jaffar_turn_walk_16, +2, 0);
  else if (jaffar.frame == jaffar_turn_walk_16)
    draw_anim (&jaffar, jaffar_turn_walk_17, +7, 0);
  else if (jaffar.frame == jaffar_turn_walk_17)
    draw_anim (&jaffar, jaffar_turn_walk_18, +3, 0);
  else if (jaffar.frame == jaffar_turn_walk_18) {
    draw_anim (&jaffar, jaffar_turn_walk_19, +1, 0);
    jaffar.dir = (jaffar.dir == RIGHT) ? LEFT : RIGHT;
    draw_jaffar = draw_jaffar_walk;
  } else error (-1, 0, "%s: unknown frame (%p)", __func__, jaffar.frame);
}

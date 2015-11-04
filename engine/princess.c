/*
  princess.c -- princess module;

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
#include "princess.h"

/* variables */
ALLEGRO_BITMAP *princess_normal,
  *princess_turn_02, *princess_turn_03, *princess_turn_04, *princess_turn_05,
  *princess_turn_06, *princess_turn_07, *princess_turn_08, *princess_turn_09,
  *princess_step_back_10, *princess_step_back_11, *princess_step_back_12,
  *princess_step_back_13, *princess_step_back_14, *princess_step_back_15,
  *princess_look_down_16, *princess_look_down_17;

struct anim princess; /* princess animation object */

void
load_princess (void)
{
  princess_normal = load_bitmap (PRINCESS_NORMAL);
  princess_turn_02 = load_bitmap (PRINCESS_TURN_02);
  princess_turn_03 = load_bitmap (PRINCESS_TURN_03);
  princess_turn_04 = load_bitmap (PRINCESS_TURN_04);
  princess_turn_05 = load_bitmap (PRINCESS_TURN_05);
  princess_turn_06 = load_bitmap (PRINCESS_TURN_06);
  princess_turn_07 = load_bitmap (PRINCESS_TURN_07);
  princess_turn_08 = load_bitmap (PRINCESS_TURN_08);
  princess_turn_09 = load_bitmap (PRINCESS_TURN_09);
  princess_step_back_10 = load_bitmap (PRINCESS_STEP_BACK_10);
  princess_step_back_11 = load_bitmap (PRINCESS_STEP_BACK_11);
  princess_step_back_12 = load_bitmap (PRINCESS_STEP_BACK_12);
  princess_step_back_13 = load_bitmap (PRINCESS_STEP_BACK_13);
  princess_step_back_14 = load_bitmap (PRINCESS_STEP_BACK_14);
  princess_step_back_15 = load_bitmap (PRINCESS_STEP_BACK_15);
  princess_look_down_16 = load_bitmap (PRINCESS_LOOK_DOWN_16);
  princess_look_down_17 = load_bitmap (PRINCESS_LOOK_DOWN_17);
}

void
unload_princess (void)
{
  al_destroy_bitmap (princess_normal);
  al_destroy_bitmap (princess_turn_02);
  al_destroy_bitmap (princess_turn_03);
  al_destroy_bitmap (princess_turn_04);
  al_destroy_bitmap (princess_turn_05);
  al_destroy_bitmap (princess_turn_06);
  al_destroy_bitmap (princess_turn_07);
  al_destroy_bitmap (princess_turn_08);
  al_destroy_bitmap (princess_turn_09);
  al_destroy_bitmap (princess_step_back_10);
  al_destroy_bitmap (princess_step_back_11);
  al_destroy_bitmap (princess_step_back_12);
  al_destroy_bitmap (princess_step_back_13);
  al_destroy_bitmap (princess_step_back_14);
  al_destroy_bitmap (princess_step_back_15);
  al_destroy_bitmap (princess_look_down_16);
  al_destroy_bitmap (princess_look_down_17);
}

void
draw_princess_normal (void)
{
  princess.draw = draw_princess_normal;
  princess.flip = (princess.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  /* coming from turn */
  if (princess.frame == princess_turn_09)
    draw_anim (&princess, princess_normal, 0, 0);
  /* coming from normal */
  else if (princess.frame == princess_normal)
    draw_anim (&princess, princess_normal, +0, 0);
  else error (-1, 0, "%s: unknown frame (%p)", __func__, princess.frame);
}

void
draw_princess_turn (void)
{
  princess.draw = draw_princess_turn;
  princess.flip = (princess.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  /* comming from normal */
  if (princess.frame == princess_normal)
    draw_anim (&princess, princess_turn_02, +0, 0);
  /* comming from turn */
  else if (princess.frame == princess_turn_02)
    draw_anim (&princess, princess_turn_03, -1, 0);
  else if (princess.frame == princess_turn_03)
    draw_anim (&princess, princess_turn_04, +0, 0);
  else if (princess.frame == princess_turn_04)
    draw_anim (&princess, princess_turn_05, +2, 0);
  else if (princess.frame == princess_turn_05)
    draw_anim (&princess, princess_turn_06, -6, 0);
  else if (princess.frame == princess_turn_06)
    draw_anim (&princess, princess_turn_07, -2, 0);
  else if (princess.frame == princess_turn_07)
    draw_anim (&princess, princess_turn_08, +6, 0);
  else if (princess.frame == princess_turn_08) {
    draw_anim (&princess, princess_turn_09, -2, 0);
    princess.dir = (princess.dir == RIGHT) ? LEFT : RIGHT;
    princess.draw = draw_princess_normal;
  } else error (-1, 0, "%s: unknown frame (%p)", __func__, princess.frame);
}

void
draw_princess_step_back (void)
{
  princess.draw = draw_princess_step_back;
  princess.flip = (princess.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  draw_anim_inv = true;

  /* comming from normal */
  if (princess.frame == princess_normal)
    draw_anim (&princess, princess_step_back_10, -6, 0);
  /* comming from step back */
  else if (princess.frame == princess_step_back_10)
    draw_anim (&princess, princess_step_back_11, -4, 0);
  else if (princess.frame == princess_step_back_11)
    draw_anim (&princess, princess_step_back_12, -1, 0);
  else if (princess.frame == princess_step_back_12)
    draw_anim (&princess, princess_step_back_13, -7, 0);
  else if (princess.frame == princess_step_back_13)
    draw_anim (&princess, princess_step_back_14, -2, 0);
  else if (princess.frame == princess_step_back_14)
    draw_anim (&princess, princess_step_back_15, +1, 0);
  else if (princess.frame == princess_step_back_15)
    draw_anim (&princess, princess_step_back_15, 0, 0);
  else error (-1, 0, "%s: unknown frame (%p)", __func__, princess.frame);

  draw_anim_inv = false;
}

void
draw_princess_look_down (void)
{
  princess.draw = draw_princess_look_down;
  princess.flip = (princess.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  /* comming from step back */
  if (princess.frame == princess_step_back_15)
    draw_anim (&princess, princess_look_down_16, 0, 0);
  /* comming from look down */
  else if (princess.frame == princess_look_down_16)
    draw_anim (&princess, princess_look_down_17, 0, 0);
  else if (princess.frame == princess_look_down_17)
    draw_anim (&princess, princess_look_down_17, 0, 0);
  else error (-1, 0, "%s: unknown frame (%p)", __func__, princess.frame);
}

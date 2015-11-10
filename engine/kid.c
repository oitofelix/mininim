/*
  kid.c -- kid module;

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
#include <error.h>
#include "kernel/video.h"
#include "kernel/audio.h"
#include "kernel/keyboard.h"
#include "anim.h"
#include "physics.h"
#include "level.h"
#include "room.h"
#include "kid.h"

/* variables */
ALLEGRO_BITMAP *kid_normal,
  *kid_start_run_01, *kid_start_run_02, *kid_start_run_03, *kid_start_run_04,
  *kid_start_run_05, *kid_start_run_06, *kid_run_07,
  *kid_run_08, *kid_run_09, *kid_run_10, *kid_run_11,
  *kid_run_12, *kid_run_13, *kid_run_14,
  *kid_turn_01, *kid_turn_02, *kid_turn_03, *kid_turn_04,
  *kid_stabilize_05, *kid_stabilize_06, *kid_stabilize_07, *kid_stabilize_08,
  *kid_stop_run_01, *kid_stop_run_02, *kid_stop_run_03, *kid_stop_run_04,
  *kid_turn_run_05, *kid_turn_run_06, *kid_turn_run_07, *kid_turn_run_08,
  *kid_turn_run_09, *kid_turn_run_10, *kid_turn_run_11, *kid_turn_run_12,
  *kid_turn_run_13,
  *kid_walk_01, *kid_walk_02, *kid_walk_03, *kid_walk_04, *kid_walk_05,
  *kid_walk_06, *kid_walk_07, *kid_walk_08, *kid_walk_09, *kid_walk_10,
  *kid_walk_11, *kid_walk_12,
  *kid_jump_01, *kid_jump_02, *kid_jump_03, *kid_jump_04, *kid_jump_05, *kid_jump_06,
  *kid_jump_07, *kid_jump_08, *kid_jump_09, *kid_jump_10, *kid_jump_11, *kid_jump_12,
  *kid_jump_13, *kid_jump_14, *kid_jump_15, *kid_jump_16, *kid_jump_17, *kid_jump_18,
  *kid_fall_13, *kid_fall_14, *kid_fall_15, *kid_fall_16, *kid_fall_17,
  *kid_couch_01, *kid_couch_02, *kid_couch_03, *kid_couch_04,
  *kid_couch_05, *kid_couch_06, *kid_couch_07, *kid_couch_08,
  *kid_couch_09, *kid_couch_10, *kid_couch_11, *kid_couch_12,
  *kid_couch_13,
  *kid_vjump_01, *kid_vjump_02, *kid_vjump_03, *kid_vjump_04, *kid_vjump_05,
  *kid_vjump_06, *kid_vjump_07, *kid_vjump_08, *kid_vjump_09, *kid_vjump_10,
  *kid_vjump_11, *kid_vjump_12, *kid_vjump_13, *kid_vjump_15, *kid_vjump_16,
  *kid_vjump_17, *kid_vjump_18, *kid_vjump_19,
  *kid_hang_00, *kid_hang_01, *kid_hang_02, *kid_hang_03,
  *kid_hang_04, *kid_hang_05, *kid_hang_06, *kid_hang_07,
  *kid_hang_08, *kid_hang_09, *kid_hang_10, *kid_hang_11,
  *kid_hang_12, *kid_hang_14,
  *kid_climb_01, *kid_climb_02, *kid_climb_03, *kid_climb_04,
  *kid_climb_05, *kid_climb_06, *kid_climb_07, *kid_climb_08,
  *kid_climb_09, *kid_climb_10, *kid_climb_11, *kid_climb_12,
  *kid_climb_13, *kid_climb_14, *kid_climb_15,
  *kid_run_jump_01, *kid_run_jump_02, *kid_run_jump_03,
  *kid_run_jump_04, *kid_run_jump_05, *kid_run_jump_06,
  *kid_run_jump_07, *kid_run_jump_08, *kid_run_jump_09,
  *kid_run_jump_10, *kid_run_jump_11;

ALLEGRO_SAMPLE *step, *hit_ground, *hit_wall, *hang_on_fall;

struct anim kid; /* kid animation object */
bool hang_limit;
bool misstep = false;

void
load_kid (void)
{
  /* bitmap */
  kid_normal = load_bitmap (KID_NORMAL);
  kid_start_run_01 = load_bitmap (KID_START_RUN_01);
  kid_start_run_02 = load_bitmap (KID_START_RUN_02);
  kid_start_run_03 = load_bitmap (KID_START_RUN_03);
  kid_start_run_04 = load_bitmap (KID_START_RUN_04);
  kid_start_run_05 = load_bitmap (KID_START_RUN_05);
  kid_start_run_06 = load_bitmap (KID_START_RUN_06);
  kid_run_07 = load_bitmap (KID_RUN_07);
  kid_run_08 = load_bitmap (KID_RUN_08);
  kid_run_09 = load_bitmap (KID_RUN_09);
  kid_run_10 = load_bitmap (KID_RUN_10);
  kid_run_11 = load_bitmap (KID_RUN_11);
  kid_run_12 = load_bitmap (KID_RUN_12);
  kid_run_13 = load_bitmap (KID_RUN_13);
  kid_run_14 = load_bitmap (KID_RUN_14);
  kid_turn_01 = load_bitmap (KID_TURN_01);
  kid_turn_02 = load_bitmap (KID_TURN_02);
  kid_turn_03 = load_bitmap (KID_TURN_03);
  kid_turn_04 = load_bitmap (KID_TURN_04);
  kid_stabilize_05 = load_bitmap (KID_STABILIZE_05);
  kid_stabilize_06 = load_bitmap (KID_STABILIZE_06);
  kid_stabilize_07 = load_bitmap (KID_STABILIZE_07);
  kid_stabilize_08 = load_bitmap (KID_STABILIZE_08);
  kid_stop_run_01 = load_bitmap (KID_STOP_RUN_01);
  kid_stop_run_02 = load_bitmap (KID_STOP_RUN_02);
  kid_stop_run_03 = load_bitmap (KID_STOP_RUN_03);
  kid_stop_run_04 = load_bitmap (KID_STOP_RUN_04);
  kid_turn_run_05 = load_bitmap (KID_TURN_RUN_05);
  kid_turn_run_06 = load_bitmap (KID_TURN_RUN_06);
  kid_turn_run_07 = load_bitmap (KID_TURN_RUN_07);
  kid_turn_run_08 = load_bitmap (KID_TURN_RUN_08);
  kid_turn_run_09 = load_bitmap (KID_TURN_RUN_09);
  kid_turn_run_10 = load_bitmap (KID_TURN_RUN_10);
  kid_turn_run_11 = load_bitmap (KID_TURN_RUN_11);
  kid_turn_run_12 = load_bitmap (KID_TURN_RUN_12);
  kid_turn_run_13 = load_bitmap (KID_TURN_RUN_13);
  kid_walk_01 = load_bitmap (KID_WALK_01);
  kid_walk_02 = load_bitmap (KID_WALK_02);
  kid_walk_03 = load_bitmap (KID_WALK_03);
  kid_walk_04 = load_bitmap (KID_WALK_04);
  kid_walk_05 = load_bitmap (KID_WALK_05);
  kid_walk_06 = load_bitmap (KID_WALK_06);
  kid_walk_07 = load_bitmap (KID_WALK_07);
  kid_walk_08 = load_bitmap (KID_WALK_08);
  kid_walk_09 = load_bitmap (KID_WALK_09);
  kid_walk_10 = load_bitmap (KID_WALK_10);
  kid_walk_11 = load_bitmap (KID_WALK_11);
  kid_walk_12 = load_bitmap (KID_WALK_12);
  kid_jump_01 = load_bitmap (KID_JUMP_01);
  kid_jump_02 = load_bitmap (KID_JUMP_02);
  kid_jump_03 = load_bitmap (KID_JUMP_03);
  kid_jump_04 = load_bitmap (KID_JUMP_04);
  kid_jump_05 = load_bitmap (KID_JUMP_05);
  kid_jump_06 = load_bitmap (KID_JUMP_06);
  kid_jump_07 = load_bitmap (KID_JUMP_07);
  kid_jump_08 = load_bitmap (KID_JUMP_08);
  kid_jump_09 = load_bitmap (KID_JUMP_09);
  kid_jump_10 = load_bitmap (KID_JUMP_10);
  kid_jump_11 = load_bitmap (KID_JUMP_11);
  kid_jump_12 = load_bitmap (KID_JUMP_12);
  kid_jump_13 = load_bitmap (KID_JUMP_13);
  kid_jump_14 = load_bitmap (KID_JUMP_14);
  kid_jump_15 = load_bitmap (KID_JUMP_15);
  kid_jump_16 = load_bitmap (KID_JUMP_16);
  kid_jump_17 = load_bitmap (KID_JUMP_17);
  kid_jump_18 = load_bitmap (KID_JUMP_18);
  kid_fall_13 = load_bitmap (KID_FALL_13);
  kid_fall_14 = load_bitmap (KID_FALL_14);
  kid_fall_15 = load_bitmap (KID_FALL_15);
  kid_fall_16 = load_bitmap (KID_FALL_16);
  kid_fall_17 = load_bitmap (KID_FALL_17);
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
  kid_vjump_01 = load_bitmap (KID_VJUMP_01);
  kid_vjump_02 = load_bitmap (KID_VJUMP_02);
  kid_vjump_03 = load_bitmap (KID_VJUMP_03);
  kid_vjump_04 = load_bitmap (KID_VJUMP_04);
  kid_vjump_05 = load_bitmap (KID_VJUMP_05);
  kid_vjump_06 = load_bitmap (KID_VJUMP_06);
  kid_vjump_07 = load_bitmap (KID_VJUMP_07);
  kid_vjump_08 = load_bitmap (KID_VJUMP_08);
  kid_vjump_09 = load_bitmap (KID_VJUMP_09);
  kid_vjump_10 = load_bitmap (KID_VJUMP_10);
  kid_vjump_11 = load_bitmap (KID_VJUMP_11);
  kid_vjump_12 = load_bitmap (KID_VJUMP_12);
  kid_vjump_13 = load_bitmap (KID_VJUMP_13);
  kid_vjump_15 = load_bitmap (KID_VJUMP_15);
  kid_vjump_16 = load_bitmap (KID_VJUMP_16);
  kid_vjump_17 = load_bitmap (KID_VJUMP_17);
  kid_vjump_18 = load_bitmap (KID_VJUMP_18);
  kid_vjump_19 = load_bitmap (KID_VJUMP_19);
  kid_hang_00 = load_bitmap (KID_HANG_00);
  kid_hang_01 = load_bitmap (KID_HANG_01);
  kid_hang_02 = load_bitmap (KID_HANG_02);
  kid_hang_03 = load_bitmap (KID_HANG_03);
  kid_hang_04 = load_bitmap (KID_HANG_04);
  kid_hang_05 = load_bitmap (KID_HANG_05);
  kid_hang_06 = load_bitmap (KID_HANG_06);
  kid_hang_07 = load_bitmap (KID_HANG_07);
  kid_hang_08 = load_bitmap (KID_HANG_08);
  kid_hang_09 = load_bitmap (KID_HANG_09);
  kid_hang_10 = load_bitmap (KID_HANG_10);
  kid_hang_11 = load_bitmap (KID_HANG_11);
  kid_hang_12 = load_bitmap (KID_HANG_12);
  kid_hang_14 = load_bitmap (KID_HANG_14);
  kid_climb_01 = load_bitmap (KID_CLIMB_01);
  kid_climb_02 = load_bitmap (KID_CLIMB_02);
  kid_climb_03 = load_bitmap (KID_CLIMB_03);
  kid_climb_04 = load_bitmap (KID_CLIMB_04);
  kid_climb_05 = load_bitmap (KID_CLIMB_05);
  kid_climb_06 = load_bitmap (KID_CLIMB_06);
  kid_climb_07 = load_bitmap (KID_CLIMB_07);
  kid_climb_08 = load_bitmap (KID_CLIMB_08);
  kid_climb_09 = load_bitmap (KID_CLIMB_09);
  kid_climb_10 = load_bitmap (KID_CLIMB_10);
  kid_climb_11 = load_bitmap (KID_CLIMB_11);
  kid_climb_12 = load_bitmap (KID_CLIMB_12);
  kid_climb_13 = load_bitmap (KID_CLIMB_13);
  kid_climb_14 = load_bitmap (KID_CLIMB_14);
  kid_climb_15 = load_bitmap (KID_CLIMB_15);
  kid_run_jump_01 = load_bitmap (KID_RUN_JUMP_01);
  kid_run_jump_02 = load_bitmap (KID_RUN_JUMP_02);
  kid_run_jump_03 = load_bitmap (KID_RUN_JUMP_03);
  kid_run_jump_04 = load_bitmap (KID_RUN_JUMP_04);
  kid_run_jump_05 = load_bitmap (KID_RUN_JUMP_05);
  kid_run_jump_06 = load_bitmap (KID_RUN_JUMP_06);
  kid_run_jump_07 = load_bitmap (KID_RUN_JUMP_07);
  kid_run_jump_08 = load_bitmap (KID_RUN_JUMP_08);
  kid_run_jump_09 = load_bitmap (KID_RUN_JUMP_09);
  kid_run_jump_10 = load_bitmap (KID_RUN_JUMP_10);
  kid_run_jump_11 = load_bitmap (KID_RUN_JUMP_11);

  /* sound */
  step = load_sample (STEP);
  hit_ground = load_sample (HIT_GROUND);
  hit_wall = load_sample (HIT_WALL);
  hang_on_fall = load_sample (HANG_ON_FALL);

  kid.id = &kid;
  kid.c.room = 1;
  /* kid.c.x = 64; */
  kid.c.x = 230;
  kid.c.y = 15 + 63 * 0;
  kid.frame = kid_normal;
  kid.dir = LEFT;
  kid.collision = draw_kid_collision;
  kid.fall = draw_kid_fall;
  kid.ceiling = draw_kid_ceiling;

  kid.draw = draw_kid_normal;
}

void
unload_kid (void)
{
  /* bitmaps */
  al_destroy_bitmap (kid_normal);
  al_destroy_bitmap (kid_start_run_01);
  al_destroy_bitmap (kid_start_run_02);
  al_destroy_bitmap (kid_start_run_03);
  al_destroy_bitmap (kid_start_run_04);
  al_destroy_bitmap (kid_start_run_05);
  al_destroy_bitmap (kid_start_run_06);
  al_destroy_bitmap (kid_run_07);
  al_destroy_bitmap (kid_run_08);
  al_destroy_bitmap (kid_run_09);
  al_destroy_bitmap (kid_run_10);
  al_destroy_bitmap (kid_run_11);
  al_destroy_bitmap (kid_run_12);
  al_destroy_bitmap (kid_run_13);
  al_destroy_bitmap (kid_run_14);
  al_destroy_bitmap (kid_turn_01);
  al_destroy_bitmap (kid_turn_02);
  al_destroy_bitmap (kid_turn_03);
  al_destroy_bitmap (kid_turn_04);
  al_destroy_bitmap (kid_stabilize_05);
  al_destroy_bitmap (kid_stabilize_06);
  al_destroy_bitmap (kid_stabilize_07);
  al_destroy_bitmap (kid_stabilize_08);
  al_destroy_bitmap (kid_stop_run_01);
  al_destroy_bitmap (kid_stop_run_02);
  al_destroy_bitmap (kid_stop_run_03);
  al_destroy_bitmap (kid_stop_run_04);
  al_destroy_bitmap (kid_turn_run_05);
  al_destroy_bitmap (kid_turn_run_06);
  al_destroy_bitmap (kid_turn_run_07);
  al_destroy_bitmap (kid_turn_run_08);
  al_destroy_bitmap (kid_turn_run_09);
  al_destroy_bitmap (kid_turn_run_10);
  al_destroy_bitmap (kid_turn_run_11);
  al_destroy_bitmap (kid_turn_run_12);
  al_destroy_bitmap (kid_turn_run_13);
  al_destroy_bitmap (kid_walk_01);
  al_destroy_bitmap (kid_walk_02);
  al_destroy_bitmap (kid_walk_03);
  al_destroy_bitmap (kid_walk_04);
  al_destroy_bitmap (kid_walk_05);
  al_destroy_bitmap (kid_walk_06);
  al_destroy_bitmap (kid_walk_07);
  al_destroy_bitmap (kid_walk_08);
  al_destroy_bitmap (kid_walk_09);
  al_destroy_bitmap (kid_walk_10);
  al_destroy_bitmap (kid_walk_11);
  al_destroy_bitmap (kid_walk_12);
  al_destroy_bitmap (kid_jump_01);
  al_destroy_bitmap (kid_jump_02);
  al_destroy_bitmap (kid_jump_03);
  al_destroy_bitmap (kid_jump_04);
  al_destroy_bitmap (kid_jump_05);
  al_destroy_bitmap (kid_jump_06);
  al_destroy_bitmap (kid_jump_07);
  al_destroy_bitmap (kid_jump_08);
  al_destroy_bitmap (kid_jump_09);
  al_destroy_bitmap (kid_jump_10);
  al_destroy_bitmap (kid_jump_11);
  al_destroy_bitmap (kid_jump_12);
  al_destroy_bitmap (kid_jump_13);
  al_destroy_bitmap (kid_jump_14);
  al_destroy_bitmap (kid_jump_15);
  al_destroy_bitmap (kid_jump_16);
  al_destroy_bitmap (kid_jump_17);
  al_destroy_bitmap (kid_jump_18);
  al_destroy_bitmap (kid_fall_13);
  al_destroy_bitmap (kid_fall_14);
  al_destroy_bitmap (kid_fall_15);
  al_destroy_bitmap (kid_fall_16);
  al_destroy_bitmap (kid_fall_17);
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
  al_destroy_bitmap (kid_vjump_01);
  al_destroy_bitmap (kid_vjump_02);
  al_destroy_bitmap (kid_vjump_03);
  al_destroy_bitmap (kid_vjump_04);
  al_destroy_bitmap (kid_vjump_05);
  al_destroy_bitmap (kid_vjump_06);
  al_destroy_bitmap (kid_vjump_07);
  al_destroy_bitmap (kid_vjump_08);
  al_destroy_bitmap (kid_vjump_09);
  al_destroy_bitmap (kid_vjump_10);
  al_destroy_bitmap (kid_vjump_11);
  al_destroy_bitmap (kid_vjump_12);
  al_destroy_bitmap (kid_vjump_13);
  al_destroy_bitmap (kid_vjump_15);
  al_destroy_bitmap (kid_vjump_16);
  al_destroy_bitmap (kid_vjump_17);
  al_destroy_bitmap (kid_vjump_18);
  al_destroy_bitmap (kid_vjump_19);
  al_destroy_bitmap (kid_hang_00);
  al_destroy_bitmap (kid_hang_01);
  al_destroy_bitmap (kid_hang_02);
  al_destroy_bitmap (kid_hang_03);
  al_destroy_bitmap (kid_hang_04);
  al_destroy_bitmap (kid_hang_05);
  al_destroy_bitmap (kid_hang_06);
  al_destroy_bitmap (kid_hang_07);
  al_destroy_bitmap (kid_hang_08);
  al_destroy_bitmap (kid_hang_09);
  al_destroy_bitmap (kid_hang_10);
  al_destroy_bitmap (kid_hang_11);
  al_destroy_bitmap (kid_hang_12);
  al_destroy_bitmap (kid_hang_14);
  al_destroy_bitmap (kid_climb_01);
  al_destroy_bitmap (kid_climb_02);
  al_destroy_bitmap (kid_climb_03);
  al_destroy_bitmap (kid_climb_04);
  al_destroy_bitmap (kid_climb_05);
  al_destroy_bitmap (kid_climb_06);
  al_destroy_bitmap (kid_climb_07);
  al_destroy_bitmap (kid_climb_08);
  al_destroy_bitmap (kid_climb_09);
  al_destroy_bitmap (kid_climb_10);
  al_destroy_bitmap (kid_climb_11);
  al_destroy_bitmap (kid_climb_12);
  al_destroy_bitmap (kid_climb_13);
  al_destroy_bitmap (kid_climb_14);
  al_destroy_bitmap (kid_climb_15);
  al_destroy_bitmap (kid_run_jump_01);
  al_destroy_bitmap (kid_run_jump_02);
  al_destroy_bitmap (kid_run_jump_03);
  al_destroy_bitmap (kid_run_jump_04);
  al_destroy_bitmap (kid_run_jump_05);
  al_destroy_bitmap (kid_run_jump_06);
  al_destroy_bitmap (kid_run_jump_07);
  al_destroy_bitmap (kid_run_jump_08);
  al_destroy_bitmap (kid_run_jump_09);
  al_destroy_bitmap (kid_run_jump_10);
  al_destroy_bitmap (kid_run_jump_11);

  /* sounds */
  al_destroy_sample (step);
  al_destroy_sample (hit_ground);
  al_destroy_sample (hit_wall);
  al_destroy_sample (hang_on_fall);
}

void draw_kid_normal ()
{
  kid.draw = draw_kid_normal;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  bool turn = ((kid.dir == RIGHT) && left_key) || ((kid.dir == LEFT) && right_key);
  bool run = ((kid.dir == RIGHT) && right_key) || ((kid.dir == LEFT) && left_key);
  bool walk = ((kid.dir == RIGHT) && right_key && shift_key)
    || ((kid.dir == LEFT) && left_key && shift_key);
  bool jump = ((kid.dir == RIGHT) && right_key && up_key)
    || ((kid.dir == LEFT) && left_key && up_key);
  bool couch = down_key;
  bool vjump = up_key;

  int dc = dist_collision (kid);

  /* static int px = 0; */
  /* static int py = 0; */
  /* if (a_key) px--; */
  /* if (d_key) px++; */
  /* if (w_key) py--; */
  /* if (s_key) py++; */
  /* al_set_target_bitmap (screen); */
  /* al_put_pixel (px, py, al_map_rgb (0, 255, 255)); */

    /* printf ("x = %i, y = %i, floor = %i, place = %i\n", px, py, (py -3) / 63, (px - 15) / 32); */

  /* comming from stabilize */
  if (kid.frame == kid_stabilize_08) {
    if (turn) draw_kid_turn ();
    else draw_anim (&kid, kid_normal, +2, 0);
  }
  /* comming from walk */
  else if (kid.frame == kid_walk_12)
    draw_anim (&kid, kid_normal, -1, 0);
  /* comming from jump */
  else if (kid.frame == kid_jump_18)
    draw_anim (&kid, kid_normal, -4, 0);
  /* comming from couch */
  else if (kid.frame == kid_couch_13)
    draw_anim (&kid, kid_normal, -1, 0);
  /* comming from vjump */
  else if (kid.frame == kid_vjump_19)
    draw_anim (&kid, kid_normal, +2, 0);
  /* comming from normal */
  else if (kid.frame == kid_normal) {
    if (couch) draw_kid_couch ();
    else if (jump) draw_kid_jump ();
    else if (turn) draw_kid_turn ();
    else if (vjump) draw_kid_vjump ();
    else if (walk) draw_kid_walk ();
    else if (run)
      if (dc < 29) draw_kid_walk ();
      else draw_kid_start_run ();
    else {
      kid.frame = kid_normal;
      kid.c.y = 63 * pos (coord_tf (kid)).floor + 15;
      draw_anim (&kid, kid_normal, +0, 0);
    }
  } else
    error (-1, 0, "%s: unknown kid frame (%p)", __func__, kid.frame);
}

bool
is_kid_normal ()
{
  return kid.frame == kid_normal;
}

void draw_kid_walk ()
{
  kid.draw = draw_kid_walk;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  int dc = dist_collision (kid);
  int df = dist_fall (kid);
  int dl = dist_loose_floor (kid);

  kid.fall = NULL;

  /* in the eminence of collision, keep standing */
  if (dc < 3) {
    draw_anim (&kid, kid_normal, +0, 0);
    kid.draw = draw_kid_normal;
    return;
  }

  /* in the eminence of fall or steping into a loose floor,
     misstep */
  if ((df < 3 || dl < 3) && ! misstep) {
    misstep = true;
    draw_kid_misstep ();
    return;
  }

  /* comming from turn */
  if (kid.frame == kid_turn_04)
    draw_anim (&kid, kid_walk_01, +0, 0);
  /* comming from stabilize */
  else if (kid.frame == kid_stabilize_05)
    draw_anim (&kid, kid_walk_01, +2, 0);
  else if (kid.frame == kid_stabilize_06)
    draw_anim (&kid, kid_walk_01, +6, 0);
  else if (kid.frame == kid_stabilize_07)
    draw_anim (&kid, kid_walk_01, +3, 0);
  else if (kid.frame == kid_stabilize_08)
    draw_anim (&kid, kid_walk_01, +0, 0);
  /* comming from normal */
  else if (kid.frame == kid_normal)
    draw_anim (&kid, kid_walk_01, -1, 0);
  /* comming from walk */
  else if (kid.frame == kid_walk_01) {
    if (! misstep)
      if (dc < 9 || df < 9 || dl < 9) draw_kid_walk_min ();
      else if (dc < 14 || df < 14 || dl < 14) draw_kid_walk_short_short ();
      else if (dc < 21 || df < 21 || dl < 21) draw_kid_walk_short ();
      else if (dc < 23 || df < 23 || dl < 23) draw_kid_walk_long ();
      else if (dc < 27 || df < 27 || dl < 27) draw_kid_walk_long_long ();
      else draw_kid_walk_max ();
    else draw_kid_walk_max ();
    misstep = false;
  } else error (-1, 0, "%s: unknown frame (%p)", __func__, kid.frame);

  kid.fall = draw_kid_fall;
}

void
draw_kid_walk_min (void)
{
  static int i = 0;

  kid.draw = draw_kid_walk_min;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  switch (i) {
  case 0: draw_anim (&kid, kid_walk_02, -1, 0); i++; break;
  case 1: draw_anim (&kid, kid_walk_03, +0, 0); i++; break;
  case 2: draw_anim_on_edge (&kid, kid_walk_11, +1, 0); i++; break;
  case 3: draw_anim (&kid, kid_walk_12, +0, 0); i = 0;
    kid.draw = draw_kid_normal;
    kid.collision = draw_kid_collision;
    break;
  }
}

void
draw_kid_walk_short_short (void)
{
  static int i = 0;

  kid.draw = draw_kid_walk_short_short;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  switch (i) {
  case 0: draw_anim (&kid, kid_walk_02, -1, 0); i++; break;
  case 1: draw_anim (&kid, kid_walk_03, +0, 0); i++; break;
  case 2: draw_anim_on_edge (&kid, kid_walk_04, +1, 0); i++; break;
  case 3: draw_anim (&kid, kid_walk_10, +1, 0); i++; break;
  case 4: draw_anim (&kid, kid_walk_11, -1, 0); i++; break;
  case 5: draw_anim (&kid, kid_walk_12, +0, 0); i = 0;
    kid.draw = draw_kid_normal;
    break;
  }
}

void
draw_kid_walk_short (void)
{
  static int i = 0;

  kid.draw = draw_kid_walk_short;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  switch (i) {
  case 0: draw_anim (&kid, kid_walk_02, -1, 0); i++; break;
  case 1: draw_anim (&kid, kid_walk_03, +0, 0); i++; break;
  case 2: draw_anim (&kid, kid_walk_04, -8, 0); i++; break;
  case 3: draw_anim_on_edge (&kid, kid_walk_05, +1, 0); i++; break;
  case 4: draw_anim (&kid, kid_walk_08, +3, 0); i++; break;
  case 5: draw_anim (&kid, kid_walk_09, -1, 0); i++; break;
  case 6: draw_anim (&kid, kid_walk_10, -1, 0); i++; break;
  case 7: draw_anim (&kid, kid_walk_11, -1, 0); i++; break;
  case 8: draw_anim (&kid, kid_walk_12, +0, 0); i = 0;
    kid.draw = draw_kid_normal;
    break;
  }
}

void
draw_kid_walk_long (void)
{
  static int i = 0;

  kid.draw = draw_kid_walk_long;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  switch (i) {
  case 0: draw_anim (&kid, kid_walk_02, -1, 0); i++; break;
  case 1: draw_anim (&kid, kid_walk_03, +0, 0); i++; break;
  case 2: draw_anim (&kid, kid_walk_04, -8, 0); i++; break;
  case 3: draw_anim (&kid, kid_walk_05, -7, 0); i++; break;
  case 4: draw_anim_on_edge (&kid, kid_walk_06, +1, 0); i++; break;
  case 5: draw_anim (&kid, kid_walk_08, +3, 0); i++; break;
  case 6: draw_anim (&kid, kid_walk_09, -1, 0); i++; break;
  case 7: draw_anim (&kid, kid_walk_10, -1, 0); i++; break;
  case 8: draw_anim (&kid, kid_walk_11, -1, 0); i++; break;
  case 9: draw_anim (&kid, kid_walk_12, +0, 0); i = 0;
    kid.draw = draw_kid_normal;
    break;
  }
}

void
draw_kid_walk_long_long (void)
{
  static int i = 0;

  kid.draw = draw_kid_walk_long_long;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  switch (i) {
  case 0: draw_anim (&kid, kid_walk_02, -1, 0); i++; break;
  case 1: draw_anim (&kid, kid_walk_03, +0, 0); i++; break;
  case 2: draw_anim (&kid, kid_walk_04, -8, 0); i++; break;
  case 3: draw_anim (&kid, kid_walk_05, -7, 0); i++; break;
  case 4: draw_anim (&kid, kid_walk_06, -6, 0); i++; break;
  case 5: draw_anim_on_edge (&kid, kid_walk_07, +1, 0); i++; break;
  case 6: draw_anim (&kid, kid_walk_08, +3, 0); i++; break;
  case 7: draw_anim (&kid, kid_walk_09, -1, 0); i++; break;
  case 8: draw_anim (&kid, kid_walk_10, -1, 0); i++; break;
  case 9: draw_anim (&kid, kid_walk_11, -1, 0); i++; break;
  case 10: draw_anim (&kid, kid_walk_12, +0, 0); i = 0;
    kid.draw = draw_kid_normal;
    break;
  }
}

void draw_kid_walk_max ()
{
  static int i = 0;

  if (kid.draw != draw_kid_walk_max) i = 0;

  kid.draw = draw_kid_walk_max;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (is_kid_start_walk ()) kid.fall = NULL;
  else kid.fall = draw_kid_fall;

  switch (i) {
  case 0: draw_anim (&kid, kid_walk_02, -1, 0); i++; break;
  case 1: draw_anim (&kid, kid_walk_03, +0, 0); i++; break;
  case 2: draw_anim (&kid, kid_walk_04, -8, 0); i++; break;
  case 3: draw_anim (&kid, kid_walk_05, -7, 0); i++; break;
  case 4: draw_anim (&kid, kid_walk_06, -6, 0); i++; break;
  case 5: draw_anim (&kid, kid_walk_07, +3, 0); i++; break;
  case 6: draw_anim (&kid, kid_walk_08, -2, 0); i++; break;
  case 7: draw_anim (&kid, kid_walk_09, -1, 0); i++; break;
  case 8: draw_anim (&kid, kid_walk_10, -1, 0); i++; break;
  case 9: draw_anim (&kid, kid_walk_11, -2, 0); i++; break;
  case 10: draw_anim (&kid, kid_walk_12, +0, 0); i = 0;
    kid.draw = draw_kid_normal;
    break;
  }
}

bool
is_kid_start_walk (void) {
  return  kid.frame == kid_walk_01
    || kid.frame == kid_walk_02
    || kid.frame == kid_walk_03
    || kid.frame == kid_walk_04;
}

bool
is_kid_walk (void) {
  return kid.frame == kid_walk_05
    || kid.frame == kid_walk_06
    || kid.frame == kid_walk_07
    || kid.frame == kid_walk_08;
}

bool
is_kid_stop_walk (void) {
  return kid.frame == kid_walk_09
    || kid.frame == kid_walk_10
    || kid.frame == kid_walk_11
    || kid.frame == kid_walk_12;
}

void draw_kid_start_run ()
{
  kid.draw = draw_kid_start_run;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  bool run = (kid.dir == RIGHT) ? right_key : left_key;
  bool turn_run = (kid.dir == RIGHT) ? left_key : right_key;
  bool couch = down_key;
  bool jump = ((kid.dir == RIGHT) && right_key && up_key)
    || ((kid.dir == LEFT) && left_key && up_key);

  if (couch) {
    draw_kid_couch ();
    return;
  }

  if (jump &&
      (kid.frame == kid_start_run_01
       || kid.frame == kid_start_run_02
       || kid.frame == kid_start_run_03)) {
    draw_kid_jump ();
    return;
  }

  /* comming from normal */
  if (kid.frame == kid_normal)
    draw_anim (&kid, kid_start_run_01, -1, 0);
  /* comming from turn */
  else if (kid.frame == kid_turn_04)
    draw_anim (&kid, kid_start_run_01, +0, 0);
  /* comming from stabilize */
  else if (kid.frame == kid_stabilize_05)
    draw_anim (&kid, kid_start_run_01, +2, 0);
  else if (kid.frame == kid_stabilize_06)
    draw_anim (&kid, kid_start_run_01, +6, 0);
  else if (kid.frame == kid_stabilize_07)
    draw_anim (&kid, kid_start_run_01, +4, 0);
  else if (kid.frame == kid_stabilize_08)
    draw_anim (&kid, kid_start_run_01, +0, 0);
  /* comming from start run */
  else if (kid.frame == kid_start_run_01)
    draw_anim (&kid, kid_start_run_02, -1, 0);
  else if (kid.frame == kid_start_run_02)
    draw_anim (&kid, kid_start_run_03, -4, 0);
  else if (kid.frame == kid_start_run_03)
    draw_anim (&kid, kid_start_run_04, -3, 0);
  else if (kid.frame == kid_start_run_04) {
    draw_anim (&kid, kid_start_run_05, -8, 0);
    if (turn_run) kid.draw = draw_kid_stop_run;
  } else if (kid.frame == kid_start_run_05) {
    draw_anim (&kid, kid_start_run_06, -8, 0);
    if (turn_run) kid.draw = draw_kid_stop_run;
  } else if (kid.frame == kid_start_run_06) {
    draw_anim (&kid, kid_run_07, -7, 0);
    kid.draw = run ? draw_kid_run : draw_kid_stop_run;
  } else error (-1, 0, "%s: unknown kid frame (%p)", __func__, kid.frame);
}

bool
is_kid_start_run (void)
{
  return kid.frame == kid_start_run_01
    || kid.frame == kid_start_run_02
    || kid.frame == kid_start_run_03
    || kid.frame == kid_start_run_04
    || kid.frame == kid_start_run_05
    || kid.frame == kid_start_run_06;
}

void draw_kid_stop_run ()
{
  kid.draw = draw_kid_stop_run;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  bool turn_run = (kid.dir == RIGHT) ? left_key : right_key;
  bool couch = down_key;
  if (couch) {
    draw_kid_couch ();
    return;
  }

  int dc = dist_collision (kid);

  /* comming from stop run */
  if (kid.frame == kid_stop_run_01) {
    draw_anim (&kid, kid_stop_run_02, -6, 0);
    play_sample (step);
  } else if (kid.frame == kid_stop_run_02) {
    if (dc < 16) {
      draw_anim (&kid, kid_stop_run_04, (int) -dc + 2, 0);
      kid.draw = turn_run ? draw_kid_turn_run : draw_kid_stabilize;
    } else draw_anim (&kid, kid_stop_run_03, -16, 0);
  } else if (kid.frame == kid_stop_run_03) {
    draw_anim (&kid, kid_stop_run_04, +0, 0);
    kid.draw = turn_run ? draw_kid_turn_run : draw_kid_stabilize;
    play_sample (step);
  } else
    draw_anim (&kid, kid_stop_run_01, +0, 0);
}

bool
is_kid_stop_run (void) {
  return kid.frame == kid_stop_run_01
    || kid.frame == kid_stop_run_02
    || kid.frame == kid_stop_run_03
    || kid.frame == kid_stop_run_04;
}

void draw_kid_run ()
{
  misstep = false;

  kid.draw = draw_kid_run;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  bool stop = ! ((kid.dir == RIGHT) ? right_key : left_key);
  bool couch = down_key;
  bool jump = ((kid.dir == RIGHT) && right_key && up_key)
    || ((kid.dir == LEFT) && left_key && up_key);

  if (couch) {
    draw_kid_couch ();
    return;
  }

  if (jump && kid.frame != kid_run_jump_11) {
    draw_kid_run_jump ();
    return;
  }

  /* comming from turn run */
  if (kid.frame == kid_turn_run_13) {
    draw_anim (&kid, kid_run_13, -4, 0);
    play_sample (step);
  } else if (kid.frame == kid_run_07)
    draw_anim (&kid, kid_run_08, -10, 0);
  else if (kid.frame == kid_run_jump_11)
    draw_anim (&kid, kid_run_07, -15, 0);
  /* comming from run */
  else if (kid.frame == kid_run_08) {
    draw_anim (&kid, kid_run_09, -3, 0);
    play_sample (step);
  }
  else if (kid.frame == kid_run_09)
    draw_anim (&kid, kid_run_10, -5, 0);
  else if (kid.frame == kid_run_10)
    draw_anim (&kid, kid_run_11, -9, 0);
  else if (kid.frame == kid_run_11)
    draw_anim (&kid, kid_run_12, -11, 0);
  else if (kid.frame == kid_run_12) {
    draw_anim (&kid, kid_run_13, -4, 0);
    play_sample (step);
  }
  else if (kid.frame == kid_run_13)
    draw_anim (&kid, kid_run_14, -8, 0);
  else {
    if (kid.frame == kid_run_jump_11)
      play_sample (step);
    draw_anim (&kid, kid_run_07, -9, 0);
  }

  if (stop) kid.draw = draw_kid_stop_run;
}

bool
is_kid_run (void)
{
  return kid.frame == kid_run_07
    || kid.frame == kid_run_08
    || kid.frame == kid_run_09
    || kid.frame == kid_run_10
    || kid.frame == kid_run_11
    || kid.frame == kid_run_12
    || kid.frame == kid_run_13
    || kid.frame == kid_run_14;
}

void draw_kid_turn ()
{
  misstep = false;

  kid.draw = draw_kid_turn;
  kid.flip = (kid.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  bool run = (kid.dir == RIGHT) ? left_key : right_key;
  bool walk = run && shift_key;
  bool jump = ((kid.dir == RIGHT) && left_key && up_key)
    || ((kid.dir == LEFT) && right_key && up_key);
  bool couch = down_key;
  static bool turn = false;
  if (! turn) turn = ((kid.dir == RIGHT) && right_key)
                || ((kid.dir == LEFT) && left_key);

  /* don't fall while turning */
  kid.fall = NULL;

  /* comming from normal */
  if (kid.frame == kid_normal)
    draw_anim (&kid, kid_turn_01, +2, 0);
  /* comming from stabilize */
  else if (kid.frame == kid_stabilize_05)
    draw_anim (&kid, kid_turn_01, +6, 0);
  else if (kid.frame == kid_stabilize_06)
    draw_anim (&kid, kid_turn_01, +11, 0);
  else if (kid.frame == kid_stabilize_07)
    draw_anim (&kid, kid_turn_01, +8, 0);
  else if (kid.frame == kid_stabilize_08)
    draw_anim (&kid, kid_turn_01, +4, 0);
  /* comming from turn */
  else if (kid.frame == kid_turn_04)
    draw_anim (&kid, kid_turn_01, +3, 0);
  else if (kid.frame == kid_turn_01)
    draw_anim (&kid, kid_turn_02, +1, 0);
  else if (kid.frame == kid_turn_02)
    draw_anim (&kid, kid_turn_03, -3, 0);
  else if (kid.frame == kid_turn_03) {
    draw_anim (&kid, kid_turn_04, +0, 0);
    kid.dir = (kid.dir == RIGHT) ? LEFT : RIGHT;
    if (turn) {
      kid.draw = draw_kid_turn;
      turn = false;
    }
    else if (couch) kid.draw = draw_kid_couch;
    else if (jump) kid.draw = draw_kid_jump;
    else if (walk) kid.draw = draw_kid_walk;
    else if (run) kid.draw = draw_kid_start_run;
    else kid.draw = draw_kid_stabilize;
  } else error (-1, 0, "%s: unknown kid frame (%p)", __func__, kid.frame);

  /* retore the normal falling behavior */
  kid.fall = draw_kid_fall;
}

bool
is_kid_turn (void)
{
  return kid.frame == kid_turn_01
    || kid.frame == kid_turn_02
    || kid.frame == kid_turn_03
    || kid.frame == kid_turn_04;
}

void draw_kid_turn_run ()
{
  kid.draw = draw_kid_turn_run;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  /* comming from stop run */
  if (kid.frame == kid_stop_run_04)
    draw_anim (&kid, kid_turn_run_05, -18, 0);
  /* comming from turn run */
  else if (kid.frame == kid_turn_run_05)
    draw_anim (&kid, kid_turn_run_06, -6, 0);
  else if (kid.frame == kid_turn_run_06)
    draw_anim (&kid, kid_turn_run_07, -4, 0);
  else if (kid.frame == kid_turn_run_07)
    draw_anim (&kid, kid_turn_run_08, +2, 0);
  else if (kid.frame == kid_turn_run_08)
    draw_anim (&kid, kid_turn_run_09, -6, 0);
  else if (kid.frame == kid_turn_run_09)
    draw_anim (&kid, kid_turn_run_10, +3, 0);
  else if (kid.frame == kid_turn_run_10)
    draw_anim (&kid, kid_turn_run_11, -1, 0);
  else if (kid.frame == kid_turn_run_11)
    draw_anim (&kid, kid_turn_run_12, +0, 0);
  else if (kid.frame == kid_turn_run_12) {
    draw_anim (&kid, kid_turn_run_13, +4, 0);
    kid.dir = (kid.dir == RIGHT) ? LEFT : RIGHT;
    kid.draw = draw_kid_run;
  } else error (-1, 0, "%s: unknown kid frame (%p)", __func__, kid.frame);
}

bool
is_kid_turn_run (void)
{
  return kid.frame == kid_turn_run_05
    || kid.frame == kid_turn_run_06
    || kid.frame == kid_turn_run_07
    || kid.frame == kid_turn_run_08
    || kid.frame == kid_turn_run_09
    || kid.frame == kid_turn_run_10
    || kid.frame == kid_turn_run_11
    || kid.frame == kid_turn_run_12
    || kid.frame == kid_turn_run_13;
}

void
draw_kid_stabilize (void)
{
  kid.draw = draw_kid_stabilize;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  bool turn = ((kid.dir == RIGHT) && left_key) || ((kid.dir == LEFT) && right_key);
  bool run = ((kid.dir == RIGHT) && right_key) || ((kid.dir == LEFT) && left_key);
  bool walk = ((kid.dir == RIGHT) && right_key && shift_key)
    || ((kid.dir == LEFT) && left_key && shift_key);
  bool jump = ((kid.dir == RIGHT) && right_key && up_key)
    || ((kid.dir == LEFT) && left_key && up_key);
  bool couch = down_key;

  int dc = dist_collision (kid);

  /* don't fall nor collide */
  kid.collision = NULL;
  kid.fall = NULL;

  /* comming from stop run */
  if (kid.frame == kid_stop_run_04)
    draw_anim (&kid, kid_stabilize_05, -5, 0);
  /* comming from turn */
  else if (kid.frame == kid_turn_04)
    draw_anim (&kid, kid_stabilize_05, -3, 0);
  /* comming from stabilize */
  else if (kid.frame == kid_stabilize_05)
    draw_anim (&kid, kid_stabilize_06, -5, 0);
  else if (kid.frame == kid_stabilize_06)
    draw_anim (&kid, kid_stabilize_07, +3, 0);
  else if (kid.frame == kid_stabilize_07) {
    draw_anim (&kid, kid_stabilize_08, +4, 0);
    kid.draw = draw_kid_normal;
  } else draw_anim (&kid, kid_stabilize_05, -5, 0);

  if (couch) {
    /* fix a bug in which the kid would couch with apparent inertia */
    kid.c.x += (kid.dir == LEFT) ? +5 : -5;
    kid.draw = draw_kid_couch;
  } else if (jump) kid.draw = draw_kid_jump;
  else if (turn) kid.draw = draw_kid_turn;
  else if (walk) kid.draw = draw_kid_walk;
  else if (run) {
    if (dc < 32) kid.draw = draw_kid_walk;
    else kid.draw = draw_kid_start_run;
  }

  /* if this function won't be called next, restore the fall and
     collision behavior */
  if (kid.draw != draw_kid_stabilize) {
    kid.collision = draw_kid_collision;
    kid.fall = draw_kid_fall;
  }
}

bool
is_kid_stabilize (void)
{
  return kid.frame == kid_stabilize_05
    || kid.frame == kid_stabilize_06
    || kid.frame == kid_stabilize_07
    || kid.frame == kid_stabilize_08;
}

void
draw_kid_jump (void)
{
  misstep = false;

  kid.draw = draw_kid_jump;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (kid.frame == kid_stabilize_06)
    draw_anim (&kid, kid_jump_01, +6, 0);
  else if (kid.frame == kid_stabilize_07)
    draw_anim (&kid, kid_jump_01, +4, 0);
  else if (kid.frame == kid_stabilize_08)
    draw_anim (&kid, kid_jump_01, +0, 0);
  /* comming from jump */
  else if (kid.frame == kid_jump_01)
    draw_anim (&kid, kid_jump_02, -2, 0);
  else if (kid.frame == kid_jump_02)
    draw_anim (&kid, kid_jump_03, -4, 0);
  else if (kid.frame == kid_jump_03)
    draw_anim (&kid, kid_jump_04, -5, 0);
  else if (kid.frame == kid_jump_04)
    draw_anim (&kid, kid_jump_05, -3, 0);
  else if (kid.frame == kid_jump_05)
    draw_anim (&kid, kid_jump_06, -6, 0);
  else if (kid.frame == kid_jump_06)
    draw_anim (&kid, kid_jump_07, -3, 0);
  else if (kid.frame == kid_jump_07)
    draw_anim (&kid, kid_jump_08, -17, 0);
  else if (kid.frame == kid_jump_08)
    draw_anim (&kid, kid_jump_09, -21, 0);
  else if (kid.frame == kid_jump_09)
    draw_anim (&kid, kid_jump_10, -11, -6);
  else if (kid.frame == kid_jump_10)
    draw_anim (&kid, kid_jump_11, -2, +6);
  else if (kid.frame == kid_jump_11) {
    draw_anim (&kid, kid_jump_12, -9, 0);
    play_sample (step);
  } else if (kid.frame == kid_jump_12)
    draw_anim (&kid, kid_jump_13, +6, 0);
  else if (kid.frame == kid_jump_13)
    draw_anim (&kid, kid_jump_14, -11, 0);
  else if (kid.frame == kid_jump_14) {
    draw_anim (&kid, kid_jump_15, +0, 0);
    play_sample (step);
  } else if (kid.frame == kid_jump_15)
    draw_anim (&kid, kid_jump_16, +0, 0);
  else if (kid.frame == kid_jump_16)
    draw_anim (&kid, kid_jump_17, +1, 0);
  else if (kid.frame == kid_jump_17) {
    draw_anim (&kid, kid_jump_18, +0, 0);
    kid.draw = draw_kid_normal;
  } else
    draw_anim (&kid, kid_jump_01, +2, 0);
}

bool
is_kid_start_jump (void) {
  return kid.frame == kid_jump_01
    || kid.frame == kid_jump_02
    || kid.frame == kid_jump_03
    || kid.frame == kid_jump_04
    || kid.frame == kid_jump_05
    || kid.frame == kid_jump_06
    || kid.frame == kid_jump_07
    || kid.frame == kid_jump_08;
}

bool
is_kid_jump (void) {
  return kid.frame == kid_jump_09
    || kid.frame == kid_jump_10
    || kid.frame == kid_jump_11;
}

bool
is_kid_stop_jump (void) {
  return kid.frame == kid_jump_12
    || kid.frame == kid_jump_13
    || kid.frame == kid_jump_14
    || kid.frame == kid_jump_15
    || kid.frame == kid_jump_16
    || kid.frame == kid_jump_17
    || kid.frame == kid_jump_18;
}

void
draw_kid_collision (void)
{
  static bool first_frame = true;

  kid.draw = draw_kid_collision;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  /* fix bug in which the kid stand in thin air if colliding while
     jumping */
  if (kid.frame == kid_jump_10) kid.c.y += 6;

  /* if in the air (after a jump), but not falling, fall instead of
     standing against the wall */
  if ((is_kid_jump ()
       || is_kid_start_run_jump ()
       || is_kid_run_jump ()
       || is_kid_stop_run_jump ())
      /* && ! is_falling (kid) */
      ) {
    play_sample (hit_wall);
    draw_kid_fall ();
    return;
  }

  /* notice that the following ignore conditions don't nullify the
     collision detection, as the wall continues to push the kid back
     to the acceptable limit.  To truly ignore collision, permitting
     the kid to temporally trespass such limit, set kid.collision to
     NULL */

  /* don't hardly hit the wall, continuing the current action if... */

  /* standing normally */
  if (is_kid_normal ()) {
    draw_kid_normal ();
    return;
  }

  /* falling */
  if (is_kid_fall ()) {
    draw_kid_fall ();
    return;
  }

  /* couching */
  if (is_kid_start_couch ()
      || is_kid_couch ()
      || is_kid_stop_couch ()) {
    /* get away from the wall so the kid doesn't collide with it while
       uncouching */
    if (kid.frame == kid_couch_01)
      kid.c.x += (kid.dir == LEFT) ? +10 : -10;
    draw_kid_couch ();
    return;
  }

  /* stopping a walk */
  if (is_kid_stop_walk ()) {
    draw_kid_walk ();
    return;
  }

  /* turning to run */
  if (is_kid_turn_run ()) {
    draw_kid_turn_run ();
    return;
  }

  /* don't collide recursively */
  kid.collision = NULL;

  /* everything relying on the first frame must be above this */
  first_frame = false;

 /* draw the standard sequence of frames for kid's collision */
 if (kid.frame == kid_stabilize_06)
    draw_anim (&kid, kid_stabilize_07, +3, 0);
 else if (kid.frame == kid_stabilize_07) {
   draw_anim (&kid, kid_stabilize_08, +4, 0);
   kid.draw = draw_kid_normal;
   first_frame = true;
 } else {
   draw_anim (&kid, kid_stabilize_06, -4, 0);
   play_sample (hit_wall);
 }

  /* if this function won't be called next, restore the collision
     behavior */
  if (kid.draw != draw_kid_collision)
    kid.collision = draw_kid_collision;
}

void
draw_kid_fall (void)
{
  static int inertia;
  static int force_floor = -2;
  static int i = 0;
  struct pos p = pos (coord_bf (kid));
  int dx = 0;

  /* hang if a edge is reachable and the shift key is pressed */
  if (is_hangable (kid) && shift_key && ! hang_limit) {
    play_sample (hang_on_fall);
    draw_kid_hang ();
    force_floor = -2;
    return;
  }

  /* if no falling frame were drawn yet */
  if (! is_kid_fall ()) {
    /* reset the inertia */
    inertia = 0;
    /* move horizontally so the kid doesn't fall through the floor */
    if (dist_fall (kid) > -13)
      kid.c.x += (kid.dir == LEFT) ? -6 : 8;
  }

  /* if the kid is turning to run */
  if (is_kid_turn_run ()) {
    /* get no inertia */
    inertia = 0;
    /* force fall from this floor */
    force_floor = p.floor;
    /* revert kid's direction */
    if (kid.frame != kid_turn_run_13)
      kid.dir = (kid.dir == LEFT) ? RIGHT : LEFT;
    /* prevent kid from falling inside walls */
    kid.c.x += (kid.dir == LEFT) ? 8 : -8;
  }

  /* get appropriate inertia  */
  if (is_kid_start_run ()) inertia = 6;
  if (is_kid_run ()) inertia = 6;
  if (is_kid_stop_run ()) inertia = 6;
  if (is_kid_start_walk ()) inertia = 6;
  if (is_kid_walk ()) inertia = 6;
  if (is_kid_stop_walk ()) inertia = 6;
  if (is_kid_jump ()) inertia = 10;
  if (is_kid_run_jump ()) {
    inertia = 10;
    dx = -10;
  }

  /* at each frame reduce the inertia */
  if (inertia > 0) inertia--;

  kid.draw = draw_kid_fall;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  /* if the kid is not falling anymore and at least the first falling
     frame has been drawn (required when falling after jumping against
     a wall) land on ground */

  if (! is_falling (kid) && is_kid_fall ()
      && p.floor != force_floor) {
    kid.c.y = 63 * p.floor + 15;
    kid.c.x += (kid.dir == LEFT) ? -6 : +6;
    kid.frame = kid_normal;
    draw_kid_couch ();
    if (i > 3) play_sample (hit_ground);
    i = 0;
    force_floor = -2;
    kid.just_fall = true;
    return;
  }

  /* comming from fall */
  if (kid.frame == kid_fall_13)
    draw_anim (&kid, kid_fall_14, -inertia, +5);
  else if (kid.frame == kid_fall_14)
    draw_anim (&kid, kid_fall_15, -inertia, +10);
  else if (kid.frame == kid_fall_15)
    draw_anim (&kid, kid_fall_16, -inertia, +11);
  else if (kid.frame == kid_fall_16)
    draw_anim (&kid, kid_fall_17, -inertia, +20);
  else if (kid.frame == kid_fall_17) {
    int speed = +21 + 3 * (i - 5);
    speed = (speed > 33) ? 33 : speed;
    draw_anim (&kid, kid_fall_17, -inertia, speed);
  } else {
    /* this is needed so the kid doesn't fall through the floor when
       hitting a wall */
    int dy = is_falling (kid) ? +17 : -6;
    draw_anim (&kid, kid_fall_13, dx, dy);
  }

  i++;
}

bool
is_kid_fall (void)
{
  return kid.frame == kid_fall_13
    || kid.frame == kid_fall_14
    || kid.frame == kid_fall_15
    || kid.frame == kid_fall_16
    || kid.frame == kid_fall_17;
}

void
draw_kid_couch (void)
{
  misstep = false;
  void (*odraw) (void) = kid.draw;

  kid.draw = draw_kid_couch;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  struct pos p = pos (coord_bf (kid));

  int dir = (kid.dir == LEFT) ? +1 : -1;
  if (! is_kid_start_couch () && ! is_kid_couch ()
      && ! is_kid_stop_couch ()
      && odraw != draw_kid_fall
      && construct_rel (p, 0, dir).fg == NO_FLOOR
      && dist_next_place (kid) > 9) {
    hang_pos = p;
    draw_kid_unclimb ();
    return;
  }

  static int inertia = 0;

  /* at each frame reduce the inertia */
  if (inertia > 0) inertia--;

  /* don't fall if getting up */
  if (is_kid_stop_couch ())
    kid.fall = NULL;
  else kid.fall = draw_kid_fall;

  bool keep_couched = down_key;
  /* if the kid is uncouching and a command is given to couch again,
     do it right away */
  if (keep_couched && is_kid_stop_couch ()) {
    draw_anim (&kid, kid_couch_01, -1, 0);
    return;
  }

  /* comming from couch */
  if (kid.frame == kid_couch_01)
    draw_anim (&kid, kid_couch_02, inertia ? -8 : -5, 0);
  else if (kid.frame == kid_couch_02)
    draw_anim (&kid, kid_couch_03, inertia ? -4 : -1, 0);
  else if (kid.frame == kid_couch_03) {
    if (keep_couched)
      draw_anim (&kid, kid_couch_03, inertia ? -3 : 0, 0);
    else {
      /* kid shouldn't fall while uncouching. */
      kid.fall = NULL;
      draw_anim (&kid, kid_couch_04, -2, 0);
    }
  } else if (kid.frame == kid_couch_04)
    draw_anim (&kid, kid_couch_05, +0, 0);
  else if (kid.frame == kid_couch_05)
    draw_anim (&kid, kid_couch_06, -5, 0);
  else if (kid.frame == kid_couch_06)
    draw_anim (&kid, kid_couch_07, +1, 0);
  else if (kid.frame == kid_couch_07)
    draw_anim (&kid, kid_couch_08, +0, 0);
  else if (kid.frame == kid_couch_08)
    draw_anim (&kid, kid_couch_09, -1, 0);
  else if (kid.frame == kid_couch_09)
    draw_anim (&kid, kid_couch_10, +0, 0);
  else if (kid.frame == kid_couch_10)
    draw_anim (&kid, kid_couch_11, +3, 0);
  else if (kid.frame == kid_couch_11)
    draw_anim (&kid, kid_couch_12, +1, 0);
  else if (kid.frame == kid_couch_12) {
    draw_anim (&kid, kid_couch_13, +4, 0);
    kid.draw = draw_kid_normal;
    kid.just_fall = false;
  } else {
    if (is_kid_start_run ()) inertia = 4;
    else if (is_kid_run ()) inertia = 8;
    else if (is_kid_stop_run ()) inertia = 6;
    else inertia = 0;
    draw_anim (&kid, kid_couch_01, -1, 0);
  }

  /* if this function won't be called next, restore the fall and
     collision behavior */
  if (kid.draw != draw_kid_couch) {
    kid.collision = draw_kid_collision;
    kid.fall = draw_kid_fall;
  }
}

bool
is_kid_start_couch (void)
{
  return kid.frame == kid_couch_01
    || kid.frame == kid_couch_02;
}

bool
is_kid_couch (void)
{
  return kid.frame == kid_couch_03;
}

bool
is_kid_stop_couch (void)
{
  return kid.frame == kid_couch_04
    || kid.frame == kid_couch_05
    || kid.frame == kid_couch_06
    || kid.frame == kid_couch_07
    || kid.frame == kid_couch_08
    || kid.frame == kid_couch_09
    || kid.frame == kid_couch_10
    || kid.frame == kid_couch_11
    || kid.frame == kid_couch_12
    || kid.frame == kid_couch_13;
}

void
draw_kid_vjump (void)
{
  misstep = false;

  static int i = 0;
  static bool hang = false;

  kid.draw = draw_kid_vjump;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  /* don't fall while jumping */
  kid.fall = NULL;

  int dir = (kid.dir == LEFT) ? +1 : -1;
  if (kid.frame == kid_normal
      && dist_next_place (kid) > 16
      && is_hangable_pos (prel (pos (coord_tf (kid)), 0, dir),
                          kid.dir)) {
    to_prev_place_edge (&kid);
    hang = true;
    kid.ceiling = false;
    hang_pos = pos (coord_tf (kid));
  } else if (kid.frame == kid_normal && is_hangable (kid)) {
    to_next_place_edge (&kid);
    hang = true;
    kid.ceiling = false;
  }

  /* comming from normal */
  if (kid.frame == kid_normal)
    draw_anim (&kid, kid_vjump_01, +5, 0);
  /* comming form vjump */
  else if (kid.frame == kid_vjump_01)
    draw_anim (&kid, kid_vjump_02, +0, 0);
  else if (kid.frame == kid_vjump_02)
    draw_anim (&kid, kid_vjump_03, -1, 0);
  else if (kid.frame == kid_vjump_03)
    draw_anim (&kid, kid_vjump_04, +1, 0);
  else if (kid.frame == kid_vjump_04)
    draw_anim (&kid, kid_vjump_05, +0, 0);
  else if (kid.frame == kid_vjump_05)
    draw_anim (&kid, kid_vjump_06, +0, 0);
  else if (kid.frame == kid_vjump_06)
    draw_anim (&kid, kid_vjump_07, +0, 0);
  else if (kid.frame == kid_vjump_07)
    draw_anim (&kid, kid_vjump_08, -2, 0);
  else if (kid.frame == kid_vjump_08)
    draw_anim (&kid, kid_vjump_09, +0, 0);
  else if (kid.frame == kid_vjump_09)
    draw_anim (&kid, kid_vjump_10, -7, 0);
  else if (kid.frame == kid_vjump_10)
    draw_anim (&kid, kid_vjump_11, +0, 0);
  else if (kid.frame == kid_vjump_11)
    draw_anim (&kid, kid_vjump_12, +0, -3);
  else if (kid.frame == kid_vjump_12)
    draw_anim (&kid, kid_vjump_13, +0, -7);
  else if (kid.frame == kid_vjump_13 && i == 0) {
    if (hang) draw_kid_hang ();
    else draw_anim (&kid, kid_vjump_13, +0, -4), i++;
  } else if (kid.frame == kid_vjump_13 && i == 1)
    draw_anim (&kid, kid_vjump_13, +0, -2), i++;
  else if (kid.frame == kid_vjump_13 && i == 2)
    draw_anim (&kid, kid_vjump_13, +0, +0), i++;
  else if (kid.frame == kid_vjump_13 && i == 3)
    draw_anim (&kid, kid_vjump_13, +0, +2), i++;
  else if (kid.frame == kid_vjump_13 && i == 4)
    draw_anim (&kid, kid_vjump_15, -2, +12), i++;
  else if (kid.frame == kid_vjump_15)
    draw_anim (&kid, kid_vjump_16, +1, +2);
  else if (kid.frame == kid_vjump_16)
    draw_anim (&kid, kid_vjump_17, +0, 0);
  else if (kid.frame == kid_vjump_17)
    draw_anim (&kid, kid_vjump_18, -1, 0);
  else if (kid.frame == kid_vjump_18) {
    draw_anim (&kid, kid_vjump_19, -2, 0);
    kid.draw = draw_kid_normal;
  } else error (-1, 0, "%s: unknown frame (%p)", __func__, kid.frame);

  /* if this function won't be called next, restore the fall and
     collision behavior, and reset the frame counter */
  if (kid.draw != draw_kid_vjump) {
    kid.collision = draw_kid_collision;
    kid.fall = draw_kid_fall;
    kid.ceiling = draw_kid_ceiling;
    hang = false;
    i = 0;
  }
}

bool
is_kid_start_vjump (void)
{
  return kid.frame == kid_vjump_01
    || kid.frame == kid_vjump_02
    || kid.frame == kid_vjump_03
    || kid.frame == kid_vjump_04
    || kid.frame == kid_vjump_05
    || kid.frame == kid_vjump_06
    || kid.frame == kid_vjump_07
    || kid.frame == kid_vjump_08
    || kid.frame == kid_vjump_09
    || kid.frame == kid_vjump_10
    || kid.frame == kid_vjump_11;
}

bool
is_kid_vjump (void)
{
  return kid.frame == kid_vjump_12
    || kid.frame == kid_vjump_13
    || kid.frame == kid_vjump_15;
}

bool
is_kid_stop_vjump (void)
{
  return kid.frame == kid_vjump_16
    || kid.frame == kid_vjump_17
    || kid.frame == kid_vjump_18
    || kid.frame == kid_vjump_19;
}

void
draw_kid_ceiling (void)
{
  kid.draw = draw_kid_ceiling;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (kid.frame == kid_vjump_15)
    draw_anim (&kid, kid_vjump_16, +1, +2);
  else if (kid.frame == kid_vjump_16)
    draw_anim (&kid, kid_vjump_17, +0, +0);
  else if (kid.frame == kid_vjump_17)
    draw_anim (&kid, kid_vjump_18, -1, +0);
  else if (kid.frame == kid_vjump_18)
    draw_anim (&kid, kid_vjump_19, -1, +0);
  else if (kid.frame == kid_vjump_19) {
    draw_anim (&kid, kid_normal, +1, +0);
    kid.draw = draw_kid_normal;
  } else {
    struct pos p = pos (coord_br (kid));
    kid.c.y = PLACE_HEIGHT * p.floor;
    draw_anim (&kid, kid_vjump_15, -2, +4);
  }
}

void
draw_kid_misstep (void)
{
  kid.draw = draw_kid_misstep;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  kid.fall = NULL;

  if (kid.frame == kid_walk_01)
    draw_anim (&kid, kid_walk_02, +0, 0);
  else if (kid.frame == kid_walk_02)
    draw_anim (&kid, kid_walk_03, +0, 0);
  else if (kid.frame == kid_walk_03)
    draw_anim (&kid, kid_walk_04, -8, 0);
  else if (kid.frame == kid_walk_04)
    draw_anim (&kid, kid_walk_05, -6, 0);
  else if (kid.frame == kid_walk_05)
    draw_anim (&kid, kid_walk_06, -3, 0);
  else if (kid.frame == kid_walk_06)
    draw_anim (&kid, kid_jump_14, +6, 0);
  else if (kid.frame == kid_jump_14) {
    draw_anim (&kid, kid_couch_10, +7, 0);
    play_sample (step);
  } else if (kid.frame == kid_couch_10)
    draw_anim (&kid, kid_couch_11, +3, 0);
  else if (kid.frame == kid_couch_11)
    draw_anim (&kid, kid_couch_12, +0, 0);
  else if (kid.frame == kid_couch_12)
    draw_anim (&kid, kid_couch_13, +4, 0);
  else if (kid.frame == kid_couch_13) {
    kid.fall = draw_kid_fall;
    kid.draw = draw_kid_normal;
    draw_anim_on_fall_or_loose_floor_edge (&kid, kid_normal, 0, +0);
  } else
    draw_anim (&kid, kid_walk_01, +0, 0);

  /* if this function won't be called next, restore the fall
     behavior */
  if (kid.draw != draw_kid_misstep) kid.fall = draw_kid_fall;
}

void
draw_kid_hang (void)
{
  kid.draw = draw_kid_hang;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  int dir = (kid.dir == LEFT) ? -1 : +1;
  enum construct_fg fg = construct_rel (hang_pos, 0, dir).fg;

  kid.fall = NULL;

  if (kid.frame == kid_hang_14)
    draw_anim (&kid, kid_hang_04, +0, +0);
  else if (kid.frame == kid_hang_04) {
    if (up_key)
      draw_kid_climb ();
    else if (shift_key && fg == WALL)
      draw_kid_hang_wall ();
    else if (shift_key)
      draw_kid_hang_free ();
    else {
      kid.fall = draw_kid_fall;
      if (is_falling (kid)) {
        draw_kid_fall ();
        return;
      }
      draw_anim (&kid, kid_vjump_15, +8, +6);
      kid.draw = draw_kid_vjump;
    }
  } else {
    kid.frame = kid_hang_14;
    int dir = (kid.dir == LEFT) ? 0 : 1;
    kid.c.x = PLACE_WIDTH * (hang_pos.place + dir) + 7;
    kid.c.y = PLACE_HEIGHT * hang_pos.floor - 6;
    draw_anim (&kid, kid_hang_14, +0, +0);
  }

  /* if this function won't be called next, restore the fall
     behavior */
  if (kid.draw != draw_kid_hang)
    kid.fall = draw_kid_fall;
}

void
draw_kid_hang_wall (void)
{
  static int i = 0;

  kid.draw = draw_kid_hang_wall;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  kid.fall = NULL;

  if (! shift_key) {
    i = 0;
    kid.fall = draw_kid_fall;
    if (is_falling (kid)) {
      draw_kid_fall ();
      return;
    }
    kid.frame = kid_vjump_15;
    kid.c.x += (kid.dir == LEFT) ? +8 : -8;;
    kid.c.y = 63 * pos (coord_m (kid)).floor;
    draw_anim (&kid, kid_vjump_15, +0, +0);
    kid.draw = draw_kid_vjump;
    return;
  } if (up_key) {
    draw_kid_climb ();
    i = 0;
    return;
  }

  switch (i) {
  case 0: draw_anim (&kid, kid_hang_05, +1, +0); i++; break;
  case 1: draw_anim (&kid, kid_hang_06, +1, -1); i++; break;
  case 2: draw_anim (&kid, kid_hang_06, +0, +0); i++; break;
  case 3: draw_anim (&kid, kid_hang_05, -1, +1); i++; break;
  case 4: draw_anim (&kid, kid_hang_05, +0, +0); i++; break;
  case 5: draw_anim (&kid, kid_hang_04, -1, +0); i++; break;
  case 6: draw_anim (&kid, kid_hang_04, +0, +0); break;
  }

  /* if this function won't be called next, restore the fall
     behavior */
  if (kid.draw != draw_kid_hang_wall)
    kid.fall = draw_kid_fall;
}

void
draw_kid_hang_free (void)
{
  static int i = 0;
  static int j = 0;

  hang_limit = (j > 1);

  kid.draw = draw_kid_hang_free;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  kid.fall = NULL;

  if (! shift_key || hang_limit) {
    i = 0;
    j = 0;
    kid.fall = draw_kid_fall;
    if (is_falling (kid)) {
      draw_kid_fall ();
      hang_limit = false;
      return;
    }
    kid.frame = kid_vjump_15;
    kid.c.y = 63 * pos (coord_m (kid)).floor;
    draw_anim (&kid, kid_vjump_15, -10, +0);
    kid.draw = draw_kid_vjump;
    hang_limit = false;
    return;
  } if (up_key) {
    draw_kid_climb ();
    i = 0;
    j = 0;
    return;
  }

 next:
  switch (i) {
  case 0: draw_anim (&kid, kid_hang_03, -2, +0); i++; break;
  case 1: draw_anim (&kid, kid_hang_02, -3, -2); i++; break;
  case 2: draw_anim (&kid, kid_hang_01, -6, +0); i++; break;
  case 3: draw_anim (&kid, kid_hang_00, +0, -2); i++; break;
  case 4: i++;
    if (j == 0) draw_anim (&kid, kid_hang_00, +0, +0);
    else goto next; break;
  case 5: i++;
    if (j == 0) draw_anim (&kid, kid_hang_00, +0, +0);
    else goto next; break;
  case 6: draw_anim (&kid, kid_hang_01, +0, +2); i++; break;
  case 7: draw_anim (&kid, kid_hang_02, +6, +0); i++; break;
  case 8: draw_anim (&kid, kid_hang_03, +3, +2); i++; break;
  case 9: draw_anim (&kid, kid_hang_04, +2, +0); i++; break;
  case 10: draw_anim (&kid, kid_hang_05, +3, +0); i++; break;
  case 11: draw_anim (&kid, kid_hang_06, +1, -1); i++; break;
  case 12: draw_anim (&kid, kid_hang_07, +2, -1); i++; break;
  case 13: draw_anim (&kid, kid_hang_08, +0, -2); i++; break;
  case 14: draw_anim (&kid, kid_hang_09, +0, +0); i++; break;
  case 15: draw_anim (&kid, kid_hang_10, +1, -1); i++; break;
  case 16: draw_anim (&kid, kid_hang_11, +0, +0); i++; break;
  case 17: draw_anim (&kid, kid_hang_12, -3, +0); i++; break;
  case 18: draw_anim (&kid, kid_hang_11, +3, +0); i++; break;
  case 19: draw_anim (&kid, kid_hang_10, +0, +0); i++; break;
  case 20: draw_anim (&kid, kid_hang_09, -1, +1); i++; break;
  case 21: draw_anim (&kid, kid_hang_08, +0, +0); i++; break;
  case 22: draw_anim (&kid, kid_hang_07, +0, +2); i++; break;
  case 23: draw_anim (&kid, kid_hang_06, -2, +1); i++; break;
  case 24: draw_anim (&kid, kid_hang_05, -1, +1); i++; break;
  case 25: draw_anim (&kid, kid_hang_04, -3, +0); i = 0; j++; break;
  }

  /* if this function won't be called next, restore the fall
     behavior */
  if (kid.draw != draw_kid_hang_free)
    kid.fall = draw_kid_fall;
}

bool
is_kid_hang (void)
{
  return kid.frame == kid_hang_00
    || kid.frame == kid_hang_01
    || kid.frame == kid_hang_02
    || kid.frame == kid_hang_03
    || kid.frame == kid_hang_04
    || kid.frame == kid_hang_05
    || kid.frame == kid_hang_06
    || kid.frame == kid_hang_07
    || kid.frame == kid_hang_08
    || kid.frame == kid_hang_09
    || kid.frame == kid_hang_10
    || kid.frame == kid_hang_11
    || kid.frame == kid_hang_12
    || kid.frame == kid_hang_14;
}

void
draw_kid_climb (void)
{
  static int i = 0;

  kid.draw = draw_kid_climb;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  kid.fall = NULL;

  switch (i) {
  case 0:
    hang_pos = npos (hang_pos);
    kid.frame = kid_climb_01;
    int dir = (kid.dir == LEFT) ? 0 : 1;
    kid.c.x = PLACE_WIDTH * (hang_pos.place + dir) + 9;
    kid.c.y = PLACE_HEIGHT * hang_pos.floor - 9;
    draw_anim (&kid, kid_climb_01, +0, +0); i++; break;
  case 1: draw_anim (&kid, kid_climb_02, -2, -9); i++; break;
  case 2: draw_anim (&kid, kid_climb_03, -2, -5); i++; break;
  case 3: draw_anim (&kid, kid_climb_04, -7, -6); i++; break;
  case 4: draw_anim (&kid, kid_climb_05, -5, -4); i++; break;
  case 5: draw_anim (&kid, kid_climb_06, -2, -5); i++; break;
  case 6: draw_anim (&kid, kid_climb_07, -1, -5); i++; break;
  case 7: draw_anim (&kid, kid_climb_08, -4, -8); i++; break;
  case 8: draw_anim (&kid, kid_climb_09, +0, -4); i++; break;
  case 9: draw_anim (&kid, kid_climb_10, +0, -1); i++; break;
  case 10: draw_anim (&kid, kid_climb_11, -3, -4); i++; break;
  case 11: draw_anim (&kid, kid_climb_12, +1, +0); i++; break;
  case 12: draw_anim (&kid, kid_climb_13, +0, +0); i++; break;
  case 13: draw_anim (&kid, kid_climb_14, -1, +0); i++; break;
  case 14: draw_anim (&kid, kid_climb_15, +0, +0); i++; break;
  case 15: draw_anim (&kid, kid_couch_12, +8, +0); i++; break;
  case 16: draw_anim (&kid, kid_couch_13, +4, +0); i++; break;
  case 17: draw_anim (&kid, kid_normal, -3, +0); i = 0;
    kid.draw = draw_kid_normal;
    break;
  }

  if (kid.draw != draw_kid_climb) {
    kid.fall = draw_kid_fall;
    i = 0;
  }

}

bool
is_kid_start_climb (void)
{
  return kid.frame == kid_climb_01
    || kid.frame == kid_climb_02;
}

bool
is_kid_climb (void)
{
  return kid.frame == kid_climb_03
    || kid.frame == kid_climb_04
    || kid.frame == kid_climb_05
    || kid.frame == kid_climb_06
    || kid.frame == kid_climb_07
    || kid.frame == kid_climb_08
    || kid.frame == kid_climb_09
    || kid.frame == kid_climb_10;
}

bool
is_kid_stop_climb (void)
{
  return kid.frame == kid_climb_11
    || kid.frame == kid_climb_12
    || kid.frame == kid_climb_13
    || kid.frame == kid_climb_14
    || kid.frame == kid_climb_15;
}

void
draw_kid_unclimb (void)
{
  static int i = 0;

  kid.draw = draw_kid_unclimb;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  kid.fall = NULL;

  switch (i) {
  case 0:
    kid.frame = kid_climb_14;
    kid.c.x = PLACE_WIDTH * hang_pos.place + 18;
    kid.c.y = PLACE_HEIGHT * hang_pos.floor + 25 ;
    draw_anim (&kid, kid_climb_14, +0, +0); i++; break;
  case 1: draw_anim (&kid, kid_climb_13, +1, +0); i++; break;
  case 2: draw_anim (&kid, kid_climb_12, +0, +0); i++; break;
  case 3: draw_anim (&kid, kid_climb_11, -1, +0); i++; break;
  case 4: draw_anim (&kid, kid_climb_10, +3, +4); i++; break;
  case 5: draw_anim (&kid, kid_climb_09, +0, +1); i++; break;
  case 6: draw_anim (&kid, kid_climb_08, +0, +4); i++; break;
  case 7: draw_anim (&kid, kid_climb_07, +4, +8); i++; break;
  case 8: draw_anim (&kid, kid_climb_06, +1, +5); i++; break;
  case 9: draw_anim (&kid, kid_climb_05, +2, +5); i++; break;
  case 10: draw_anim (&kid, kid_climb_04, +5, +4); i++; break;
  case 11: draw_anim (&kid, kid_climb_03, +7, +6); i++; break;
  case 12: draw_anim (&kid, kid_climb_02, +2, +5); i++; break;
  case 13: draw_anim (&kid, kid_climb_01, +2, +9); i++; break;
  case 14: draw_anim (&kid, kid_hang_04, -5, +3); i = 0;
    kid.draw = draw_kid_hang;
    hang_pos = npos (prel (hang_pos, +1, (kid.dir == LEFT) ? +1 : -1));
    break;
  }

  if (kid.draw != draw_kid_unclimb) {
    kid.fall = draw_kid_fall;
    i = 0;
  }

}

void
draw_kid_run_jump (void)
{
  static int i = 0;

  if (kid.draw != draw_kid_run_jump) i = 0;

  kid.draw = draw_kid_run_jump;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (kid.frame == kid_run_jump_03
      || kid.frame == kid_run_jump_04
      || kid.frame == kid_run_jump_05
      || kid.frame == kid_run_jump_06
      || kid.frame == kid_run_jump_07
      || kid.frame == kid_run_jump_08) kid.fall = NULL;
  else kid.fall = draw_kid_fall;

  switch (i) {
  case 0: draw_anim (&kid, kid_run_jump_01, -10, +0); i++;
    play_sample (step); break;
  case 1: draw_anim (&kid, kid_run_jump_02, -11, +0); i++; break;
  case 2: draw_anim (&kid, kid_run_jump_03, -13, +0); i++; break;
  case 3: draw_anim (&kid, kid_run_jump_04, -7, +0); i++; break;
  case 4: draw_anim (&kid, kid_run_jump_05, -12, +0); i++;
    play_sample (step); break;
  case 5: draw_anim (&kid, kid_run_jump_06, -15, +0); i++; break;
  case 6: draw_anim (&kid, kid_run_jump_07, -29, -3); i++; break;
  case 7: draw_anim (&kid, kid_run_jump_08, -17, -9); i++; break;
  case 8: draw_anim (&kid, kid_run_jump_09, -18, -2); i++; break;
  case 9: draw_anim (&kid, kid_run_jump_10, -10, +11); i++; break;
  case 10:
    draw_anim (&kid, kid_run_jump_11, -8, +3); i = 0;
    kid.draw = draw_kid_run;
    break;
  }

  kid.fall = draw_kid_fall;
}

bool
is_kid_start_run_jump (void) {
  return kid.frame == kid_run_jump_01
    ||  kid.frame == kid_run_jump_02
    ||  kid.frame == kid_run_jump_03
    ||  kid.frame == kid_run_jump_04
    ||  kid.frame == kid_run_jump_05
    ||  kid.frame == kid_run_jump_06
    ||  kid.frame == kid_run_jump_07
    ||  kid.frame == kid_run_jump_08;
}

bool
is_kid_run_jump () {
  return kid.frame == kid_run_jump_09
    || kid.frame == kid_run_jump_10;
}

bool
is_kid_stop_run_jump () {
  return kid.frame == kid_run_jump_11;
}

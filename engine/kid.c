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
#include "door.h"
#include "kid.h"
#include "loose-floor.h"

struct anim kid;
struct survey kids, kidsf;
bool hang_limit;
bool misstep = false;
static int inertia = 0;
static bool critical_edge = false;
static bool turn = false;

static struct frameset hang_frameset[13];
static struct frameset climb_frameset[15];

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

  /* shared framesets */
  init_climb_frameset ();
  init_hang_frameset ();

  /* kid himself */
  kid.id = &kid;
  kid.frame = kid_normal;
  kid.dir = LEFT;
  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;
  kid.fall = draw_kid_fall;
  kid.ceiling = draw_kid_ceiling;
  kid.draw = draw_kid_normal;

  place_kid (7, 0, 8);
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

void
place_kid (int room, int floor, int place)
{
  kid.c.room = room;
  kid.c.x = PLACE_WIDTH * place + 15;
  kid.c.y = PLACE_HEIGHT * floor + 15;
}

void
draw_kid_normal (void)
{
  kid.draw = draw_kid_normal;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (is_falling (kid)) {
    draw_kid_fall ();
    return;
  }

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
    draw_anim (&kid, kid_normal, -2, 0);
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

void
draw_kid_walk (void)
{
  void (*odraw) (void) = kid.draw;
  kid.draw = draw_kid_walk;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  static int i = 0;
  if (odraw != draw_kid_walk) i = 0;

  static bool walk_0 = false, walk_1 = false,
    walk_2 = false, walk_3 = false, walk_4 = false;

  int dc, df, dl;

  if (i == 0) {
    dc = dist_kid_collision ();
    df = dist_kid_construct (NO_FLOOR);
    dl = dist_kid_construct (LOOSE_FLOOR);

    if (dc < 4) {
      draw_anim (&kid, kid_normal, +0, 0);
      kid.draw = draw_kid_normal;
      return;
    }

    if (! misstep) {
      if (df < 4 || dl < 4) {
        misstep = true;
        draw_kid_misstep ();
        return;
      } else if (dc < 9 || df < 9 || dl < 9) walk_0 = true;
      else if (dc < 14 || df < 14 || dl < 14) walk_1 = true;
      else if (dc < 21 || df < 21 || dl < 21) walk_2 = true;
      else if (dc < 23 || df < 23 || dl < 23) walk_3 = true;
      else if (dc < 27 || df < 27 || dl < 27) walk_4 = true;
    }
  }

  struct frameset frameset[12] =
    {{kid_walk_01,-1,0},{kid_walk_02,-1,0},{kid_walk_03,+0,0},
     {kid_walk_04,-8,0},{kid_walk_05,-7,0},{kid_walk_06,-6,0},
     {kid_walk_07,+3,0},{kid_walk_08,-2,0},{kid_walk_09,-1,0},
     {kid_walk_10,-1,0},{kid_walk_11,-2,0},{kid_walk_12,+0,0}};

  ALLEGRO_BITMAP *frame = frameset[i].frame;
  int dx = frameset[i].dx;
  int dy = frameset[i].dy;

  if (((i == 10) && walk_0)
      || ((i == 3) && walk_1)
      || ((i == 4) && walk_2)
      || ((i == 5) && walk_3)
      || ((i == 6) && walk_4)) {
    dx = +1;
    place_kid_on_collision_edge (frame)
      || place_kid_on_construct_edge (frame, LOOSE_FLOOR)
      || place_kid_on_construct_edge (frame, NO_FLOOR);
  }

  if (walk_1) {
    if (i == 9) dx = +1;
    if (i == 10) dx = -1;
  }

  if (walk_2 || walk_3 || walk_4) {
    if (i == 7) dx = +3;
    if (i == 10) dx = -1;
  }

  if (i == 0) {
    if (kid.frame == kid_turn_04) dx = +0;
    if (kid.frame == kid_stabilize_05) dx = +2;
    if (kid.frame == kid_stabilize_06) dx = +6;
    if (kid.frame == kid_stabilize_07) dx = +3;
    if (kid.frame == kid_stabilize_08) dx = +0;
  }

  /* fall */
  if (! walk_0 && ! walk_1 && ! walk_2
      && ! walk_3 && ! walk_4
      && ((i < 6 && kids.cbb == NO_FLOOR)
          || (i >= 6 && kids.cmbo == NO_FLOOR))) {
    draw_kid_fall ();
    i =0; return;
  }

  kid.back_collision = NULL;
  kid.collision = NULL;
  kid.fall = NULL;

  draw_anim (&kid, frame, dx, dy);

  kid.fall = draw_kid_fall;
  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;

  if (i == 2 && walk_0) i = 10;
  else if (i == 3 && walk_1) i = 9;
  else if (i == 4 && walk_2) i = 7;
  else if (i == 5 && walk_3) i = 7;
  else if (i == 6 && walk_4) i = 7;
  else if (i < 11) i++;
  else {
    if (walk_0 || walk_1 || walk_2 || walk_3 || walk_4) {
      kid.c.x += (kid.dir == LEFT) ? +10 : -10;
      place_kid_on_collision_edge (kid_normal)
        || place_kid_on_construct_edge (kid_normal, LOOSE_FLOOR)
        || place_kid_on_construct_edge (kid_normal, NO_FLOOR);
      walk_0 = walk_1 = walk_2 = walk_3 = walk_4 = false;
    }
    kid.draw = draw_kid_normal;
    i = 0;
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

void
draw_kid_start_run (void)
{
  void (*odraw) (void) = kid.draw;
  kid.draw = draw_kid_start_run;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
  misstep = false;
  inertia = 4;

  static int i = 0;
  if (odraw != draw_kid_start_run) i = 0;

  struct frameset frameset[6] =
    {{kid_start_run_01,-1,0},{kid_start_run_02,-1,0},
     {kid_start_run_03,-4,0},{kid_start_run_04,-3,0},
     {kid_start_run_05,-8,0},{kid_start_run_06,-8,0}};

  ALLEGRO_BITMAP *frame = frameset[i].frame;
  int dx = frameset[i].dx;
  int dy = frameset[i].dy;

  if (kid.frame == kid_turn_04) dx = +0;
  if (kid.frame == kid_stabilize_05) dx = +2;
  if (kid.frame == kid_stabilize_06) dx = +6;
  if (kid.frame == kid_stabilize_07) dx = +4;
  if (kid.frame == kid_stabilize_08) dx = +0;

  bool run = (kid.dir == RIGHT) ? right_key : left_key;
  bool turn_run = (kid.dir == RIGHT) ? left_key : right_key;
  bool couch = down_key;
  bool jump = ((kid.dir == RIGHT) && right_key && up_key)
    || ((kid.dir == LEFT) && left_key && up_key);

  /* fall */
  if (kids.ctf == NO_FLOOR) {
    draw_kid_fall ();
    i = 0; return;
  }

  /* collision */
  if (is_kid_colliding (-dx)) {
    draw_kid_stabilize_collision ();
    i = 0; return;
  }

  if (couch) {
    draw_kid_couch ();
    i = 0;
    return;
  }

  if (jump && i < 3) {
    draw_kid_jump ();
    i = 0;
    return;
  }

  kid.back_collision = NULL;
  kid.collision = NULL;
  kid.fall = NULL;

  draw_anim (&kid, frame, dx, dy);

  kid.fall = draw_kid_fall;
  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;

  if ((i == 3 || i ==4) && turn_run) {
    kid.draw = draw_kid_stop_run;
    i = 0;
  }

  if (i < 5) i++;
  else {
    kid.draw = run ? draw_kid_run : draw_kid_stop_run;
    i = 0;
  }
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

void
draw_kid_stop_run (void)
{
  void (*odraw) (void) = kid.draw;
  kid.draw = draw_kid_stop_run;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
  inertia = 6;

  static int i = 0;
  if (odraw != draw_kid_stop_run) i = 0;

  struct frameset frameset[4] =
    {{kid_stop_run_01,+0,0},{kid_stop_run_02,-6,0},
     {kid_stop_run_03,-16,0},{kid_stop_run_04,+0,0}};

  int dc = dist_kid_collision ();

  if (i == 2 && dc < 16) i = 3;

  ALLEGRO_BITMAP *frame = frameset[i].frame;
  int dx = frameset[i].dx;
  int dy = frameset[i].dy;

  bool turn_run = (kid.dir == RIGHT) ? left_key : right_key;
  bool couch = down_key;

  /* fall */
  if (kids.ctf == NO_FLOOR) {
    draw_kid_fall ();
    i = 0; return;
  }

  /* collision */
  if (is_kid_colliding (-dx)) {
    draw_kid_stabilize_collision ();
    i = 0; return;
  }

  if (couch) {
    draw_kid_couch ();
    return;
  }

  if (i == 1 || i == 3) play_sample (step);
  if (i == 2 && dc < 16) dx = -dc + 2;

  kid.back_collision = NULL;
  kid.collision = NULL;
  kid.fall = NULL;

  draw_anim (&kid, frame, dx, dy);

  kid.fall = draw_kid_fall;
  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;

  if (i < 3) i++;
  else {
    i = 0;
    kid.draw = turn_run ? draw_kid_turn_run : draw_kid_stabilize;
  }
}

bool
is_kid_stop_run (void) {
  return kid.frame == kid_stop_run_01
    || kid.frame == kid_stop_run_02
    || kid.frame == kid_stop_run_03
    || kid.frame == kid_stop_run_04;
}

void
draw_kid_run (void)
{
  void (*odraw) (void) = kid.draw;
  kid.draw = draw_kid_run;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
  inertia = 8;

  static int i = 0;
  if (odraw != draw_kid_run) i = 0;

  struct frameset frameset[8] =
    {{kid_run_07,-9,0},{kid_run_08,-10,0},{kid_run_09,-3,0},
     {kid_run_10,-5,0},{kid_run_11,-9,0},{kid_run_12,-11,0},
     {kid_run_13,-4,0},{kid_run_14,-8,0}};

  if (kid.frame == kid_turn_run_13) i = 6;

  ALLEGRO_BITMAP *frame = frameset[i].frame;
  int dx = frameset[i].dx;
  int dy = frameset[i].dy;

  bool stop = ! ((kid.dir == RIGHT) ? right_key : left_key);
  bool couch = down_key;
  bool jump = ((kid.dir == RIGHT) && right_key && up_key)
    || ((kid.dir == LEFT) && left_key && up_key);

  /* fall */
  if (kids.ctf == NO_FLOOR) {
    draw_kid_fall ();
    i = 0; return;
  }

  /* collision */
  if (is_kid_colliding (-dx)) {
    draw_kid_stabilize_collision ();
    i = 0; return;
  }

  if (couch) {
    draw_kid_couch ();
    i = 0; return;
  }

  if (jump && kid.frame != kid_run_jump_11) {
    draw_kid_run_jump ();
    i = 0; return;
  }

  if (stop) {
    draw_kid_stop_run ();
    i = 0; return;
  }

  if (kid.frame == kid_start_run_06) dx = -7;
  if (kid.frame == kid_turn_run_13) dx = -4;
  if (kid.frame == kid_run_jump_11) dx = -15, play_sample (step);
  if (i == 2 || i == 6) play_sample (step);

  kid.back_collision = NULL;
  kid.collision = NULL;
  kid.fall = NULL;

  draw_anim (&kid, frame, dx, dy);

  kid.fall = draw_kid_fall;
  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;

  if (i < 7) i++;
  else i = 0;
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

void
draw_kid_turn (void)
{
  void (*odraw) (void) = kid.draw;
  kid.draw = draw_kid_turn;
  kid.flip = (kid.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;
  misstep = false;

  static int i = 0;
  if (odraw != draw_kid_turn) i = 0;

  struct frameset frameset[4] =
    {{kid_turn_01,+2,0},{kid_turn_02,+1,0},{kid_turn_03,-3,0},
     {kid_turn_04,+0,+0}};

  ALLEGRO_BITMAP *frame = frameset[i].frame;
  int dx = frameset[i].dx;
  int dy = frameset[i].dy;

  if (kid.frame == kid_stabilize_05) dx = +6;
  if (kid.frame == kid_stabilize_06) dx = +10;
  if (kid.frame == kid_stabilize_07) dx = +8;
  if (kid.frame == kid_stabilize_08) dx = +4;
  if (kid.frame == kid_turn_04) dx = +3;

  if (! turn)
    turn = ((kid.dir == RIGHT) && right_key)
      || ((kid.dir == LEFT) && left_key);
  bool run = (kid.dir == RIGHT) ? left_key : right_key;
  bool walk = run && shift_key;
  bool jump = ((kid.dir == RIGHT) && left_key && up_key)
    || ((kid.dir == LEFT) && right_key && up_key);
  bool couch = down_key;

  kid.collision = NULL;
  kid.back_collision = NULL;
  kid.fall = NULL;

  draw_anim (&kid, frame, dx, dy);

  kid.fall = draw_kid_fall;
  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;

  if (i < 3) i++;
  else if (i == 3) {
    i = 0;
    kid.dir = (kid.dir == RIGHT) ? LEFT : RIGHT;
    if (turn) kid.draw = draw_kid_turn, turn = false;
    else if (couch) kid.draw = draw_kid_couch;
    else if (jump) kid.draw = draw_kid_jump;
    else if (walk) kid.draw = draw_kid_walk;
    else if (run) kid.draw = draw_kid_start_run;
    else kid.draw = draw_kid_stabilize;
  }
}

bool
is_kid_turn (void)
{
  return kid.frame == kid_turn_01
    || kid.frame == kid_turn_02
    || kid.frame == kid_turn_03
    || kid.frame == kid_turn_04;
}

void
draw_kid_turn_run (void)
{
  void (*odraw) (void) = kid.draw;
  kid.draw = draw_kid_turn_run;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  static int i = 0;
  if (odraw != draw_kid_turn_run) i = 0;

  struct frameset frameset[9] =
    {{kid_turn_run_05,-18,0},{kid_turn_run_06,-6,0},{kid_turn_run_07,-4,0},
     {kid_turn_run_08,+2,0},{kid_turn_run_09,-6,0},{kid_turn_run_10,+3,0},
     {kid_turn_run_11,-1,0},{kid_turn_run_12,+0,0},{kid_turn_run_13,+4,0}};

  ALLEGRO_BITMAP *frame = frameset[i].frame;
  int dx = frameset[i].dx;
  int dy = frameset[i].dy;

  /* fall */
  if (kids.ctf == NO_FLOOR) {
    draw_kid_fall ();
    i = 0; return;
  }

  /* collision */
  if (is_kid_colliding (-dx)) kid_to_collision_edge (-dx);

  kid.back_collision = NULL;
  kid.collision = NULL;
  kid.fall = NULL;

  draw_anim (&kid, frame, dx, dy);

  kid.fall = draw_kid_fall;
  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;

  if (i < 8) i++;
  else {
    kid.dir = (kid.dir == RIGHT) ? LEFT : RIGHT;
    kid.draw = draw_kid_run;
    i = 0;
  }
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
  void (*odraw) (void) = kid.draw;
  kid.draw = draw_kid_stabilize;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  static int i = 0;
  static bool collision = false;
  if (odraw != draw_kid_stabilize) i = 0;
  if (odraw == draw_kid_stabilize_collision) i = 1, collision = true;

  /* actions */
  if (! turn)
    turn = ((kid.dir == RIGHT) && left_key)
      || ((kid.dir == LEFT) && right_key);
  bool run = ((kid.dir == RIGHT) && right_key)
               || ((kid.dir == LEFT) && left_key);
  bool walk = ((kid.dir == RIGHT) && right_key && shift_key)
                || ((kid.dir == LEFT) && left_key && shift_key);
  bool jump = ((kid.dir == RIGHT) && right_key && up_key)
    || ((kid.dir == LEFT) && left_key && up_key);
  bool couch = down_key;

  struct frameset frameset[4] =
    {{kid_stabilize_05,-3,0},{kid_stabilize_06,-4,0},
     {kid_stabilize_07,+2,0},{kid_stabilize_08,+4,0}};

  ALLEGRO_BITMAP *frame = frameset[i].frame;
  int dx = frameset[i].dx;
  int dy = frameset[i].dy;

  if (kid.frame == kid_stop_run_04) dx = -5;

  kid.collision = NULL;
  kid.back_collision = NULL;
  kid.fall = NULL;

  draw_anim (&kid, frame, dx, dy);

  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;
  kid.fall = draw_kid_fall;

  if (! collision) {
    if (couch) {
      kid.draw = draw_kid_couch;
      i = 0; return;
    } else if (jump) {
      kid.draw = draw_kid_jump;
      i = 0; return;
    } else if (turn) {
      kid.draw = draw_kid_turn;
      i = 0; turn = false; return;
    } else if (walk) {
      kid.draw = draw_kid_walk;
      i = 0; return;
    } else if (run) {
      if (dist_collision (kid) < 32) kid.draw = draw_kid_walk;
      else kid.draw = draw_kid_start_run;
      i = 0; return;
    }
  }

  if (i < 3) i++;
  else if (i == 3) {
    kid.draw = draw_kid_normal;
    i = 0; collision = turn = false; return;
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
  void (*odraw) (void) = kid.draw;
  kid.draw = draw_kid_jump;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;
  misstep = false;

  static int i = 0;
  if (odraw != draw_kid_jump) i = 0;

  struct frameset frameset[18] =
    {{kid_jump_01,+2,0},{kid_jump_02,-2,0},{kid_jump_03,-4,0},
     {kid_jump_04,-5,0},{kid_jump_05,-3,0},{kid_jump_06,-6,0},
     {kid_jump_07,-3,0},{kid_jump_08,-17,0},{kid_jump_09,-21,0},
     {kid_jump_10,-11,-6},{kid_jump_11,-2,+6},{kid_jump_12,-9,0},
     {kid_jump_13,+6,0},{kid_jump_14,-11,0},{kid_jump_15,+0,0},
     {kid_jump_16,+0,0},{kid_jump_17,+1,0},{kid_jump_18,+0,0}};

  ALLEGRO_BITMAP *frame = frameset[i].frame;
  int dx = frameset[i].dx;
  int dy = frameset[i].dy;

  if (kid.frame == kid_stabilize_05) dx = +2;
  if (kid.frame == kid_stabilize_06) dx = +6;
  if (kid.frame == kid_stabilize_07) dx = +4;
  if (kid.frame == kid_stabilize_08) dx = +0;

  /* fall */
  if ((kids.cbb == NO_FLOOR
       && kids.cmbo == NO_FLOOR
       && i < 7)
      || ( i > 10 && kids.cbf == NO_FLOOR)) {
    draw_kid_fall ();
    i = 0; return;
  }

  /* collision */
  if (is_kid_colliding (-dx)) {
    if (i < 7 || i > 10) draw_kid_stabilize_collision ();
    else draw_kid_couch_collision ();
    i = 0; return;
  }

  if (i == 11 || i == 14) play_sample (step);
  if (i == 12) shake_loose_floor_row (kids.pmbo);

  kid.fall = NULL;
  kid.collision = NULL;
  kid.back_collision = NULL;

  draw_anim (&kid, frame, dx, dy);

  kid.fall = draw_kid_fall;
  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;

  if (i++ == 17) {
    kid.draw = draw_kid_normal;
    i = 0;
  }
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
  kid.draw = draw_kid_collision;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  /* if in the air (after a jump), but not falling, fall instead of
     standing against the wall */
  if ((is_kid_start_run_jump ()
       || is_kid_run_jump ()
       || is_kid_stop_run_jump ())) {
    switch (collision_construct) {
    case WALL:
      play_sample (hit_wall);
      if (is_falling (kid)) {
        draw_kid_fall ();
        return;
      }
    case DOOR:
      draw_kid_couch_collision ();
      return;
    default:
      error (-1, 0, "%s: unknown collision construction (%i)",
             __func__, collision_construct);
    }
  }

  /* notice that the following ignore conditions don't nullify the
     collision detection, as the wall continues to push the kid back
     to the acceptable limit.  To truly ignore collision, permitting
     the kid to temporally trespass such limit, set kid.collision to
     NULL */

  /* don't hardly hit the wall, continuing the current action if... */

  /* standing normally */
  /* if (is_kid_normal ()) { */
  /*   draw_kid_normal (); */
  /*   return; */
  /* } */

  /* vjump */
  if (is_kid_vjump ()
      || is_kid_start_vjump ()
      || is_kid_stop_vjump ()) {
    kid.odraw ();
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

  draw_kid_stabilize_collision ();
}

void
draw_kid_back_collision (void)
{
  kid.draw = draw_kid_collision;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  /* vertically jumping */
  if (is_kid_vjump ()
      || is_kid_start_vjump ()
      || is_kid_stop_vjump ()) {
    draw_kid_vjump ();
    return;
  }

  /* turning */
  if (is_kid_turn ()) {
    draw_kid_turn ();
    return;
  }

  /* falling */
  if (is_kid_fall ()) {
    draw_kid_fall ();
    return;
  }

  /* turning to run */
  if (is_kid_turn_run ()) {
    draw_kid_turn_run ();
    return;
  }

  /* don't collide recursively */
  kid.back_collision = NULL;

  /* draw the standard sequence of frames for kid's collision */
  if (kid.frame == kid_stabilize_06)
    draw_anim (&kid, kid_stabilize_07, +3, 0);
  else if (kid.frame == kid_stabilize_07) {
    draw_anim (&kid, kid_stabilize_08, +4, 0);
    kid.draw = draw_kid_normal;
  } else {
    draw_anim (&kid, kid_stabilize_06, -4, 0);
    play_sample (hit_wall);
  }

  kid.back_collision = draw_kid_back_collision;
}

void
draw_kid_fall (void)
{
  static int inertia;
  static int force_floor = -2;
  static int i = 0;
  struct pos p = pos (coord_bf (kid));
  int dx = 0;
  int dy = 0;

  /* hang if a edge is reachable and the shift key is pressed */
  if (is_hangable (kid) && shift_key && ! hang_limit
      && is_kid_fall ()
      && kid.frame != kid_fall_13) {
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
    shake_loose_floor_row (p);
    return;
  }

  /* comming from fall */
  if (kid.frame == kid_fall_13)
    draw_anim (&kid, kid_fall_14, -inertia / 2, +5);
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
    /* int dy = is_falling (kid) ? +17 : -6; */
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
  void (*odraw) (void) = kid.draw;
  kid.draw = draw_kid_couch;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;
  misstep = false;
  if (inertia > 0) inertia--;

  static int i = 0;
  static int wait = 1;
  static bool collision = false;
  if (odraw != draw_kid_couch) i = 0;
  if (odraw == draw_kid_climb) i = 11, critical_edge = false;
  if (odraw == draw_kid_couch_collision) collision = true;

  struct frameset frameset[13] =
    {{kid_couch_01,-3,0},{kid_couch_02,-4,0},{kid_couch_03,+0,0},
     {kid_couch_04,-4,0},{kid_couch_05,-1,0},{kid_couch_06,-4,0},
     {kid_couch_07,+1,0},{kid_couch_08,-2,0},{kid_couch_09,-1,0},
     {kid_couch_10,+0,0},{kid_couch_11,+3,0},{kid_couch_12,+0,0},
     {kid_couch_13,+4,0}};

  ALLEGRO_BITMAP *frame = frameset[i].frame;
  int dx = frameset[i].dx;
  int dy = frameset[i].dy;

  if (odraw == draw_kid_climb) dx += 7;
  if (i > 0 && i < 3) dx -= inertia ? 3 : 0;

  /* unclimb */
  int dir = (kid.dir == LEFT) ? +1 : -1;
  if (odraw != draw_kid_couch_collision
      && i == 0
      && odraw != draw_kid_fall
      && construct_rel (kids.pbf, 0, dir).fg == NO_FLOOR
      && dist_prev_place (kid) < 22
      && ! (kids.ctf == DOOR && kid.dir == LEFT
            && door_at_pos (kids.ptf)->i > DOOR_CLIMB_LIMIT)) {
    hang_pos = kids.pbf;
    critical_edge =
      (construct_rel (hang_pos, +1, dir).fg == NO_FLOOR);
    draw_kid_unclimb ();
    return;
  }

  /* fall */
  if (kids.cm == NO_FLOOR) {
    if (collision) kid_to_collision_edge (0);
    draw_kid_fall ();
    i = 0; collision = false; return;
  }

  /* collision */
  if (is_kid_colliding (-dx)) {
    if (! collision) {
      draw_kid_stabilize_collision ();
      kid.c.x += (kid.dir == LEFT) ? +4 : -4;
      i = 0; collision = false; return;
    } else kid_to_collision_edge (-dx);
  }

  kid.fall = NULL;
  kid.collision = NULL;
  kid.back_collision = NULL;

  draw_anim (&kid, frame, dx, dy);

  kid.fall = draw_kid_fall;
  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;

  if (i == 12) {
    kid.draw = draw_kid_normal;
    inertia = 0; i = 0; collision = false;
  } else if (i == 2 && down_key
           && ((kid.dir == LEFT && left_key)
               || (kid.dir == RIGHT && right_key))
             && wait-- == 0) {
    if (! is_kid_colliding (6))
      kid.c.x += (kid.dir == LEFT) ? -6 : +6;
    i = 0; wait = 1; collision = false;
  }
  else if (i != 2 || ! down_key) i++;
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
  void (*odraw) (void) = kid.draw;
  kid.draw = draw_kid_vjump;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;
  misstep = false;

  static int i = 0, j = 0;
  if (odraw != draw_kid_vjump) i = j = 0;
  if (odraw == draw_kid_hang_wall
      || odraw == draw_kid_hang_free) i = 13;
  static bool hang = false;

  struct frameset frameset[18] =
    {{kid_vjump_01,+3,+0},{kid_vjump_02,-1,+0},{kid_vjump_03,-1,+0},
     {kid_vjump_04,+2,+0},{kid_vjump_05,+0,+0},{kid_vjump_06,-1,+0},
     {kid_vjump_07,-1,+0},{kid_vjump_08,-3,+0},{kid_vjump_09,-1,+0},
     {kid_vjump_10,-6,+0},{kid_vjump_11,+0,+0},{kid_vjump_12,+2,-3},
     {kid_vjump_13,+3,-7},{kid_vjump_15,+0,+8},{kid_vjump_16,+3,+2},
     {kid_vjump_17,-1,+0},{kid_vjump_18,+0,+0},{kid_vjump_19,+0,0}};

  ALLEGRO_BITMAP *frame = frameset[i].frame;
  int dx = frameset[i].dx;
  int dy = frameset[i].dy;

  if (critical_edge && odraw == draw_kid_hang_free)
    dx -= (kid.dir == LEFT) ? 9 : 13;
  if (critical_edge && i == 11) dx = +7;
  if (i == 12 && j++ > 0) dx = 0, dy += 2 * j + 1;

  int dir = (kid.dir == LEFT) ? +1 : -1;
  if (i == 0
      && dist_prev_place (kid) < 22
      && is_hangable_pos_for_kid (prel (kids.ptf, 0, dir))) {
      to_prev_place_edge (&kid);
      if (construct (prel (kids.ptf, 0, dir)).fg == NO_FLOOR) {
        dx -= 12;
        critical_edge = true;
      } else {
        dx -= 3;
        critical_edge = false;
      }
      hang_pos = prel (kids.ptf, 0, dir);
      hang = true;
  } else if (i == 0 && can_kid_hang ()) {
    to_next_place_edge (&kid);
    dx -= 3; hang = true;
  }

  /* fall */
  if (kids.cm == NO_FLOOR
      && kids.cmf == NO_FLOOR
      && kids.cmba == NO_FLOOR) {
    draw_kid_fall ();
    i = j = 0; hang = critical_edge = false;
    return;
  }

  kid.collision = NULL;
  kid.back_collision = NULL;
  kid.fall = NULL;
  kid.ceiling = NULL;

  draw_anim (&kid, frame, dx, dy);

  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_collision;
  kid.fall = draw_kid_fall;
  kid.ceiling = draw_kid_ceiling;

  if (i == 12 && hang) {
    kid.draw = draw_kid_hang;
    i = j = 0; hang = false;
    return;
  }
  /* is kid hit ceiling? */
  else if (i == 12 && j == 1
           && construct_rel (kids.ptb, -1, 0).fg != NO_FLOOR) i++;
  else if (j == 4) j = 0, i++;
  else if (i != 12 && i < 17) i++;
  else if (i == 17) {
    shake_loose_floor_row (kids.pmbo);
    kid.draw = draw_kid_normal;
    hang = critical_edge = false; i = j = 0;
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
    shake_loose_floor_row (kids.pmbo);
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

  if (kids.cmba != NO_FLOOR) kid.fall = NULL;

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
  } else if (kid.frame == kid_couch_10) {
    draw_anim (&kid, kid_couch_11, +3, 0);
    shake_loose_floor_row (kids.pmbo);
  } else if (kid.frame == kid_couch_11)
    draw_anim (&kid, kid_couch_12, +0, 0);
  else if (kid.frame == kid_couch_12)
    draw_anim (&kid, kid_couch_13, +4, 0);
  else if (kid.frame == kid_couch_13) {
    kid.fall = draw_kid_fall;
    kid.draw = draw_kid_normal;
    draw_anim_on_fall_or_floor_edge (&kid, kid_normal, 0, +0, LOOSE_FLOOR);
  } else
    draw_anim (&kid, kid_walk_01, +0, 0);

  kid.fall = draw_kid_fall;
}

void
init_hang_frameset (void)
{
  struct frameset frameset[13] =
    {{kid_hang_00,+0,+0},{kid_hang_01,+0,+2},{kid_hang_02,+4,+0},
     {kid_hang_03,+3,+2},{kid_hang_04,+3,+0},{kid_hang_05,+1,+0},
     {kid_hang_06,+1,-1},{kid_hang_07,+2,+0},{kid_hang_08,+0,-3},
     {kid_hang_09,+0,+0}, {kid_hang_10,+1,-1},{kid_hang_11,+0,+0},
     {kid_hang_12,-3,+0}};

  memcpy (&hang_frameset, &frameset,
          13 * sizeof (struct frameset));
}

void
draw_kid_hang (void)
{
  void (*odraw) (void) = kid.draw;
  kid.draw = draw_kid_hang;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  kid.frame = kid_hang_14;
  int dir = (kid.dir == LEFT) ? 0 : 1;
  kid.c.x = PLACE_WIDTH * (hang_pos.place + dir) + 7;
  kid.c.y = PLACE_HEIGHT * hang_pos.floor - 9;

  critical_edge = (construct (hang_pos).fg == NO_FLOOR);

  kid.fall = NULL;
  kid.collision = NULL;
  kid.back_collision = NULL;

  if (odraw != draw_kid_couch && odraw != draw_kid_hang)
    draw_anim (&kid, kid_hang_14, +0, +0);
  else if (kids.cmf == WALL) draw_kid_hang_wall ();
  else draw_kid_hang_free ();

  kid.fall = draw_kid_fall;
  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;
}

void
draw_kid_hang_wall (void)
{
  kid.draw = draw_kid_hang_wall;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  static bool reverse = false;
  static int i = 4;
  static int wait = 0;
  if (kid.draw != draw_kid_hang_wall) {
    reverse = false; i = 4; wait = 0;
  }

  ALLEGRO_BITMAP *frame = hang_frameset[i].frame;
  int dx = (reverse) ? -hang_frameset[i + 1].dx : hang_frameset[i].dx;
  int dy = (reverse) ? -hang_frameset[i + 1].dy : hang_frameset[i].dy;

  if (kid.frame == kid_hang_14) dx = +0, dy = +1;
  if (reverse && wait == 0) {
    dx = 0;
    dy = 0;
  }
  if (! reverse && i == 4 && shift_key && ! up_key
      && ! hang_limit)
    play_sample (hit_wall);

  if ((! shift_key && (reverse || i > 4))
      || hang_limit || kids.ctf == NO_FLOOR) {
    reverse = false; i = 4; wait = 0;
    if (kids.cbb == NO_FLOOR) {
      draw_kid_fall ();
      hang_limit = false;
      return;
    }
    kid.frame = kid_vjump_15;
    kid.c.x += (kid.dir == LEFT) ? +12 : -12;
    kid.c.y = PLACE_HEIGHT * kids.pm.floor - 8;
    draw_kid_vjump ();
    hang_limit = false;
    return;
  } if (up_key) {
    draw_kid_climb ();
    reverse = false; i = 4; wait = 0;
    return;
  }

  kid.fall = NULL;
  kid.collision = NULL;
  kid.back_collision = NULL;

  draw_anim (&kid, frame, dx, dy);

  kid.fall = draw_kid_fall;
  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;

  if (! reverse && i < 6) i++;
  else if (! reverse && i == 6) {
   reverse = true;
  } else if (reverse && i > 4 && wait == 0) {
    i--; wait = 1;
  } else if (reverse && i >= 4 && wait > 0) wait--;
}

void
draw_kid_hang_free (void)
{
  void (*odraw) (void) = kid.draw;
  kid.draw = draw_kid_hang_free;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  static bool reverse = true;
  static int i = 4;
  static int j = -1;
  static int wait = 3;
  if (odraw != draw_kid_hang_free) {
    reverse = true; i = 4; j = -1; wait = 3;
  }

  ALLEGRO_BITMAP *frame = hang_frameset[i].frame;
  int dx = (reverse) ? -hang_frameset[i + 1].dx
    : hang_frameset[i].dx;
  int dy = (reverse) ? -hang_frameset[i + 1].dy
    : hang_frameset[i].dy;

  if (reverse && j == 0 && i == 0 && wait < 3) dy = 0;
  if (kid.frame == kid_hang_14) dx = +0, dy = +1;

  if ((! shift_key || hang_limit || kids.ctf == NO_FLOOR)
      && j > -1) {
    if (kids.cm == NO_FLOOR) {
      reverse = true; i = 4; j = -1; wait = 3;
      kid.c.x += (kid.dir == LEFT) ? +2 : -2;
      draw_kid_fall ();
      hang_limit = false;
      return;
    }
    kid.frame = kid_vjump_15;
    kid.c.x += (kid.dir == LEFT) ? +1 : -1;
    kid.c.y = PLACE_HEIGHT * kids.pm.floor - 8;
    draw_kid_vjump ();
    hang_limit = false;
    reverse = true; i = 4; j = -1; wait = 3;
    return;
  } if (up_key) {
    draw_kid_climb ();
    reverse = true; i = 4; j = -1; wait = 3;
    return;
  }

  kid.fall = NULL;
  kid.collision = NULL;
  kid.back_collision = NULL;

  draw_anim (&kid, frame, dx, dy);

  kid.fall = draw_kid_fall;
  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;

  if (reverse && i > 0) {
    if (i == 4  && j++ > 0) hang_limit = true;
    i--;
  } else if (reverse && i == 0) {
    if (wait == 0) {
      reverse = false; i++;
    } else wait--;
  } else if (! reverse
             && ((j == 0 && i < 12)
                 || (j > 0 && i < 9))) i++;
  else if (! reverse
           && ((j == 0 && i == 12)
               || (j > 0 && i == 9))) {
    reverse = true; i--;
  }
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
  void (*odraw) (void) = kid.draw;
  kid.draw = draw_kid_climb;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  static int i = 0;
  static int wait = DOOR_WAIT_LOOK;
  if (odraw != draw_kid_climb) i = 0;

  if (i == 15) {
    draw_kid_couch ();
    i = 0; wait = DOOR_WAIT_LOOK;
    return;
  }

  ALLEGRO_BITMAP *frame = climb_frameset[i].frame;
  int dx = climb_frameset[i].dx;
  int dy = climb_frameset[i].dy;

  /* climbing when looking left should let the kid near to the edge */
  if (kid.dir == LEFT && dx != 0 && i % 2) dx += 1;

  if (i == 0) {
    hang_pos = npos (hang_pos);
    kid.frame = kid_climb_01;
    int dir = (kid.dir == LEFT) ? 0 : 1;
    kid.c.x = PLACE_WIDTH * (hang_pos.place + dir) + 9;
    kid.c.y = PLACE_HEIGHT * hang_pos.floor - 9;
  }

  if (i == 3 && wait < DOOR_WAIT_LOOK) dx = 0, dy = 0;

  /* fall */
  if (kids.ctf == NO_FLOOR) {
    draw_kid_fall ();
    i = 0; wait = DOOR_WAIT_LOOK;
    return;
  }

  /* door collision */
  if (i == 3 && kids.ctf == DOOR
      && door_at_pos (kids.ptf)->i > DOOR_CLIMB_LIMIT) {
    if (wait == 0) {
      wait = DOOR_WAIT_LOOK;
      i = 0;
      int dir = (kid.dir == LEFT) ? -1 : +1;
      hang_pos = prel (hang_pos, -1, dir);
      hang_limit = true;
      draw_kid_unclimb ();
      return;
    } else if (wait > 0) wait--;
  }

  kid.fall = NULL;
  kid.collision = NULL;
  kid.back_collision = NULL;

  draw_anim (&kid, frame, dx, dy);

  kid.fall = draw_kid_fall;
  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;

 if (wait == DOOR_WAIT_LOOK && i < 15) i++;
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
init_climb_frameset (void)
{
  struct frameset frameset[15] =
    {{kid_climb_01,+1,+0},{kid_climb_02,+0,-9},{kid_climb_03,-4,-5},
     {kid_climb_04,-8,-6},{kid_climb_05,-5,-4},{kid_climb_06,-2,-5},
     {kid_climb_07,-1,-5},{kid_climb_08,-4,-8},{kid_climb_09,+0,-4},
     {kid_climb_10,+0,-1},{kid_climb_11,-3,-4},{kid_climb_12,+1,+0},
     {kid_climb_13,+0,+0},{kid_climb_14,-1,+0},{kid_climb_15,+0,+0}};

  memcpy (&climb_frameset, &frameset,
          15 * sizeof (struct frameset));
}

void
draw_kid_unclimb (void)
{
  void (*odraw) (void) = kid.draw;
  kid.draw = draw_kid_unclimb;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  static int i = 13;
  if (odraw != draw_kid_unclimb) i = 13;
  if (odraw == draw_kid_climb) i = 2;

  ALLEGRO_BITMAP *frame = climb_frameset[i].frame;
  int dx = -climb_frameset[i + 1].dx;
  int dy = -climb_frameset[i + 1].dy;

  if (i == 13) {
    kid.frame = kid_climb_14;
    kid.c.x = PLACE_WIDTH * hang_pos.place + 18;
    kid.c.y = PLACE_HEIGHT * hang_pos.floor + 25;
  }

  /* fall */
  if (kids.ctf == NO_FLOOR) {
    draw_kid_fall ();
    i = 13;
    return;
  }

  kid.fall = NULL;
  kid.collision = NULL;
  kid.back_collision = NULL;

  draw_anim (&kid, frame, dx, dy);

  kid.fall = draw_kid_fall;
  kid.collision = draw_kid_collision;
  kid.back_collision = draw_kid_back_collision;

  if (i-- == 1) {
    i = 13;
    kid.draw = draw_kid_hang;
    hang_pos =
      npos (prel (hang_pos, +1, (kid.dir == LEFT) ? +1 : -1));
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

  kid.back_collision = NULL;

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
  kid.back_collision = draw_kid_back_collision;
}

bool
is_kid_start_run_jump (void)
{
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
is_kid_run_jump (void)
{
  return kid.frame == kid_run_jump_09
    || kid.frame == kid_run_jump_10;
}

bool
is_kid_stop_run_jump (void)
{
  return kid.frame == kid_run_jump_11;
}

bool
is_kid_on_air (void)
{
  return kid.frame == kid_jump_04
    || kid.frame == kid_jump_05
    || kid.frame == kid_jump_06
    || kid.frame == kid_jump_07
    || kid.frame == kid_jump_08
    || kid.frame == kid_jump_09
    || kid.frame == kid_jump_10
    || kid.frame == kid_jump_11
    || kid.frame == kid_run_jump_06
    || kid.frame == kid_run_jump_07
    || kid.frame == kid_run_jump_08
    || kid.frame == kid_run_jump_09
    || kid.frame == kid_run_jump_10
    || kid.frame == kid_run_jump_11
    || kid.frame == kid_vjump_12
    || kid.frame == kid_vjump_13
    || kid.frame == kid_vjump_15;
}

bool
is_kid_hanging_at_pos (struct pos p)
{
  int dir = (kid.dir == LEFT) ? -1 : +1;
  return ((is_kid_hang ()
           || is_kid_start_climb ()
           || is_kid_climb ())
          && peq (prel (kids.pbb, -1, dir), p));
}

bool
is_kid_colliding (int min_dist)
{
  int dn = dist_next_place (kid);
  int dir = (kid.dir == LEFT) ? -1: +1;

  return dn <= min_dist
    && (/* wall */
        construct_rel (kids.pbf, 0, dir).fg == WALL

        /* door */
        || (kid.dir == RIGHT
            && kids.cbf == DOOR
            && kids.tf.y <= door_grid_tip_y (kids.pbf) - 10)
        || (kid.dir == LEFT
            && construct_rel (kids.pbf, 0, -1).fg == DOOR
            && kids.tf.y <=
            door_grid_tip_y (prel (kids.pbf, 0, -1)) - 10));
}

bool
is_kid_on_construct (int min_dist, enum construct_fg c)
{
  int dn = dist_next_place_0 (kid, coord_bf, pos, 4);
  int dir = (kid.dir == LEFT) ? -1: +1;

  return dn <= min_dist
    && construct_rel (kids.pmbo, 0, dir).fg == c;
}

int
dist_kid_collision (void)
{
  int dn = dist_next_place (kid);
  if (is_kid_colliding (dn)) return dn;
  else return PLACE_WIDTH;
}

int
dist_kid_construct (enum construct_fg c)
{
  int dn = dist_next_place_0 (kid, coord_bf, pos, 4);
  if (is_kid_on_construct (dn, c)) return dn;
  else return PLACE_WIDTH;
}

void
kid_to_collision_edge (int min_dist)
{
  int dc = dist_kid_collision ();
  int dir = (kid.dir == LEFT) ? -1 : +1;
  kid.c.x += dir * ((dc < PLACE_WIDTH) ? dc - 1 : 0);
  kid.c.x += -dir * min_dist;
}

void
kid_to_construct_edge (int min_dist, enum construct_fg c)
{
  int dc = dist_kid_construct (c);
  int dir = (kid.dir == LEFT) ? -1 : +1;
  kid.c.x += dir * ((dc < PLACE_WIDTH) ? dc - 1 : 0);
  kid.c.x += -dir * min_dist;
}

void
draw_kid_stabilize_collision (void)
{
  kid.draw = draw_kid_stabilize_collision;

  play_sample (hit_wall);
  kid_to_collision_edge (0);
  if (kid.dir == RIGHT) kid.c.x -= 3;

  draw_kid_stabilize ();
}

void
draw_kid_couch_collision (void)
{
  kid.draw = draw_kid_couch_collision;

  /* kid.c.y = PLACE_HEIGHT * kids.pbf.floor + 15; */
  /* kid.c.x = PLACE_WIDTH * kids.pbf.place + 15; */
  /* kid.frame = kid_normal; */

  play_sample (hit_wall);
  kid_to_collision_edge (0);
  if (kid.dir == RIGHT) kid.c.x -= 12;

  draw_kid_couch ();
}

bool
place_kid_on_collision_edge (ALLEGRO_BITMAP* frame)
{
  struct anim na = next_anim (kid, frame, +0, 0);
  kid = na;
  int dc = dist_kid_collision ();
  kid_to_collision_edge (0);
  return dc < PLACE_WIDTH;
}

bool
place_kid_on_construct_edge (ALLEGRO_BITMAP* frame,
                             enum construct_fg c)
{
  struct anim na = next_anim (kid, frame, +0, 0);
  kid = na;
  int dc = dist_kid_construct (c);
  kid_to_construct_edge (0, c);
  return dc < PLACE_WIDTH;
}

bool
is_hangable_pos_for_kid (struct pos p)
{
  int dir = (kid.dir == LEFT) ? -1 : +1;
  enum construct_fg fg = construct_rel (p, -1, dir).fg;

  return (fg == FLOOR || fg == BROKEN_FLOOR
          || fg == LOOSE_FLOOR || fg == OPENER_FLOOR
          || fg == PILLAR || fg == DOOR)
    && construct_rel (p, -1, 0).fg == NO_FLOOR;
}

bool
can_kid_hang (void)
{
  bool hmf = is_hangable_pos_for_kid (kids.pmf);
  /* bool hm = is_hangable_pos_for_kid (kids.pm); */
  /* bool hmb = is_hangable_pos_for_kid (kids.pmb); */

  if (! hmf
      /* && ! hm */
      /* && ! hmb */
      ) return false;

  if (hmf) hang_pos = kids.pmf;
  /* if (hm) hang_pos = kids.pm; */
  /* if (hmb) hang_pos = kids.pmb; */

  /* for fall */
  /* struct coord ch; */
  /* int dir = (kid.dir == LEFT) ? 0 : 1; */
  /* ch.x = PLACE_WIDTH * (hang_pos.place + dir) + 7 + 8 * dir; */
  /* ch.y = PLACE_HEIGHT * hang_pos.floor - 6; */

  /* if (dist_coord (kids.tf, ch) > 16) return false; */

  /* printf ("dist_coord = %f\n", dist_coord (kids.tf, ch)); */

  return true;
}

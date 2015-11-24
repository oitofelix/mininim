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
#include "opener-floor.h"

struct anim kid;
struct survey kids, kidsf;
static bool hang_limit;
static bool misstep = false;
static int inertia = 0;
static bool critical_edge = false;
static bool turn = false;
static struct pos depressible_floor_pos[2] = {{.room = -1},
                                            {.room = -1}};
bool depressible_loose_floor;

#define WALK_FRAMESET_NMEMB 12
#define START_RUN_FRAMESET_NMEMB 6
#define RUN_FRAMESET_NMEMB 8
#define STOP_RUN_FRAMESET_NMEMB 4
#define TURN_FRAMESET_NMEMB 4
#define TURN_RUN_FRAMESET_NMEMB 9
#define JUMP_FRAMESET_NMEMB 18
#define VJUMP_FRAMESET_NMEMB 18
#define RUN_JUMP_FRAMESET_NMEMB 11
#define MISSTEP_FRAMESET_NMEMB 11
#define HANG_FRAMESET_NMEMB 13
#define CLIMB_FRAMESET_NMEMB 15
#define STABILIZE_FRAMESET_NMEMB 4
#define COUCH_FRAMESET_NMEMB 13
#define FALL_FRAMESET_NMEMB 13

static struct frameset walk_frameset[WALK_FRAMESET_NMEMB];
static struct frameset start_run_frameset[START_RUN_FRAMESET_NMEMB];
static struct frameset run_frameset[RUN_FRAMESET_NMEMB];
static struct frameset stop_run_frameset[STOP_RUN_FRAMESET_NMEMB];
static struct frameset turn_frameset[TURN_FRAMESET_NMEMB];
static struct frameset turn_run_frameset[TURN_RUN_FRAMESET_NMEMB];
static struct frameset jump_frameset[JUMP_FRAMESET_NMEMB];
static struct frameset vjump_frameset[VJUMP_FRAMESET_NMEMB];
static struct frameset run_jump_frameset[JUMP_FRAMESET_NMEMB];
static struct frameset misstep_frameset[MISSTEP_FRAMESET_NMEMB];
static struct frameset hang_frameset[HANG_FRAMESET_NMEMB];
static struct frameset climb_frameset[CLIMB_FRAMESET_NMEMB];
static struct frameset stabilize_frameset[STABILIZE_FRAMESET_NMEMB];
static struct frameset couch_frameset[COUCH_FRAMESET_NMEMB];
static struct frameset fall_frameset[FALL_FRAMESET_NMEMB];

static void init_walk_frameset (void);
static void init_start_run_frameset (void);
static void init_run_frameset (void);
static void init_stop_run_frameset (void);
static void init_turn_frameset (void);
static void init_turn_run_frameset (void);
static void init_jump_frameset (void);
static void init_vjump_frameset (void);
static void init_run_jump_frameset (void);
static void init_misstep_frameset (void);
static void init_hang_frameset (void);
static void init_climb_frameset (void);
static void init_stabilize_frameset (void);
static void init_couch_frameset (void);
static void init_fall_frameset (void);

ALLEGRO_BITMAP *kid_normal_00,
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

static void place_kid (int room, int floor, int place);

static void kid_normal (void);
static void kid_walk (void);
static void kid_start_run (void);
static void kid_run (void);
static void kid_stop_run (void);
void kid_turn (void);
static void kid_turn_run (void);
static void kid_jump (void);
void kid_vjump (void);
static void kid_run_jump (void);
static void kid_misstep (void);
static void kid_hang (void);
static void kid_hang_wall (void);
static void kid_hang_free (void);
static void kid_climb (void);
static void kid_unclimb (void);
static void kid_stabilize (void);
static void kid_couch (void);
static void kid_stabilize_collision (void);
static void kid_couch_collision (void);
static void kid_fall (void);

static void
update_depressible_floor
(struct coord (*coord_f0) (struct anim a),
 struct coord (*coord_f1) (struct anim a));
static void keep_depressible_floor (void);
static void press_depressible_floor (struct pos p);

void
load_kid (void)
{
  /* bitmap */
  kid_normal_00 = load_bitmap (KID_NORMAL_00);
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

  /* framesets */
  init_walk_frameset ();
  init_start_run_frameset ();
  init_run_frameset ();
  init_stop_run_frameset ();
  init_turn_frameset ();
  init_turn_run_frameset ();
  init_jump_frameset ();
  init_vjump_frameset ();
  init_run_jump_frameset ();
  init_misstep_frameset ();
  init_hang_frameset ();
  init_climb_frameset ();
  init_stabilize_frameset ();
  init_couch_frameset ();
  init_fall_frameset ();

  /* kid himself */
  kid.id = &kid;
  kid.frame = kid_normal_00;
  kid.dir = LEFT;
  kid.action = kid_normal;

  place_kid (7, 0, 8);
}

void
unload_kid (void)
{
  /* bitmaps */
  al_destroy_bitmap (kid_normal_00);
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
init_walk_frameset (void)
{
  struct frameset frameset[WALK_FRAMESET_NMEMB] =
    {{kid_walk_01,-1,0},{kid_walk_02,-1,0},{kid_walk_03,+0,0},
     {kid_walk_04,-8,0},{kid_walk_05,-7,0},{kid_walk_06,-6,0},
     {kid_walk_07,+3,0},{kid_walk_08,-2,0},{kid_walk_09,-1,0},
     {kid_walk_10,-1,0},{kid_walk_11,-2,0},{kid_walk_12,+0,0}};

  memcpy (&walk_frameset, &frameset,
          WALK_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_start_run_frameset (void)
{
  struct frameset frameset[START_RUN_FRAMESET_NMEMB] =
    {{kid_start_run_01,-1,0},{kid_start_run_02,-2,0},
     {kid_start_run_03,-5,0},{kid_start_run_04,-1,0},
     {kid_start_run_05,-7,0},{kid_start_run_06,-6,0}};

  memcpy (&start_run_frameset, &frameset,
          START_RUN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_run_frameset (void)
{
  struct frameset frameset[RUN_FRAMESET_NMEMB] =
    {{kid_run_07,-10,0},{kid_run_08,-7,0},{kid_run_09,-4,0},
     {kid_run_10,-4,0},{kid_run_11,-8,0},{kid_run_12,-11,0},
     {kid_run_13,-4,0},{kid_run_14,-8,0}};

  memcpy (&run_frameset, &frameset,
          RUN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_stop_run_frameset (void)
{
  struct frameset frameset[STOP_RUN_FRAMESET_NMEMB] =
    {{kid_stop_run_01,+0,0},{kid_stop_run_02,-1,0},
     {kid_stop_run_03,-21,0},{kid_stop_run_04,-2,0}};

  memcpy (&stop_run_frameset, &frameset,
          STOP_RUN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_turn_frameset (void)
{
  struct frameset frameset[TURN_FRAMESET_NMEMB] =
    {{kid_turn_01,+2,0},{kid_turn_02,+1,0},
     {kid_turn_03,-3,0},{kid_turn_04,+0,+0}};

  memcpy (&turn_frameset, &frameset,
          TURN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_turn_run_frameset (void)
{
  struct frameset frameset[9] =
    {{kid_turn_run_05,-18,0},{kid_turn_run_06,-6,0},{kid_turn_run_07,-4,0},
     {kid_turn_run_08,+2,0},{kid_turn_run_09,-6,0},{kid_turn_run_10,+3,0},
     {kid_turn_run_11,-1,0},{kid_turn_run_12,+0,0},{kid_turn_run_13,+4,0}};

  memcpy (&turn_run_frameset, &frameset,
          TURN_RUN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_jump_frameset (void)
{
  struct frameset frameset[JUMP_FRAMESET_NMEMB] =
    {{kid_jump_01,+0,0},{kid_jump_02,-2,0},{kid_jump_03,-3,0},
     {kid_jump_04,-6,0},{kid_jump_05,-2,0},{kid_jump_06,-4,0},
     {kid_jump_07,-1,0},{kid_jump_08,-12,0},{kid_jump_09,-18,0},
     {kid_jump_10,-15,-6},{kid_jump_11,-2,+6},{kid_jump_12,-11,0},
     {kid_jump_13,+5,0},{kid_jump_14,-13,0},{kid_jump_15,+0,0},
     {kid_jump_16,-1,0},{kid_jump_17,-1,0},{kid_jump_18,+0,0}};

  memcpy (&jump_frameset, &frameset,
          JUMP_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_vjump_frameset (void)
{
  struct frameset frameset[VJUMP_FRAMESET_NMEMB] =
    {{kid_vjump_01,+3,+0},{kid_vjump_02,-1,+0},{kid_vjump_03,-1,+0},
     {kid_vjump_04,+2,+0},{kid_vjump_05,+0,+0},{kid_vjump_06,-1,+0},
     {kid_vjump_07,-1,+0},{kid_vjump_08,-3,+0},{kid_vjump_09,-1,+0},
     {kid_vjump_10,-6,+0},{kid_vjump_11,+0,+0},{kid_vjump_12,+2,-3},
     {kid_vjump_13,+3,-7},{kid_vjump_15,+0,+8},{kid_vjump_16,+3,+2},
     {kid_vjump_17,-1,+0},{kid_vjump_18,+0,+0},{kid_vjump_19,+0,0}};

  memcpy (&vjump_frameset, &frameset,
          VJUMP_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_run_jump_frameset (void)
{
  struct frameset frameset[RUN_JUMP_FRAMESET_NMEMB] =
    {{kid_run_jump_01,-10,+0},{kid_run_jump_02,-11,+0},
     {kid_run_jump_03,-13,+0},{kid_run_jump_04,-7,+0},
     {kid_run_jump_05,-12,+0},{kid_run_jump_06,-15,+0},
     {kid_run_jump_07,-29,-3},{kid_run_jump_08,-17,-9},
     {kid_run_jump_09,-18,-2},{kid_run_jump_10,-10,+11},
     {kid_run_jump_11,-8,+3}};

  memcpy (&run_jump_frameset, &frameset,
          RUN_JUMP_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_misstep_frameset (void)
{
  struct frameset frameset[MISSTEP_FRAMESET_NMEMB] =
    {{kid_walk_01,-1,0},{kid_walk_02,-1,0},{kid_walk_03,+0,0},
     {kid_walk_04,-8,0},{kid_walk_05,-7,0},{kid_walk_06,-4,0},
     {kid_jump_14,+8,0},{kid_couch_10,+8,0},{kid_couch_11,+3,0},
     {kid_couch_12,+0,0},{kid_couch_13,+4,0}};

  memcpy (&misstep_frameset, &frameset,
          MISSTEP_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_hang_frameset (void)
{
  struct frameset frameset[HANG_FRAMESET_NMEMB] =
    {{kid_hang_00,+0,+0},{kid_hang_01,+0,+2},{kid_hang_02,+4,+0},
     {kid_hang_03,+3,+2},{kid_hang_04,+3,+0},{kid_hang_05,+1,+0},
     {kid_hang_06,+1,-1},{kid_hang_07,+2,+0},{kid_hang_08,+0,-3},
     {kid_hang_09,+0,+0},{kid_hang_10,+1,-1},{kid_hang_11,+0,+0},
     {kid_hang_12,-3,+0}};

  memcpy (&hang_frameset, &frameset,
          HANG_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_climb_frameset (void)
{
  struct frameset frameset[CLIMB_FRAMESET_NMEMB] =
    {{kid_climb_01,+1,+0},{kid_climb_02,+0,-9},{kid_climb_03,-4,-5},
     {kid_climb_04,-8,-6},{kid_climb_05,-5,-4},{kid_climb_06,-2,-5},
     {kid_climb_07,-1,-5},{kid_climb_08,-4,-8},{kid_climb_09,+0,-4},
     {kid_climb_10,+0,-1},{kid_climb_11,-3,-4},{kid_climb_12,+1,+0},
     {kid_climb_13,+0,+0},{kid_climb_14,-1,+0},{kid_climb_15,+0,+0}};

  memcpy (&climb_frameset, &frameset,
          CLIMB_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_stabilize_frameset (void)
{
  struct frameset frameset[STABILIZE_FRAMESET_NMEMB] =
    {{kid_stabilize_05,-3,0},{kid_stabilize_06,-4,0},
     {kid_stabilize_07,+2,0},{kid_stabilize_08,+4,0}};

  memcpy (&stabilize_frameset, &frameset,
          STABILIZE_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_couch_frameset (void)
{
  struct frameset frameset[COUCH_FRAMESET_NMEMB] =
    {{kid_couch_01,-3,0},{kid_couch_02,-4,0},{kid_couch_03,+0,0},
     {kid_couch_04,-4,0},{kid_couch_05,-1,0},{kid_couch_06,-4,0},
     {kid_couch_07,+1,0},{kid_couch_08,-2,0},{kid_couch_09,-1,0},
     {kid_couch_10,+0,0},{kid_couch_11,+3,0},{kid_couch_12,+0,0},
     {kid_couch_13,+4,0}};

  memcpy (&couch_frameset, &frameset,
          COUCH_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_fall_frameset (void)
{
  struct frameset frameset[FALL_FRAMESET_NMEMB] =
    {{kid_fall_13,+0,+0},{kid_fall_14,+0,+5},{kid_fall_15,+0,+10},
     {kid_fall_16,+0,+11},{kid_fall_17,+0,+20}};

  memcpy (&fall_frameset, &frameset,
          FALL_FRAMESET_NMEMB * sizeof (struct frameset));
}




void
kid_normal (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_normal;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
  inertia = 0;

  bool turn = ((kid.dir == RIGHT) && left_key) || ((kid.dir == LEFT) && right_key);
  bool walk = ((kid.dir == RIGHT) && right_key && shift_key)
    || ((kid.dir == LEFT) && left_key && shift_key);
  bool run = (((kid.dir == RIGHT) && right_key)
              || ((kid.dir == LEFT) && left_key)) && ! walk;
  bool jump = ((kid.dir == RIGHT) && right_key && up_key)
    || ((kid.dir == LEFT) && left_key && up_key);
  bool couch = down_key;
  bool vjump = up_key;

  ALLEGRO_BITMAP *frame = kid_normal_00;
  int dx = +0;
  int dy = +0;

  /* fall */
  if (kids.cmbo == NO_FLOOR) {
    kid_fall ();
    return;
  }

  /* static int px = 0; */
  /* static int py = 0; */
  /* if (a_key) px--; */
  /* if (d_key) px++; */
  /* if (w_key) py--; */
  /* if (s_key) py++; */
  /* al_set_target_bitmap (screen); */
  /* al_put_pixel (px, py, al_map_rgb (0, 255, 255)); */

    /* printf ("x = %i, y = %i, floor = %i, place = %i\n", px, py, (py -3) / 63, (px - 15) / 32); */

  if (kid.frame == stabilize_frameset[3].frame) dx = +2;
  if (kid.frame == walk_frameset[11].frame) dx = -1;
  if (kid.frame == jump_frameset[17].frame) dx = -2;
  if (kid.frame == couch_frameset[12].frame) dx = -2;
  if (kid.frame == vjump_frameset[17].frame) dx = +2;

  if (couch) {
    kid_couch ();
    return;
  } else if (jump) {
    kid_jump ();
    return;
  } else if (turn) {
    kid_turn ();
    return;
  } else if (vjump) {
    kid_vjump ();
    return;
  } else if (walk) {
    kid_walk ();
    return;
  } else if (run) {
    if (dist_collision (kid, coord_tf, pos, 0, false) < 29)
      kid_walk ();
    else kid_start_run ();
    return;
  }

  kid = next_anim (kid, frame, dx, dy);
  kid.c.y = 63 * kids.ptf.floor + 15;

  /* depressible floors */
  keep_depressible_floor ();
}

void
kid_walk (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_walk;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
  inertia = 0;

  static int i = 0;
  if (oaction != kid_walk) i = 0;

  static bool walk_0 = false, walk_1 = false,
    walk_2 = false, walk_3 = false, walk_4 = false;

  int dc, df, dl;

  if (i == 0) {
    dc = dist_collision (kid, coord_tf, pos, +0, false);
    df = dist_con (kid, coord_bf, pos, -4, false, NO_FLOOR);
    dl = dist_con (kid, coord_bf, pos, -4, false, LOOSE_FLOOR);

    if (dc < 4) {
      kid = next_anim (kid, kid_normal_00, +0, 0);
      kid.action = kid_normal;
      keep_depressible_floor ();
      return;
    }

    if (! misstep) {
      if (df < 4 || dl < 4) {
        kid_misstep ();
        return;
      } else if (dc < 9 || df < 9 || dl < 9) walk_0 = true;
      else if (dc < 16 || df < 16 || dl < 16) walk_1 = true;
      else if (dc < 21 || df < 21 || dl < 21) walk_2 = true;
      else if (dc < 24 || df < 24 || dl < 24) walk_3 = true;
      else if (dc < 27 || df < 27 || dl < 27) walk_4 = true;
    }
  }

  ALLEGRO_BITMAP *frame = walk_frameset[i].frame;
  int dx = walk_frameset[i].dx;
  int dy = walk_frameset[i].dy;

  if (((i == 10) && walk_0)
      || ((i == 3) && walk_1)
      || ((i == 4) && walk_2)
      || ((i == 5) && walk_3)
      || ((i == 6) && walk_4)) {
    dx = +1;
    to_collision_edge (&kid, frame, coord_tf, pos, +0, false, 0)
      || to_con_edge (&kid, frame, coord_bf, pos, -4, false, 0, LOOSE_FLOOR)
      || to_con_edge (&kid, frame, coord_bf, pos, -4, false, 0, NO_FLOOR);
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
    if (kid.frame == turn_frameset[3].frame) dx = +0;
    if (kid.frame == stabilize_frameset[0].frame) dx = +2;
    if (kid.frame == stabilize_frameset[1].frame) dx = +6;
    if (kid.frame == stabilize_frameset[2].frame) dx = +3;
    if (kid.frame == stabilize_frameset[3].frame) dx = +0;
  }

  /* fall */
  if (! walk_0 && ! walk_1 && ! walk_2
      && ! walk_3 && ! walk_4
      && ((i < 6 && kids.cbb == NO_FLOOR)
          || (i >= 6 && kids.cmbo == NO_FLOOR))) {
    kid_fall ();
    i =0; return;
  }

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  depressible_loose_floor = misstep;

  switch (i) {
  case 0: update_depressible_floor (coord_bf, coord_bb); break;
  case 1: update_depressible_floor (coord_bf, coord_bb); break;
  case 2: update_depressible_floor (coord_bf, coord_bb); break;
  case 3: update_depressible_floor (coord_mbo, coord_bb); break;
  case 4: update_depressible_floor (coord_mbo, coord_bb); break;
  case 5: update_depressible_floor (coord_mbo, coord_bb); break;
  case 6: update_depressible_floor (coord_bf, coord_bb); break;
  case 7: update_depressible_floor (coord_bf, coord_bb); break;
  case 8: update_depressible_floor (coord_bf, coord_bb); break;
  case 9: update_depressible_floor (coord_bf, coord_bb); break;
  case 10: update_depressible_floor (coord_bf, coord_bb); break;
  case 11: update_depressible_floor (coord_bf, coord_bb); break;
  }

  /* next iteration */
  if (i == 2 && walk_0) i = 10;
  else if (i == 3 && walk_1) i = 9;
  else if (i == 4 && walk_2) i = 7;
  else if (i == 5 && walk_3) i = 7;
  else if (i == 6 && walk_4) i = 7;
  else if (i < 11) i++;
  else {
    kid.action = kid_normal;
    walk_0 = walk_1 = walk_2 = walk_3 = walk_4 = false;
    misstep = false; i = 0;
  }
}

void
kid_start_run (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_start_run;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
  misstep = false;
  inertia = 1;

  static int i = 0;
  if (oaction != kid_start_run) i = 0;

  ALLEGRO_BITMAP *frame = start_run_frameset[i].frame;
  int dx = start_run_frameset[i].dx;
  int dy = start_run_frameset[i].dy;

  if (kid.frame == turn_frameset[3].frame) dx = +0;
  if (kid.frame == stabilize_frameset[0].frame) dx = +2;
  if (kid.frame == stabilize_frameset[1].frame) dx = +6;
  if (kid.frame == stabilize_frameset[2].frame) dx = +4;
  if (kid.frame == stabilize_frameset[3].frame) dx = +0;

  bool run = (kid.dir == RIGHT) ? right_key : left_key;
  bool turn_run = (kid.dir == RIGHT) ? left_key : right_key;
  bool couch = down_key;
  bool jump = ((kid.dir == RIGHT) && right_key && up_key)
    || ((kid.dir == LEFT) && left_key && up_key);

  /* fall */
  if (kids.ctf == NO_FLOOR) {
    kid_fall ();
    i = 0; return;
  }

  /* collision */
  if (is_colliding (kid, coord_tf, pos, 0, false, -dx)) {
    kid_stabilize_collision ();
    i = 0; return;
  }

  if (couch) {
    kid_couch ();
    i = 0; return;
  }

  if (jump && i < 3) {
    kid_jump ();
    i = 0;
    return;
  }

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  depressible_loose_floor = true;

  switch (i) {
  case 0: update_depressible_floor (coord_mbo, coord_bb); break;
  case 1: update_depressible_floor (coord_mbo, coord_bb); break;
  case 2: update_depressible_floor (coord_mbo, coord_bb); break;
  case 3: update_depressible_floor (coord_mbo, coord_bb); break;
  case 4: update_depressible_floor (coord_mbo, coord_bb); break;
  case 5: update_depressible_floor (coord_mbo, coord_bb); break;
  }

  if ((i == 3 || i ==4) && turn_run) {
    kid.action = kid_stop_run;
    i = 0;
  }

  if (i < 5) i++;
  else {
    kid.action = run ? kid_run : kid_stop_run;
    i = 0;
  }
}

void
kid_run (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_run;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
  inertia = 3;

  static int i = 0;
  if (oaction != kid_run) i = 0;

  if (kid.frame == turn_run_frameset[8].frame) i = 6;

  ALLEGRO_BITMAP *frame = run_frameset[i].frame;
  int dx = run_frameset[i].dx;
  int dy = run_frameset[i].dy;

  bool stop = ! ((kid.dir == RIGHT) ? right_key : left_key);
  bool couch = down_key;
  bool jump = ((kid.dir == RIGHT) && right_key && up_key)
    || ((kid.dir == LEFT) && left_key && up_key);

  /* fall */
  if (kids.ctf == NO_FLOOR) {
    kid_fall ();
    i = 0; return;
  }

  /* collision */
  if (is_colliding (kid, coord_tf, pos, 0, false, -dx)) {
    kid_stabilize_collision ();
    i = 0; return;
  }

  if (couch) {
    kid_couch ();
    i = 0; return;
  }

  if (jump && kid.frame != run_jump_frameset[10].frame) {
    kid_run_jump ();
    i = 0; return;
  }

  if (stop) {
    kid_stop_run ();
    i = 0; return;
  }

  if (kid.frame == start_run_frameset[5].frame) dx = -6;
  if (kid.frame == turn_run_frameset[8].frame) dx = -4;
  if (kid.frame == run_jump_frameset[10].frame) dx = -15, play_sample (step);
  if (i == 2 || i == 6) play_sample (step);

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  depressible_loose_floor = true;

  switch (i) {
  case 0: update_depressible_floor (coord_mbo, coord_bb); break;
  case 1: update_depressible_floor (coord_mbo, coord_bb); break;
  case 2: update_depressible_floor (coord_mbo, coord_bb); break;
  case 3: update_depressible_floor (coord_mbo, coord_bb); break;
  case 4: update_depressible_floor (coord_mbo, coord_bb); break;
  case 5: update_depressible_floor (coord_mbo, coord_bb); break;
  case 6: update_depressible_floor (coord_mbo, coord_bb); break;
  case 7: update_depressible_floor (coord_mbo, coord_bb); break;
  }

  if (i < 7) i++;
  else i = 0;
}

void
kid_stop_run (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_stop_run;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
  inertia = 0;

  static int i = 0;
  if (oaction != kid_stop_run) i = 0;

  ALLEGRO_BITMAP *frame = stop_run_frameset[i].frame;
  int dx = stop_run_frameset[i].dx;
  int dy = stop_run_frameset[i].dy;

  bool turn_run = (kid.dir == RIGHT) ? left_key : right_key;
  bool couch = down_key;

  /* fall */
  if (kids.ctf == NO_FLOOR) {
    kid_fall ();
    i = 0; return;
  }

  /* collision */
  if (is_colliding (kid, coord_tf, pos, 0, false, -dx)) {
    kid_stabilize_collision ();
    i = 0; return;
  }

  if (couch) {
    kid_couch ();
    return;
  }

  if (i == 1 || i == 3) play_sample (step);

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  depressible_loose_floor = true;

  switch (i) {
  case 0: update_depressible_floor (coord_bf, coord_mbo); break;
  case 1: update_depressible_floor (coord_bf, coord_mbo); break;
  case 2: update_depressible_floor (coord_bf, coord_mbo); break;
  case 3: update_depressible_floor (coord_bf, coord_mbo); break;
  }

  if (i < 3) i++;
  else {
    i = 0;
    kid.action = turn_run ? kid_turn_run : kid_stabilize;
  }
}

void
kid_turn (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_turn;
  kid.flip = (kid.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;
  misstep = false;

  static int i = 0;
  if (oaction != kid_turn) i = 0;

  ALLEGRO_BITMAP *frame = turn_frameset[i].frame;
  int dx = turn_frameset[i].dx;
  int dy = turn_frameset[i].dy;

  if (kid.frame == stabilize_frameset[0].frame) dx = +6;
  if (kid.frame == stabilize_frameset[1].frame) dx = +10;
  if (kid.frame == stabilize_frameset[2].frame) dx = +8;
  if (kid.frame == stabilize_frameset[3].frame) dx = +4;
  if (kid.frame == turn_frameset[3].frame) dx = +3;

  /* fall */
  if (kids.cbf == NO_FLOOR && kids.cbb == NO_FLOOR) {
    kid_fall ();
    return;
  }

  /* collision */
  kid.dir = (kid.dir == RIGHT) ? LEFT : RIGHT;
  if (is_colliding (kid, coord_tf, pos, 0, false, -dx)) {
    to_collision_edge (&kid, frame, coord_tf, pos, 0, false, 0);
    dx = 0;
  }
  kid.dir = (kid.dir == RIGHT) ? LEFT : RIGHT;

  if (! turn)
    turn = ((kid.dir == RIGHT) && right_key)
      || ((kid.dir == LEFT) && left_key);
  bool run = (kid.dir == RIGHT) ? left_key : right_key;
  bool walk = run && shift_key;
  bool jump = ((kid.dir == RIGHT) && left_key && up_key)
    || ((kid.dir == LEFT) && right_key && up_key);
  bool couch = down_key;

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  keep_depressible_floor ();

  if (i < 3) i++;
  else if (i == 3) {
    i = 0;
    kid.dir = (kid.dir == RIGHT) ? LEFT : RIGHT;
    int dc = dist_collision (kid, coord_tf, pos, 0, false);
    int df = dist_con (kid, coord_bf, pos, -4, false, NO_FLOOR);
    if (turn) kid.action = kid_turn, turn = false;
    else if (couch) kid.action = kid_couch;
    else if (jump) kid.action = kid_jump;
    else if (walk && dc > PLACE_WIDTH && df > PLACE_WIDTH)
      kid.action = kid_walk;
    else if (run && dc > PLACE_WIDTH && df > PLACE_WIDTH)
      kid.action = kid_start_run;
    else kid.action = kid_stabilize;
  }
}

void
kid_turn_run (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_turn_run;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;
  inertia = 0;

  static int i = 0;
  if (oaction != kid_turn_run) i = 0;

  ALLEGRO_BITMAP *frame = turn_run_frameset[i].frame;
  int dx = turn_run_frameset[i].dx;
  int dy = turn_run_frameset[i].dy;

  /* fall */
  if (kids.ctf == NO_FLOOR) {
    kid_fall ();
    i = 0; return;
  }

  /* collision */
  if (is_colliding (kid, coord_tf, pos, 0, false, -dx))
    to_collision_edge (&kid, frame, coord_tf, pos, 0, false, -dx);

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  depressible_loose_floor = true;

  switch (i) {
  case 0: update_depressible_floor (coord_bf, coord_bb); break;
  case 1: update_depressible_floor (coord_bf, coord_mbo); break;
  case 2: update_depressible_floor (coord_bf, coord_mbo); break;
  case 3: update_depressible_floor (coord_bf, coord_mbo); break;
  case 4: update_depressible_floor (coord_bf, coord_mbo); break;
  case 5: update_depressible_floor (coord_bf, coord_mbo); break;
  case 6: update_depressible_floor (coord_bf, coord_mbo); break;
  case 7: update_depressible_floor (coord_bf, coord_mbo); break;
  case 8: update_depressible_floor (coord_bf, coord_mbo); break;
  }

  if (i < 8) i++;
  else {
    kid.dir = (kid.dir == RIGHT) ? LEFT : RIGHT;
    kid.action = kid_run;
    i = 0;
  }
}

void
kid_jump (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_jump;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;
  misstep = false;
  inertia = 4;

  static int i = 0;
  if (oaction != kid_jump) i = 0;

  ALLEGRO_BITMAP *frame = jump_frameset[i].frame;
  int dx = jump_frameset[i].dx;
  int dy = jump_frameset[i].dy;

  if (kid.frame == stabilize_frameset[0].frame) dx = +2;
  if (kid.frame == stabilize_frameset[1].frame) dx = +6;
  if (kid.frame == stabilize_frameset[2].frame) dx = +4;
  if (kid.frame == stabilize_frameset[3].frame) dx = +0;

  /* fall */
  if ((kids.cbb == NO_FLOOR
       && kids.cmbo == NO_FLOOR
       && i < 7)
      || ( i > 9 && kids.cbf == NO_FLOOR)) {
    kid_fall ();
    i = 0; return;
  }

  /* collision */
  if (is_colliding (kid, coord_tf, pos, 0, false, -dx)) {
    if (i < 7 || i > 10) kid_stabilize_collision ();
    else kid_couch_collision ();
    i = 0; return;
  }

  if (i == 11 || i == 14) play_sample (step);
  if (i == 12) shake_loose_floor_row (kids.pmbo);

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  depressible_loose_floor = (i > 13);

  switch (i) {
  case 0: update_depressible_floor (coord_bf, coord_bb); break;
  case 1: update_depressible_floor (coord_bf, coord_bb); break;
  case 2: update_depressible_floor (coord_mbo, coord_bb); break;
  case 3: update_depressible_floor (coord_mbo, coord_bb); break;
  case 4: update_depressible_floor (coord_mbo, coord_bb); break;
  case 5: update_depressible_floor (coord_mbo, coord_bb); break;
  case 6: update_depressible_floor (coord_mbo, coord_bb); break;
  case 7: update_depressible_floor (coord_mbo, coord_bb); break;
  case 8: update_depressible_floor (NULL, NULL); break;
  case 9: update_depressible_floor (NULL, NULL); break;
  case 10: update_depressible_floor (NULL, NULL); break;
  case 11: update_depressible_floor (coord_mbo, NULL); break;
  case 12: update_depressible_floor (coord_mbo, NULL); break;
  case 13: update_depressible_floor (coord_mbo, NULL); break;
  case 14: update_depressible_floor (coord_bf, coord_mbo); break;
  case 15: update_depressible_floor (coord_bf, coord_mbo); break;
  case 16: update_depressible_floor (coord_bf, coord_mbo); break;
  case 17: update_depressible_floor (coord_bf, coord_mbo); break;
  }

  if (i++ == 17) {
    kid.action = kid_normal;
    i = 0;
  }
}

void
kid_vjump (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_vjump;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;
  misstep = false;

  static int i = 0, j = 0;
  static bool hit_ceiling = false;
  if (oaction != kid_vjump) i = j = 0, hit_ceiling = false;
  if (oaction == kid_hang_wall
      || oaction == kid_hang_free) i = 13;
  static bool hang = false;

  ALLEGRO_BITMAP *frame = vjump_frameset[i].frame;
  int dx = vjump_frameset[i].dx;
  int dy = vjump_frameset[i].dy;

  if (critical_edge && oaction == kid_hang_free)
    dx -= (kid.dir == LEFT) ? 9 : 13;
  if (critical_edge && i == 11) dx = +7;
  if (i == 12 && j++ > 0) dx = 0, dy += 2 * j + 1;

  int dir = (kid.dir == LEFT) ? +1 : -1;
  if (i == 0
      && dist_next_place (kid, coord_tf, pos, 0, true) < 23
      && is_hangable_pos (prel (kids.ptf, 0, dir), kid.dir)) {
    to_next_place_edge (&kid, frame, coord_tf, pos, 0, true, 0);
    if (crel (kids.ptf, 0, dir).fg == NO_FLOOR) {
        dx -= 12;
        critical_edge = true;
    } else {
      dx -= 3;
      critical_edge = false;
    }
    hang_pos = prel (kids.ptf, 0, dir);
    hang = true;
  } else if (i == 0 && can_hang (kid)
             && con (hang_pos).fg != NO_FLOOR
             && (kid.dir == LEFT || con (hang_pos).fg != DOOR)) {
    to_next_place_edge (&kid, frame, coord_tf, pos, 0, false, 0);
    dx -= 3; hang = true;
  }

  /* fall */
  if (kids.cm == NO_FLOOR
      && kids.cmf == NO_FLOOR
      && kids.cmba == NO_FLOOR) {
    kid_fall ();
    i = j = 0; hit_ceiling = hang = critical_edge = false;
    return;
  }

  /* collision */
  if (is_colliding (kid, coord_tf, pos, 0, false, -dx)
      && confg_collision == DOOR
      && kid.dir == RIGHT)
    to_collision_edge (&kid, frame, coord_tf, pos, 0, false, -dx);

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  depressible_loose_floor = false;

  switch (i) {
  case 0: update_depressible_floor (coord_bf, coord_bb); break;
  case 1: update_depressible_floor (coord_bf, coord_bb); break;
  case 2: update_depressible_floor (coord_bf, coord_bb); break;
  case 3: update_depressible_floor (coord_bf, coord_mbo); break;
  case 4: update_depressible_floor (coord_bf, coord_mbo); break;
  case 5: update_depressible_floor (coord_bf, coord_mbo); break;
  case 6: update_depressible_floor (coord_bf, coord_mbo); break;
  case 7: update_depressible_floor (coord_bf, coord_mbo); break;
  case 8: update_depressible_floor (NULL, coord_mbo); break;
  case 9: update_depressible_floor (NULL, coord_mbo); break;
  case 10: update_depressible_floor (NULL, coord_mbo); break;
  case 11: update_depressible_floor (NULL, NULL); break;
  case 12: update_depressible_floor (NULL, NULL); break;
  case 13: update_depressible_floor (NULL, NULL); break;
  case 14: update_depressible_floor (coord_bf, coord_mbo); break;
  case 15: update_depressible_floor (coord_bf, coord_bb); break;
  case 16: update_depressible_floor (coord_mbo, coord_bb); break;
  case 17: update_depressible_floor (coord_mbo, coord_bb); break;
  }

  if (i == 12 && hang) {
    kid.action = kid_hang;
    i = j = 0; hit_ceiling = hang = false;
    return;
  }
  /* is kid hitting the ceiling? */
  else if (i == 12 && j == 1
           && crel (kids.ptb, -1, 0).fg != NO_FLOOR) {
    hit_ceiling = true;
    i++;
  } else if (j == 4) j = 0, i++;
  else if (i != 12 && i < 17) i++;
  else if (i == 17) {
    shake_loose_floor_row (kids.pmbo);
    kid.action = kid_normal;
    hit_ceiling = hang = critical_edge = false; i = j = 0;
  }
}

void
kid_run_jump (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_run_jump;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  static int i = 0;
  if (oaction != kid_run_jump) i = 0;

  if (i < 4 || i > 9) inertia = 3;
  else inertia = 6;

  ALLEGRO_BITMAP *frame = run_jump_frameset[i].frame;
  int dx = run_jump_frameset[i].dx;
  int dy = run_jump_frameset[i].dy;

  /* fall */
  if ((kids.cbf == NO_FLOOR && i < 4)
      || (kids.cbf == NO_FLOOR && i > 9)) {
    kid_fall ();
    i = 0; return;
  }

  /* collision */
  if (is_colliding (kid, coord_tf, pos, 0, false, -dx)) {
    if (i < 6) kid_stabilize_collision ();
    else kid_couch_collision ();
    i = 0; return;
  }

  if (i == 0 || i == 4) play_sample (step);
  if (i == 10) shake_loose_floor_row (kids.pmbo);

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  depressible_loose_floor = (i < 4 || i > 9);

  switch (i) {
  case 0: update_depressible_floor (coord_bf, coord_mbo); break;
  case 1: update_depressible_floor (coord_mbo, coord_bb); break;
  case 2: update_depressible_floor (NULL, coord_bb); break;
  case 3: update_depressible_floor (coord_bf, NULL); break;
  case 4: update_depressible_floor (coord_bf, coord_bb); break;
  case 5: update_depressible_floor (coord_mbo, coord_bb); break;
  case 6: update_depressible_floor (NULL, NULL); break;
  case 7: update_depressible_floor (NULL, NULL); break;
  case 8: update_depressible_floor (NULL, NULL); break;
  case 9: update_depressible_floor (NULL, NULL); break;
  case 10: update_depressible_floor (coord_mbo, NULL); break;
  }

  if (i < 10) i++;
  else {
    kid.action = kid_run;
    i = 0;
  }
}

void
kid_misstep (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_misstep;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
  misstep = true;

  static int i = 0;
  if (oaction != kid_misstep) i = 0;

  ALLEGRO_BITMAP *frame = misstep_frameset[i].frame;
  int dx = misstep_frameset[i].dx;
  int dy = misstep_frameset[i].dy;

  /* fall */
  if (kids.cmba == NO_FLOOR) {
    kid_fall ();
    i = 0; return;
  }

  if (i == 7) play_sample (step);
  if (i == 8) shake_loose_floor_row (kids.pmbo);

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  keep_depressible_floor ();

  if (i < 10) i++;
  else {
    kid.action = kid_normal;
    to_con_edge (&kid, kid_normal_00, coord_bf, pos, -4, false, 0, LOOSE_FLOOR)
      || to_con_edge (&kid, kid_normal_00, coord_bf, pos, -4, false, 0, NO_FLOOR);
    i = 0;
  }
}

void
kid_hang (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_hang;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;
  misstep = false;
  inertia = 0;

  static int i = 0;

  kid.frame = kid_hang_14;
  int dir = (kid.dir == LEFT) ? 0 : 1;
  kid.c.x = PLACE_WIDTH * (hang_pos.place + dir) + 7;
  kid.c.y = PLACE_HEIGHT * hang_pos.floor - 9;

  critical_edge = (con (hang_pos).fg == NO_FLOOR);

  if (i == 0 && oaction != kid_unclimb)
    kid = next_anim (kid, kid_hang_14, +0, +0);
  else if (kids.cmf == WALL
           || (kids.cmf == DOOR
               && kid.dir == LEFT)) kid_hang_wall ();
  else kid_hang_free ();

  /* depressible floors */
  update_depressible_floor (NULL, NULL);
  dir = (kid.dir == LEFT) ? -1 : +1;
  struct pos hanged_con_pos = prel (hang_pos, -1, dir);
  press_depressible_floor (hanged_con_pos);

  /* next frame */
  if (i < 1) i++;
  else i = 0;
}

void
kid_hang_wall (void)
{
  kid.action = kid_hang_wall;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  static bool reverse = false;
  static int i = 4;
  static int wait = 0;
  if (kid.action != kid_hang_wall) {
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
      kid_fall ();
      hang_limit = false;
      return;
    }
    kid.frame = vjump_frameset[13].frame;
    kid.c.x += (kid.dir == LEFT) ? +12 : -12;
    kid.c.y = PLACE_HEIGHT * kids.pm.floor - 8;
    kid_vjump ();
    hang_limit = false;
    return;
  } if (up_key) {
    kid_climb ();
    reverse = false; i = 4; wait = 0;
    return;
  }

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  update_depressible_floor (NULL, NULL);
  int dir = (kid.dir == LEFT) ? -1 : +1;
  struct pos hanged_con_pos = prel (hang_pos, -1, dir);
  press_depressible_floor (hanged_con_pos);

  /* next frame */
  if (! reverse && i < 6) i++;
  else if (! reverse && i == 6) {
   reverse = true;
  } else if (reverse && i > 4 && wait == 0) {
    i--; wait = 1;
  } else if (reverse && i >= 4 && wait > 0) wait--;
}

void
kid_hang_free (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_hang_free;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  static bool reverse = true;
  static int i = 4;
  static int j = -1;
  static int wait = 3;
  if (oaction != kid_hang_free) {
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
      kid_fall ();
      hang_limit = false;
      return;
    }
    kid.frame = vjump_frameset[13].frame;
    kid.c.x += (kid.dir == LEFT) ? +1 : -1;
    kid.c.y = PLACE_HEIGHT * kids.pm.floor - 8;
    kid_vjump ();
    hang_limit = false;
    reverse = true; i = 4; j = -1; wait = 3;
    return;
  } if (up_key) {
    kid_climb ();
    reverse = true; i = 4; j = -1; wait = 3;
    return;
  }

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  update_depressible_floor (NULL, NULL);
  int dir = (kid.dir == LEFT) ? -1 : +1;
  struct pos hanged_con_pos = prel (hang_pos, -1, dir);
  press_depressible_floor (hanged_con_pos);

  /* next frame */
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

void
kid_climb (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_climb;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  static int i = 0;
  static int wait = DOOR_WAIT_LOOK;
  if (oaction != kid_climb) i = 0;

  if (i == 15) {
    kid_couch ();
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
    kid.frame = climb_frameset[0].frame;
    int dir = (kid.dir == LEFT) ? 0 : 1;
    kid.c.x = PLACE_WIDTH * (hang_pos.place + dir) + 9;
    kid.c.y = PLACE_HEIGHT * hang_pos.floor - 9;
  }

  if (i == 3 && wait < DOOR_WAIT_LOOK) dx = 0, dy = 0;

  /* fall */
  if (kids.ctf == NO_FLOOR) {
    kid_fall ();
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
      kid_unclimb ();
      return;
    } else if (wait > 0) wait--;
  }

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  update_depressible_floor (NULL, NULL);
  int dir = (kid.dir == LEFT) ? -1 : +1;
  struct pos hanged_con_pos = prel (hang_pos, -1, dir);
  press_depressible_floor (hanged_con_pos);

  /* next frame */
  if (wait == DOOR_WAIT_LOOK && i < 15) i++;
}

void
kid_unclimb (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_unclimb;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  static int i = 13;
  if (oaction != kid_unclimb) i = 13;
  if (oaction == kid_climb) i = 2;

  if (i == 0) {
    i = 13;
    /* in this function the hang_pos variable holds the position of
       the hanged construction instead of the usual hangable position
       facing it */
    hang_pos =
      npos (prel (hang_pos, +1, (kid.dir == LEFT) ? +1 : -1));
    kid_hang ();
    return;
  }

  ALLEGRO_BITMAP *frame = climb_frameset[i].frame;
  int dx = -climb_frameset[i + 1].dx;
  int dy = -climb_frameset[i + 1].dy;

  if (i == 13) {
    kid.frame = climb_frameset[13].frame;
    kid.c.x = PLACE_WIDTH * hang_pos.place + 18;
    kid.c.y = PLACE_HEIGHT * hang_pos.floor + 25;
  }

  /* fall */
  if (kids.ctf == NO_FLOOR) {
    kid_fall ();
    i = 13;
    return;
  }

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  update_depressible_floor (NULL, NULL);
  press_depressible_floor (hang_pos);

  /* next frame */
  i--;
}

void
kid_stabilize (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_stabilize;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;
  misstep = false;
  inertia = 0;

  static int i = 0;
  static bool collision = false;
  if (oaction != kid_stabilize) i = 0;
  if (oaction == kid_stabilize_collision) i = 1, collision = true;

  ALLEGRO_BITMAP *frame = stabilize_frameset[i].frame;
  int dx = stabilize_frameset[i].dx;
  int dy = stabilize_frameset[i].dy;

  if (kid.frame == stop_run_frameset[3].frame) dx = -5;

  /* collision */
  if (is_colliding (kid, coord_tf, pos, 0, false, -dx))
    to_collision_edge (&kid, frame, coord_tf, pos, 0, false, -dx);

  /* actions */
  if (! turn)
    turn = ((kid.dir == RIGHT) && left_key)
      || ((kid.dir == LEFT) && right_key);
  bool walk = ((kid.dir == RIGHT) && right_key && shift_key)
                || ((kid.dir == LEFT) && left_key && shift_key);
  bool run = (((kid.dir == RIGHT) && right_key)
              || ((kid.dir == LEFT) && left_key)) && ! walk;
  bool jump = ((kid.dir == RIGHT) && right_key && up_key)
    || ((kid.dir == LEFT) && left_key && up_key);
  bool couch = down_key;

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  keep_depressible_floor ();

  int dc = dist_collision (kid, coord_bb, pos, 0, false);
  int df = dist_con (kid, coord_bb, pos, -4, false, NO_FLOOR);

  if (! collision) {
    if (couch) {
      kid.action = kid_couch;
      i = 0; return;
    } else if (jump) {
      kid.action = kid_jump;
      i = 0; return;
    } else if (turn) {
      kid.action = kid_turn;
      i = 0; turn = false; return;
    } else if (walk && i == 3) {
      kid.action = kid_walk;
      i = 0; return;
    } else if (run && dc > PLACE_WIDTH && df > PLACE_WIDTH) {
      kid.action = kid_start_run;
      i = 0; return;
    }
  }

  if (i < 3) i++;
  else if (i == 3) {
    kid.action = kid_normal;
    i = 0; collision = turn = false; return;
  }
}

void
kid_couch (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_couch;
  kid.flip = (kid.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;
  misstep = false;

  static int i = 0;
  static int wait = 1;
  static bool collision = false;
  static bool fall = false;
  static int cinertia = 0;
  if (oaction != kid_couch) i = 0;
  if (oaction == kid_climb) i = 11, critical_edge = false;
  if (oaction == kid_couch_collision)
    collision = true, inertia = 0;
  if (oaction == kid_fall)
    fall = true, inertia = 0;

  ALLEGRO_BITMAP *frame = couch_frameset[i].frame;
  int dx = couch_frameset[i].dx;
  int dy = couch_frameset[i].dy;

  if (oaction == kid_climb) dx += 7;
  if (i == 0) cinertia = 2.6 * inertia;
  if (collision || fall) cinertia = 0;
  if (i > 0 && i < 3) dx -= cinertia ? cinertia : 0;

  /* unclimb */
  int dir = (kid.dir == LEFT) ? +1 : -1;
  if (oaction != kid_couch_collision
      && oaction != kid_fall
      && i == 0
      && oaction != kid_fall
      && crel (kids.pbf, 0, dir).fg == NO_FLOOR
      && dist_next_place (kid, coord_tf, pos, 0, true) < 22
      && ! (kids.ctf == DOOR && kid.dir == LEFT
            && door_at_pos (kids.ptf)->i > DOOR_CLIMB_LIMIT)) {
    hang_pos = kids.pbf;
    critical_edge =
      (crel (hang_pos, +1, dir).fg == NO_FLOOR);
    kid_unclimb ();
    return;
  }

  /* fall */
  if (kids.cm == NO_FLOOR && ! (fall && i == 0)) {
    if (collision) to_collision_edge (&kid, fall_frameset[0].frame, coord_tf, pos, 0, false, 0);
    kid_fall ();
    i = 0; collision = false; return;
  }

  /* collision */

  /* ensure kid is not colliding */
  if (con (pos (coord_bf (kid))).fg == WALL)
    to_next_place_edge (&kid, frame, coord_bf, pos, 0, true, -1);

  /* wall pushes back */
  if (is_colliding (kid, coord_tf, pos, 0, false, -dx))
    to_collision_edge (&kid, frame, coord_tf, pos, 0, false, -dx);

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  depressible_loose_floor = true;

  switch (i) {
  case 0: update_depressible_floor (coord_mbo, coord_mbo); break;
  case 1: update_depressible_floor (coord_mbo, coord_mbo); break;
  case 2: update_depressible_floor (coord_bf, coord_mbo); break;
  case 3: update_depressible_floor (coord_bf, coord_bb); break;
  case 4: update_depressible_floor (coord_bf, coord_bb); break;
  case 5: update_depressible_floor (coord_mbo, coord_bb); break;
  case 6: update_depressible_floor (coord_mbo, coord_bb); break;
  case 7: update_depressible_floor (coord_mbo, coord_bb); break;
  case 8: update_depressible_floor (coord_bf, coord_mbo); break;
  case 9: update_depressible_floor (coord_bf, coord_mbo); break;
  case 10: update_depressible_floor (coord_bf, coord_mbo); break;
  case 11: update_depressible_floor (coord_bf, coord_bb); break;
  case 12: update_depressible_floor (coord_bf, coord_bb); break;
  }

  if (i == 12) {
    kid.action = kid_normal;
    cinertia = 0; i = 0; collision = fall = false;
  } else if (i == 2 && down_key
           && ((kid.dir == LEFT && left_key)
               || (kid.dir == RIGHT && right_key))
             && wait-- == 0) {
    if (! is_colliding (kid, coord_tf, pos, 0, false, 6))
      kid.c.x += (kid.dir == LEFT) ? -6 : +6;
    i = 0; wait = 1; collision = fall = false;
  }
  else if (i != 2 || ! down_key) i++;

  if (cinertia > 0) cinertia--;
}

void
kid_stabilize_collision (void)
{
  kid.action = kid_stabilize_collision;

  play_sample (hit_wall);
  to_collision_edge (&kid, stabilize_frameset[0].frame, coord_tf, pos, 0, false, 0);
  if (kid.dir == RIGHT) kid.c.x -= 3;

  kid_stabilize ();
}

void
kid_couch_collision (void)
{
  kid.action = kid_couch_collision;

  kid.c.y = PLACE_HEIGHT * kids.pbf.floor + 15;
  kid.c.x = PLACE_WIDTH * kids.pbf.place + 15;
  kid.frame = kid_normal_00;

  play_sample (hit_wall);
  to_collision_edge (&kid, couch_frameset[0].frame, coord_tf, pos, 0, false, 0);
  if (kid.dir == RIGHT) kid.c.x -= 12;

  kid_couch ();
}

void
kid_fall (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_fall;
  kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  static int i = 0;
  static int force_floor = -2;

  ALLEGRO_BITMAP *frame = fall_frameset[i > 4 ? 4 : i].frame;
  int dx = fall_frameset[i > 4 ? 4 : i].dx;
  int dy = fall_frameset[i > 4 ? 4 : i].dy;

  int dir = (kid.dir == LEFT) ? -1 : +1;

  /* put kid in front of the floor */
  if (i == 0
      && con (pos (coord_bb (kid))).fg != NO_FLOOR) {
    kid = next_anim (kid, frame, +0, 0);
    int dir = (kid.dir == LEFT) ? -1 : +1;
    int x = kid.c.x;
    while (con (pos (coord_bb (kid))).fg != NO_FLOOR
           && abs (kid.c.x - x) < PLACE_WIDTH)
      kid.c.x += dir;
    kid.c.x += -dir * 12;
  }

  /* ensure kid's proximity for hang */
  if (i == 0 && crel (kids.ptf, 0, dir).fg != NO_FLOOR
      && crel (kids.ptf, 0, dir).fg != WALL
      && oaction == kid_walk) {
    to_next_place_edge (&kid, frame, coord_tf, pos, 0, false, 0);
  } else if (kids.ctf != NO_FLOOR) inertia = 0;
  else {
    if (oaction == kid_run_jump) dx = -12, dy= +12;
    if (oaction == kid_jump) dx = -12, dy = +12;
  }

  /* hang */
  if (i > 0 && can_hang (kid) && shift_key && ! hang_limit) {
    play_sample (hang_on_fall);
    kid_hang ();
    i = 0; force_floor = -2;
    return;
  }

  if (i == 1) dx = -inertia;
  if (i > 1) dx = -inertia;
  if (i > 4) {
    int speed = +21 + 3 * (i - 5);
    dy = (speed > 33) ? 33 : speed;
  }

  /* turn run */
  if (oaction == kid_turn_run) {
    force_floor = kids.pbf.floor;
    if (kid.frame != turn_run_frameset[8].frame)
      kid.dir = (kid.dir == LEFT) ? RIGHT : LEFT;
    kid.flip = (kid.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
    kid.c.x += (kid.dir == LEFT) ? +12 : -12;
    kid.c.y += 12;
  }

  struct pos pmbo =
    pos (coord_mbo (next_anim (kid, kid.frame, 0, 34)));

  /* land on ground */
  if (i > 0
      && kids.cmbo != NO_FLOOR
      /* && kids.cmbo != WALL */
      && kids.pmbo.floor != pmbo.floor
      && kids.pmbo.floor != force_floor) {
    inertia = 0;

    kid.c.y = 63 * kids.pbf.floor + 15;
    kid.c.x += (kid.dir == LEFT) ? -6 : +6;
    kid.frame = kid_normal_00;

    /* ensure kid isn't colliding */
    if (con (pos (coord_bf (kid))).fg == WALL
        || con (pos (coord_bf (kid))).fg == DOOR)
      to_next_place_edge (&kid, frame, coord_bf, pos, 0, true, -1);

    kid_couch ();
    if (i > 3) play_sample (hit_ground);
    i = 0; force_floor = -2;
    shake_loose_floor_row (kids.pbf);
    return;
  }

  /* collision */
  struct anim a = next_anim (kid, frame, dx, dy);
  if (con (pos (coord_bf (a))).fg == WALL
      || (a.dir == LEFT &&
          con (pos (coord_bf (a))).fg == DOOR)) {
    to_next_place_edge (&a, frame, coord_bf, pos, 0, true, -1);
    kid = a; dx = 0; dy = 0;
  }

  /* if (is_colliding (kid, coord_bf, pos, 0, false, -dx)) */
  /*   to_collision_edge (&kid, frame, coord_bf, pos, 0, false, -dx); */

  kid = next_anim (kid, frame, dx, dy);

  /* depressible floors */
  update_depressible_floor (NULL, NULL);

  /* next frame */
  i++;
  /* if (inertia > 0 && i % 3) inertia--; */
}




bool
is_kid_fall (struct anim a)
{
  return a.frame == kid_fall_13
    || a.frame == kid_fall_14
    || a.frame == kid_fall_15
    || a.frame == kid_fall_16
    || a.frame == kid_fall_17;
}

bool
is_kid_hang_or_climb (struct anim a)
{
  return a.frame == kid_hang_00
    || a.frame == kid_hang_01
    || a.frame == kid_hang_02
    || a.frame == kid_hang_03
    || a.frame == kid_hang_04
    || a.frame == kid_hang_05
    || a.frame == kid_hang_06
    || a.frame == kid_hang_07
    || a.frame == kid_hang_08
    || a.frame == kid_hang_09
    || a.frame == kid_hang_10
    || a.frame == kid_hang_11
    || a.frame == kid_hang_12
    || a.frame == kid_hang_14

    || a.frame == kid_climb_01
    || a.frame == kid_climb_02
    || a.frame == kid_climb_03
    || a.frame == kid_climb_04
    || a.frame == kid_climb_05
    || a.frame == kid_climb_06
    || a.frame == kid_climb_07
    || a.frame == kid_climb_08
    || a.frame == kid_climb_09
    || a.frame == kid_climb_10;
}

bool
is_kid_climb (struct anim a)
{
  return a.frame == kid_climb_03
    || a.frame == kid_climb_04
    || a.frame == kid_climb_05
    || a.frame == kid_climb_06
    || a.frame == kid_climb_07
    || a.frame == kid_climb_08
    || a.frame == kid_climb_09
    || a.frame == kid_climb_10;
}

bool
is_kid_hanging_at_pos (struct anim a, struct pos p)
{
  struct pos pbb = pos (coord_bb (a));
  int dir = (a.dir == LEFT) ? -1 : +1;
  return (is_kid_hang_or_climb (a)
          && peq (prel (pbb, -1, dir), p));
}




static void
update_depressible_floor
(struct coord (*coord_f0) (struct anim a),
 struct coord (*coord_f1) (struct anim a))
{
  struct pos p0, p1;

  if (coord_f0) p0 = pos (coord_f0 (kid));
  if (coord_f1) p1 = pos (coord_f1 (kid));

  if (coord_f0
      && (con (p0).fg != LOOSE_FLOOR
          || depressible_loose_floor)) {
    press_depressible_floor (p0);
    depressible_floor_pos[0] = p0;
  }

  if (coord_f1
      && (con (p1).fg != LOOSE_FLOOR
          || depressible_loose_floor)) {
    press_depressible_floor (p1);
    depressible_floor_pos[1] = p1;
  }

  if (! coord_f0 ||
      (con (p0).fg != OPENER_FLOOR
       && con (p0).fg != LOOSE_FLOOR))
    depressible_floor_pos[0].room = -1;

  if (! coord_f1 ||
      (con (p1).fg != OPENER_FLOOR
       && con (p1).fg != LOOSE_FLOOR))
    depressible_floor_pos[1].room = -1;
}

static void
keep_depressible_floor (void)
{
  struct pos p0 = depressible_floor_pos[0];
  struct pos p1 = depressible_floor_pos[1];

  if (p0.room != -1) press_depressible_floor (p0);
  if (p1.room != -1) press_depressible_floor (p1);
}

static void
press_depressible_floor (struct pos p)
{
  switch (con (p).fg) {
  case OPENER_FLOOR: press_opener_floor (p); break;
  case LOOSE_FLOOR: release_loose_floor (p); break;
  default: break;
  }
}

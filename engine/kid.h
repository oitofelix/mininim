/*
  kid.h -- kid module;

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

#ifndef FREEPOP_KID_H
#define FREEPOP_KID_H

/* bitmap resources */
#define KID_NORMAL "dat/kid/normal.png"
#define KID_START_RUN_01 "dat/kid/running/frame01.png"
#define KID_START_RUN_02 "dat/kid/running/frame02.png"
#define KID_START_RUN_03 "dat/kid/running/frame03.png"
#define KID_START_RUN_04 "dat/kid/running/frame04.png"
#define KID_START_RUN_05 "dat/kid/running/frame05.png"
#define KID_START_RUN_06 "dat/kid/running/frame06.png"
#define KID_RUN_07 "dat/kid/running/frame07.png"
#define KID_RUN_08 "dat/kid/running/frame08.png"
#define KID_RUN_09 "dat/kid/running/frame09.png"
#define KID_RUN_10 "dat/kid/running/frame10.png"
#define KID_RUN_11 "dat/kid/running/frame11.png"
#define KID_RUN_12 "dat/kid/running/frame12.png"
#define KID_RUN_13 "dat/kid/running/frame13.png"
#define KID_RUN_14 "dat/kid/running/frame14.png"
#define KID_TURN_01 "dat/kid/turning/frame01.png"
#define KID_TURN_02 "dat/kid/turning/frame02.png"
#define KID_TURN_03 "dat/kid/turning/frame03.png"
#define KID_TURN_04 "dat/kid/turning/frame04.png"
#define KID_STABILIZE_05 "dat/kid/turning/frame05.png"
#define KID_STABILIZE_06 "dat/kid/turning/frame06.png"
#define KID_STABILIZE_07 "dat/kid/turning/frame07.png"
#define KID_STABILIZE_08 "dat/kid/turning/frame08.png"
#define KID_STOP_RUN_01 "dat/kid/turn running/frame01.png"
#define KID_STOP_RUN_02 "dat/kid/turn running/frame02.png"
#define KID_STOP_RUN_03 "dat/kid/turn running/frame03.png"
#define KID_STOP_RUN_04 "dat/kid/turn running/frame04.png"
#define KID_TURN_RUN_05 "dat/kid/turn running/frame05.png"
#define KID_TURN_RUN_06 "dat/kid/turn running/frame06.png"
#define KID_TURN_RUN_07 "dat/kid/turn running/frame07.png"
#define KID_TURN_RUN_08 "dat/kid/turn running/frame08.png"
#define KID_TURN_RUN_09 "dat/kid/turn running/frame09.png"
#define KID_TURN_RUN_10 "dat/kid/turn running/frame10.png"
#define KID_TURN_RUN_11 "dat/kid/turn running/frame11.png"
#define KID_TURN_RUN_12 "dat/kid/turn running/frame12.png"
#define KID_TURN_RUN_13 "dat/kid/turn running/frame13.png"
#define KID_WALK_01 "dat/kid/walking a step/frame01.png"
#define KID_WALK_02 "dat/kid/walking a step/frame02.png"
#define KID_WALK_03 "dat/kid/walking a step/frame03.png"
#define KID_WALK_04 "dat/kid/walking a step/frame04.png"
#define KID_WALK_05 "dat/kid/walking a step/frame05.png"
#define KID_WALK_06 "dat/kid/walking a step/frame06.png"
#define KID_WALK_07 "dat/kid/walking a step/frame07.png"
#define KID_WALK_08 "dat/kid/walking a step/frame08.png"
#define KID_WALK_09 "dat/kid/walking a step/frame09.png"
#define KID_WALK_10 "dat/kid/walking a step/frame10.png"
#define KID_WALK_11 "dat/kid/walking a step/frame11.png"
#define KID_WALK_12 "dat/kid/walking a step/frame12.png"
#define KID_JUMP_01 "dat/kid/simple jump/frame01.png"
#define KID_JUMP_02 "dat/kid/simple jump/frame02.png"
#define KID_JUMP_03 "dat/kid/simple jump/frame03.png"
#define KID_JUMP_04 "dat/kid/simple jump/frame04.png"
#define KID_JUMP_05 "dat/kid/simple jump/frame05.png"
#define KID_JUMP_06 "dat/kid/simple jump/frame06.png"
#define KID_JUMP_07 "dat/kid/simple jump/frame07.png"
#define KID_JUMP_08 "dat/kid/simple jump/frame08.png"
#define KID_JUMP_09 "dat/kid/simple jump/frame09.png"
#define KID_JUMP_10 "dat/kid/simple jump/frame10.png"
#define KID_JUMP_11 "dat/kid/simple jump/frame11.png"
#define KID_JUMP_12 "dat/kid/simple jump/frame12.png"
#define KID_JUMP_13 "dat/kid/simple jump/frame13.png"
#define KID_JUMP_14 "dat/kid/simple jump/frame14.png"
#define KID_JUMP_15 "dat/kid/simple jump/frame15.png"
#define KID_JUMP_16 "dat/kid/simple jump/frame16.png"
#define KID_JUMP_17 "dat/kid/simple jump/frame17.png"
#define KID_JUMP_18 "dat/kid/simple jump/frame18.png"
#define KID_FALL_13 "dat/kid/hanging and falling/frame13.png"
#define KID_FALL_14 "dat/kid/hanging and falling/frame14.png"
#define KID_FALL_15 "dat/kid/hanging and falling/frame15.png"
#define KID_FALL_16 "dat/kid/hanging and falling/frame16.png"
#define KID_FALL_17 "dat/kid/hanging and falling/frame17.png"
#define KID_COUCH_01 "dat/kid/couching/frame01.png"
#define KID_COUCH_02 "dat/kid/couching/frame02.png"
#define KID_COUCH_03 "dat/kid/couching/frame03.png"
#define KID_COUCH_04 "dat/kid/couching/frame04.png"
#define KID_COUCH_05 "dat/kid/couching/frame05.png"
#define KID_COUCH_06 "dat/kid/couching/frame06.png"
#define KID_COUCH_07 "dat/kid/couching/frame07.png"
#define KID_COUCH_08 "dat/kid/couching/frame08.png"
#define KID_COUCH_09 "dat/kid/couching/frame09.png"
#define KID_COUCH_10 "dat/kid/couching/frame10.png"
#define KID_COUCH_11 "dat/kid/couching/frame11.png"
#define KID_COUCH_12 "dat/kid/couching/frame12.png"
#define KID_COUCH_13 "dat/kid/couching/frame13.png"
#define KID_VJUMP_01 "dat/kid/scaling/frame01.png"
#define KID_VJUMP_02 "dat/kid/scaling/frame02.png"
#define KID_VJUMP_03 "dat/kid/scaling/frame03.png"
#define KID_VJUMP_04 "dat/kid/scaling/frame04.png"
#define KID_VJUMP_05 "dat/kid/scaling/frame05.png"
#define KID_VJUMP_06 "dat/kid/scaling/frame06.png"
#define KID_VJUMP_07 "dat/kid/scaling/frame07.png"
#define KID_VJUMP_08 "dat/kid/scaling/frame08.png"
#define KID_VJUMP_09 "dat/kid/scaling/frame09.png"
#define KID_VJUMP_10 "dat/kid/scaling/frame10.png"
#define KID_VJUMP_11 "dat/kid/scaling/frame11.png"
#define KID_VJUMP_12 "dat/kid/scaling/frame12.png"
#define KID_VJUMP_13 "dat/kid/scaling/frame13.png"
#define KID_VJUMP_15 "dat/kid/scaling/frame15.png"
#define KID_VJUMP_16 "dat/kid/scaling/frame16.png"
#define KID_VJUMP_17 "dat/kid/scaling/frame17.png"
#define KID_VJUMP_18 "dat/kid/scaling/frame18.png"
#define KID_VJUMP_19 "dat/kid/scaling/frame19.png"
#define KID_HANG_00 "dat/kid/hanging and falling/frame00.png"
#define KID_HANG_01 "dat/kid/hanging and falling/frame01.png"
#define KID_HANG_02 "dat/kid/hanging and falling/frame02.png"
#define KID_HANG_03 "dat/kid/hanging and falling/frame03.png"
#define KID_HANG_04 "dat/kid/hanging and falling/frame04.png"
#define KID_HANG_05 "dat/kid/hanging and falling/frame05.png"
#define KID_HANG_06 "dat/kid/hanging and falling/frame06.png"
#define KID_HANG_07 "dat/kid/hanging and falling/frame07.png"
#define KID_HANG_08 "dat/kid/hanging and falling/frame08.png"
#define KID_HANG_09 "dat/kid/hanging and falling/frame09.png"
#define KID_HANG_10 "dat/kid/hanging and falling/frame10.png"
#define KID_HANG_11 "dat/kid/hanging and falling/frame11.png"
#define KID_HANG_12 "dat/kid/hanging and falling/frame12.png"
#define KID_HANG_14 "dat/kid/scaling/frame14.png"
#define KID_CLIMB_01 "dat/kid/clipping/frame01.png"
#define KID_CLIMB_02 "dat/kid/clipping/frame02.png"
#define KID_CLIMB_03 "dat/kid/clipping/frame03.png"
#define KID_CLIMB_04 "dat/kid/clipping/frame04.png"
#define KID_CLIMB_05 "dat/kid/clipping/frame05.png"
#define KID_CLIMB_06 "dat/kid/clipping/frame06.png"
#define KID_CLIMB_07 "dat/kid/clipping/frame07.png"
#define KID_CLIMB_08 "dat/kid/clipping/frame08.png"
#define KID_CLIMB_09 "dat/kid/clipping/frame09.png"
#define KID_CLIMB_10 "dat/kid/clipping/frame10.png"
#define KID_CLIMB_11 "dat/kid/clipping/frame11.png"
#define KID_CLIMB_12 "dat/kid/clipping/frame12.png"
#define KID_CLIMB_13 "dat/kid/clipping/frame13.png"
#define KID_CLIMB_14 "dat/kid/clipping/frame14.png"
#define KID_CLIMB_15 "dat/kid/clipping/frame15.png"
#define KID_RUN_JUMP_01 "dat/kid/running jump/frame01.png"
#define KID_RUN_JUMP_02 "dat/kid/running jump/frame02.png"
#define KID_RUN_JUMP_03 "dat/kid/running jump/frame03.png"
#define KID_RUN_JUMP_04 "dat/kid/running jump/frame04.png"
#define KID_RUN_JUMP_05 "dat/kid/running jump/frame05.png"
#define KID_RUN_JUMP_06 "dat/kid/running jump/frame06.png"
#define KID_RUN_JUMP_07 "dat/kid/running jump/frame07.png"
#define KID_RUN_JUMP_08 "dat/kid/running jump/frame08.png"
#define KID_RUN_JUMP_09 "dat/kid/running jump/frame09.png"
#define KID_RUN_JUMP_10 "dat/kid/running jump/frame10.png"
#define KID_RUN_JUMP_11 "dat/kid/running jump/frame11.png"

/* sound resources */
#define STEP "dat/digisnd1/running 2.ogg"
#define HIT_GROUND "dat/digisnd1/running 1.ogg"
#define HIT_WALL "dat/digisnd1/touching a wall.ogg"
#define HANG_ON_FALL "dat/digisnd1/normal falling.ogg"

/* functions */
void load_kid (void);
void unload_kid (void);
void init_climb_frameset (void);
void init_hang_frameset (void);

void place_kid (int room, int floor, int place);

void draw_kid_stabilize (void);
void draw_kid_vjump (void);
void draw_kid_ceiling (void);
void draw_kid_normal (void);
void draw_kid_walk (void);
void draw_kid_walk_min (void);
void draw_kid_walk_short_short (void);
void draw_kid_walk_short (void);
void draw_kid_walk_long (void);
void draw_kid_walk_long_long (void);
void draw_kid_walk_max (void);
void draw_kid_start_run (void);
void draw_kid_stop_run (void);
void draw_kid_run (void);
void draw_kid_turn (void);
void draw_kid_turn_run (void);
void draw_kid_jump (void);
void draw_kid_collision (void);
void draw_kid_back_collision (void);
void draw_kid_fall (void);
void draw_kid_couch (void);
void draw_kid_misstep (void);
void draw_kid_hang (void);
void draw_kid_hang_wall (void);
void draw_kid_hang_free (void);
void draw_kid_climb (void);
void draw_kid_unclimb (void);
void draw_kid_run_jump (void);

bool is_kid_colliding (int min_dist);
int dist_kid_collision (void);
void kid_to_collision_edge (int min_dist);
void draw_kid_stabilize_collision (void);
void draw_kid_couch_collision (void);
bool is_hangable_pos_for_kid (struct pos p);
bool can_kid_hang (void);

bool is_kid_normal ();
bool is_kid_start_walk (void);
bool is_kid_walk (void);
bool is_kid_stop_walk (void);
bool is_kid_run (void);
bool is_kid_start_couch (void);
bool is_kid_couch (void);
bool is_kid_stop_couch (void);
bool is_kid_stop_run (void);
bool is_kid_fall (void);
bool is_kid_start_jump (void);
bool is_kid_jump (void);
bool is_kid_stop_jump (void);
bool is_kid_start_vjump (void);
bool is_kid_vjump (void);
bool is_kid_stop_vjump (void);
bool is_kid_stabilize (void);
bool is_kid_turn (void);
bool is_kid_turn_run (void);
bool is_kid_hang (void);
bool is_kid_start_climb (void);
bool is_kid_climb (void);
bool is_kid_stop_climb (void);
bool is_kid_start_run_jump (void);
bool is_kid_run_jump ();
bool is_kid_stop_run_jump ();
bool is_kid_on_air (void);
bool is_kid_hanging_at_pos (struct pos p);

/* variables */
extern struct anim kid; /* kid animation object */

extern ALLEGRO_BITMAP *kid_normal,
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

extern bool hang_limit;
extern struct survey kids, kidsf;

#endif	/* FREEPOP_KID_H */

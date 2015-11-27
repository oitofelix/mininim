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
#define KID_NORMAL_00 "dat/kid/normal.png"
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
#define KID_DRINK_01 "dat/kid/drinking/frame01.png"
#define KID_DRINK_02 "dat/kid/drinking/frame02.png"
#define KID_DRINK_03 "dat/kid/drinking/frame03.png"
#define KID_DRINK_04 "dat/kid/drinking/frame04.png"
#define KID_DRINK_05 "dat/kid/drinking/frame05.png"
#define KID_DRINK_06 "dat/kid/drinking/frame06.png"
#define KID_DRINK_07 "dat/kid/drinking/frame07.png"
#define KID_DRINK_08 "dat/kid/drinking/frame08.png"
#define KID_DRINK_09 "dat/kid/drinking/frame09.png"
#define KID_DRINK_10 "dat/kid/drinking/frame10.png"
#define KID_DRINK_11 "dat/kid/drinking/frame11.png"
#define KID_DRINK_12 "dat/kid/drinking/frame12.png"
#define KID_DRINK_13 "dat/kid/drinking/frame13.png"
#define KID_DRINK_14 "dat/kid/drinking/frame14.png"
#define KID_DRINK_15 "dat/kid/drinking/frame15.png"
#define KID_RAISE_SWORD_01 "dat/kid/got sword/frame01.png"
#define KID_RAISE_SWORD_02 "dat/kid/got sword/frame02.png"
#define KID_RAISE_SWORD_03 "dat/kid/got sword/frame03.png"
#define KID_RAISE_SWORD_04 "dat/kid/got sword/frame04.png"
#define KID_KEEP_SWORD_01 "dat/kid/got sword/frame05.png"
#define KID_KEEP_SWORD_02 "dat/kid/got sword/frame06.png"
#define KID_KEEP_SWORD_03 "dat/kid/got sword/frame07.png"
#define KID_KEEP_SWORD_04 "dat/kid/got sword/frame08.png"
#define KID_KEEP_SWORD_05 "dat/kid/putting down sword/frame01.png"
#define KID_KEEP_SWORD_06 "dat/kid/putting down sword/frame02.png"
#define KID_KEEP_SWORD_07 "dat/kid/putting down sword/frame03.png"
#define KID_KEEP_SWORD_08 "dat/kid/putting down sword/frame04.png"
#define KID_KEEP_SWORD_09 "dat/kid/putting down sword/frame05.png"
#define KID_KEEP_SWORD_10 "dat/kid/putting down sword/frame06.png"
#define KID_TAKE_SWORD_01 "dat/kid/taking sword out/frame01.png"
#define KID_TAKE_SWORD_02 "dat/kid/taking sword out/frame02.png"
#define KID_TAKE_SWORD_03 "dat/kid/taking sword out/frame03.png"
#define KID_TAKE_SWORD_04 "dat/kid/taking sword out/frame04.png"
#define KID_SWORD_NORMAL_08 "dat/kid/sword attacking/frame08.png"
#define KID_SWORD_WALKF_14 "dat/kid/sword attacking/frame14.png"
#define KID_SWORD_WALKF_15 "dat/kid/sword attacking/frame15.png"
#define KID_SWORD_WALKB_10 "dat/kid/sword attacking/frame10.png"
#define KID_SWORD_WALKB_07 "dat/kid/sword attacking/frame07.png"

/* sound resources */
#define STEP "dat/digisnd1/running 2.ogg"
#define HIT_GROUND "dat/digisnd1/running 1.ogg"
#define HIT_WALL "dat/digisnd1/touching a wall.ogg"
#define HANG_ON_FALL "dat/digisnd1/normal falling.ogg"
#define DRINK "dat/digisnd3/drinking.ogg"
#define GLORY "dat/midisnd1/guard death 1.ogg"
#define TAKE_SWORD "dat/digisnd1/taking out the sword.ogg"

/* functions */
void load_kid (void);
void unload_kid (void);
bool is_kid_fall (struct anim a);
bool is_kid_hang_or_climb (struct anim a);
bool is_kid_climb (struct anim a);
bool is_kid_hanging_at_pos (struct anim a, struct pos p);

void kid_turn (void);
void kid_vjump (void);

/* variables */
extern struct anim kid; /* kid animation object */
extern struct survey kids, kidsf;

extern ALLEGRO_BITMAP *kid_climb_03, *kid_climb_04,
  *kid_climb_05, *kid_climb_06, *kid_climb_07, *kid_climb_08,
  *kid_climb_09, *kid_climb_10;

#endif	/* FREEPOP_KID_H */

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

#include "kid-normal.h"
#include "kid-walk.h"
#include "kid-start-run.h"
#include "kid-run.h"
#include "kid-stop-run.h"
#include "kid-turn-run.h"
#include "kid-couch.h"
#include "kid-turn.h"
#include "kid-stabilize.h"
#include "kid-jump.h"
#include "kid-vjump.h"
#include "kid-run-jump.h"
#include "kid-misstep.h"

/* bitmap resources */
#define KID_FULL_LIFE "dat/kid/objects/full live.png"
#define KID_EMPTY_LIFE "dat/kid/objects/empty live.png"
#define KID_FALL_13 "dat/kid/hanging and falling/frame13.png"
#define KID_FALL_14 "dat/kid/hanging and falling/frame14.png"
#define KID_FALL_15 "dat/kid/hanging and falling/frame15.png"
#define KID_FALL_16 "dat/kid/hanging and falling/frame16.png"
#define KID_FALL_17 "dat/kid/hanging and falling/frame17.png"
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
#define KID_SWORD_DEFENSE_18 "dat/kid/sword attacking/frame18.png"
#define KID_SWORD_DEFENSE_11 "dat/kid/sword attacking/frame11.png"
#define KID_SWORD_ATTACK_01 "dat/kid/sword attacking/frame01.png"
#define KID_SWORD_ATTACK_02 "dat/kid/sword attacking/frame02.png"
#define KID_SWORD_ATTACK_03 "dat/kid/sword attacking/frame03.png"
#define KID_SWORD_ATTACK_04 "dat/kid/sword attacking/frame04.png"
#define KID_SWORD_ATTACK_05 "dat/kid/sword attacking/frame05.png"
#define KID_SWORD_ATTACK_06 "dat/kid/sword attacking/frame06.png"
#define KID_SWORD_ATTACK_07 "dat/kid/sword attacking/frame07.png"

/* sound resources */
#define STEP_SAMPLE "dat/digisnd1/running 2.ogg"
#define HIT_GROUND_SAMPLE "dat/digisnd1/running 1.ogg"
#define HIT_WALL_SAMPLE "dat/digisnd1/touching a wall.ogg"
#define HANG_ON_FALL_SAMPLE "dat/digisnd1/normal falling.ogg"
#define DRINK_SAMPLE "dat/digisnd3/drinking.ogg"
#define GLORY_SAMPLE "dat/midisnd1/guard death 1.ogg"
#define TAKE_SWORD_SAMPLE "dat/digisnd1/taking out the sword.ogg"
#define SWORD_ATTACK_SAMPLE "dat/digisnd1/sword moving.ogg"

/* functions */
void load_kid (void);
void unload_kid (void);
void sample_kid (void);

void kid_climb (void);
void kid_fall (void);
void kid_unclimb (void);
void kid_drink (void);
void kid_raise_sword (void);
void kid_hang (void);
void kid_hang_wall (void);
void kid_hang_free (void);
void kid_keep_sword (void);
void kid_take_sword (void);
void kid_sword_normal (void);
void kid_sword_walkf (void);
void kid_sword_walkb (void);
void kid_sword_defense (void);
void kid_sword_attack (void);

bool is_kid_fall (struct frame *f);
bool is_kid_hang_or_climb (struct frame *f);
bool is_kid_climb (struct frame *f);
bool is_kid_vjump (struct frame *f);
bool is_kid_turn (struct frame *f);
bool is_kid_hanging_at_pos (struct frame *f, struct pos *p);
void draw_kid_lives (ALLEGRO_BITMAP *bitmap, int j);

/* variables */
extern struct anim kid; /* kid animation object */

#define KID_FALL_FRAMESET_NMEMB 13
#define KID_HANG_FRAMESET_NMEMB 13
#define KID_CLIMB_FRAMESET_NMEMB 15
#define KID_DRINK_FRAMESET_NMEMB 15
#define KID_RAISE_SWORD_FRAMESET_NMEMB 4
#define KID_KEEP_SWORD_FRAMESET_NMEMB 10
#define KID_TAKE_SWORD_FRAMESET_NMEMB 4
#define KID_SWORD_WALKF_FRAMESET_NMEMB 2
#define KID_SWORD_WALKB_FRAMESET_NMEMB 2
#define KID_SWORD_DEFENSE_FRAMESET_NMEMB 2
#define KID_SWORD_ATTACK_FRAMESET_NMEMB 7

extern struct frameset fall_frameset[KID_FALL_FRAMESET_NMEMB];
extern struct frameset kid_hang_frameset[KID_HANG_FRAMESET_NMEMB];
extern struct frameset kid_climb_frameset[KID_CLIMB_FRAMESET_NMEMB];
extern struct frameset kid_fall_frameset[KID_FALL_FRAMESET_NMEMB];
extern struct frameset kid_drink_frameset[KID_DRINK_FRAMESET_NMEMB];
extern struct frameset kid_raise_sword_frameset[KID_RAISE_SWORD_FRAMESET_NMEMB];
extern struct frameset kid_keep_sword_frameset[KID_KEEP_SWORD_FRAMESET_NMEMB];
extern struct frameset kid_take_sword_frameset[KID_TAKE_SWORD_FRAMESET_NMEMB];
extern struct frameset kid_sword_walkf_frameset[KID_SWORD_WALKF_FRAMESET_NMEMB];
extern struct frameset kid_sword_walkb_frameset[KID_SWORD_WALKB_FRAMESET_NMEMB];
extern struct frameset kid_sword_defense_frameset[KID_SWORD_DEFENSE_FRAMESET_NMEMB];
extern struct frameset kid_sword_attack_frameset[KID_SWORD_ATTACK_FRAMESET_NMEMB];



extern ALLEGRO_BITMAP *kid_climb_03, *kid_climb_04,
  *kid_climb_05, *kid_climb_06, *kid_climb_07, *kid_climb_08,
  *kid_climb_09, *kid_climb_10;

extern bool misstep, uncouch_slowly, critical_edge,
  hang_limit, turn, keep_sword_fast;

extern int inertia;

extern struct pos item_pos;

extern ALLEGRO_BITMAP *kid_full_life, *kid_empty_life,
  *kid_fall_13, *kid_fall_14, *kid_fall_15, *kid_fall_16, *kid_fall_17,
  *kid_hang_00, *kid_hang_01, *kid_hang_02, *kid_hang_03,
  *kid_hang_04, *kid_hang_05, *kid_hang_06, *kid_hang_07,
  *kid_hang_08, *kid_hang_09, *kid_hang_10, *kid_hang_11,
  *kid_hang_12, *kid_hang_14,
  *kid_climb_01, *kid_climb_02, *kid_climb_03, *kid_climb_04,
  *kid_climb_05, *kid_climb_06, *kid_climb_07, *kid_climb_08,
  *kid_climb_09, *kid_climb_10, *kid_climb_11, *kid_climb_12,
  *kid_climb_13, *kid_climb_14, *kid_climb_15,
  *kid_drink_01, *kid_drink_02, *kid_drink_03, *kid_drink_04, *kid_drink_05,
  *kid_drink_06, *kid_drink_07, *kid_drink_08, *kid_drink_09, *kid_drink_10,
  *kid_drink_11, *kid_drink_12, *kid_drink_13, *kid_drink_14, *kid_drink_15,
  *kid_raise_sword_01, *kid_raise_sword_02, *kid_raise_sword_03, *kid_raise_sword_04,
  *kid_keep_sword_01, *kid_keep_sword_02, *kid_keep_sword_03, *kid_keep_sword_04,
  *kid_keep_sword_05, *kid_keep_sword_06, *kid_keep_sword_07, *kid_keep_sword_08,
  *kid_keep_sword_09, *kid_keep_sword_10,
  *kid_take_sword_01, *kid_take_sword_02, *kid_take_sword_03, *kid_take_sword_04,
  *kid_sword_normal_08,
  *kid_sword_walkf_14, *kid_sword_walkf_15,
  *kid_sword_walkb_10, *kid_sword_walkb_07,
  *kid_sword_defense_18, *kid_sword_defense_11,
  *kid_sword_attack_01, *kid_sword_attack_02, *kid_sword_attack_03,
  *kid_sword_attack_04, *kid_sword_attack_05, *kid_sword_attack_06,
  *kid_sword_attack_07;

extern bool sample_step, sample_hit_ground, sample_hit_wall,
  sample_hang_on_fall, sample_drink, sample_glory,
  sample_take_sword, sample_sword_attack;

#endif	/* FREEPOP_KID_H */

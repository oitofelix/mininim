/*
  jaffar.h -- jaffar module;

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

#ifndef FREEPOP_JAFFAR_H
#define FREEPOP_JAFFAR_H

#define JAFFAR_NORMAL_00 "dat/pv/jaffar/walking/frame07.png"
#define JAFFAR_WALK_01 "dat/pv/jaffar/walking/frame01.png"
#define JAFFAR_WALK_02 "dat/pv/jaffar/walking/frame02.png"
#define JAFFAR_WALK_03 "dat/pv/jaffar/walking/frame03.png"
#define JAFFAR_WALK_04 "dat/pv/jaffar/walking/frame04.png"
#define JAFFAR_WALK_05 "dat/pv/jaffar/walking/frame05.png"
#define JAFFAR_WALK_06 "dat/pv/jaffar/walking/frame06.png"
#define JAFFAR_WALK_08 "dat/pv/jaffar/walking/frame08.png"
#define JAFFAR_WALK_09 "dat/pv/jaffar/walking/frame09.png"
#define JAFFAR_TURN_WALK_10 "dat/pv/jaffar/walking/frame10.png"
#define JAFFAR_TURN_WALK_11 "dat/pv/jaffar/walking/frame11.png"
#define JAFFAR_TURN_WALK_12 "dat/pv/jaffar/walking/frame12.png"
#define JAFFAR_TURN_WALK_13 "dat/pv/jaffar/walking/frame13.png"
#define JAFFAR_TURN_WALK_14 "dat/pv/jaffar/walking/frame14.png"
#define JAFFAR_TURN_WALK_15 "dat/pv/jaffar/walking/frame15.png"
#define JAFFAR_TURN_WALK_16 "dat/pv/jaffar/walking/frame16.png"
#define JAFFAR_TURN_WALK_17 "dat/pv/jaffar/walking/frame17.png"
#define JAFFAR_TURN_WALK_18 "dat/pv/jaffar/walking/frame18.png"
#define JAFFAR_TURN_WALK_19 "dat/pv/jaffar/walking/frame19.png"
#define JAFFAR_OPEN_ARMS_38 "dat/pv/jaffar/conjuring/frame38.png"
#define JAFFAR_OPEN_ARMS_20 "dat/pv/jaffar/conjuring/frame20.png"
#define JAFFAR_RAISE_ARMS_21 "dat/pv/jaffar/conjuring/frame21.png"
#define JAFFAR_RAISE_ARMS_22 "dat/pv/jaffar/conjuring/frame22.png"
#define JAFFAR_RAISE_ARMS_23 "dat/pv/jaffar/conjuring/frame23.png"
#define JAFFAR_RAISE_ARMS_24 "dat/pv/jaffar/conjuring/frame24.png"
#define JAFFAR_RAISE_ARMS_25 "dat/pv/jaffar/conjuring/frame25.png"
#define JAFFAR_RAISE_ARMS_26 "dat/pv/jaffar/conjuring/frame26.png"
#define JAFFAR_RAISE_ARMS_27 "dat/pv/jaffar/conjuring/frame27.png"
#define JAFFAR_RAISE_ARMS_28 "dat/pv/jaffar/conjuring/frame28.png"
#define JAFFAR_RAISE_ARMS_36 "dat/pv/jaffar/conjuring/frame36.png"
#define JAFFAR_RAISE_ARMS_37 "dat/pv/jaffar/conjuring/frame37.png"
#define JAFFAR_RAISE_ARMS_29 "dat/pv/jaffar/conjuring/frame29.png"
#define JAFFAR_LOWER_ARMS_30 "dat/pv/jaffar/conjuring/frame30.png"
#define JAFFAR_LOWER_ARMS_31 "dat/pv/jaffar/conjuring/frame31.png"
#define JAFFAR_LOWER_ARMS_32 "dat/pv/jaffar/conjuring/frame32.png"
#define JAFFAR_LOWER_ARMS_33 "dat/pv/jaffar/conjuring/frame33.png"
#define JAFFAR_LOWER_ARMS_34 "dat/pv/jaffar/conjuring/frame34.png"
#define JAFFAR_LOWER_ARMS_35 "dat/pv/jaffar/conjuring/frame35.png"

/* functions */
void load_jaffar (void);
void unload_jaffar (void);
void jaffar_normal (void);
void jaffar_walk (void);
void jaffar_open_arms (void);
void jaffar_raise_arms (void);
void jaffar_lower_arms (void);
void jaffar_turn_walk (void);

/* variables */
extern struct anim jaffar; /* animation object */

extern ALLEGRO_BITMAP *jaffar_normal_00,
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

#endif	/* FREEPOP_JAFFAR_H */

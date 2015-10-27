/*
  princess.h -- princess module;

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

#ifndef FREEPOP_PRINCESS_H
#define FREEPOP_PRINCESS_H

#define PRINCESS_NORMAL "dat/pv/princess/in story/frame01.png"
#define PRINCESS_TURN_02 "dat/pv/princess/in story/frame02.png"
#define PRINCESS_TURN_03 "dat/pv/princess/in story/frame03.png"
#define PRINCESS_TURN_04 "dat/pv/princess/in story/frame04.png"
#define PRINCESS_TURN_05 "dat/pv/princess/in story/frame05.png"
#define PRINCESS_TURN_06 "dat/pv/princess/in story/frame06.png"
#define PRINCESS_TURN_07 "dat/pv/princess/in story/frame07.png"
#define PRINCESS_TURN_08 "dat/pv/princess/in story/frame08.png"
#define PRINCESS_TURN_09 "dat/pv/princess/in story/frame09.png"
#define PRINCESS_STEP_BACK_10 "dat/pv/princess/in story/frame10.png"
#define PRINCESS_STEP_BACK_11 "dat/pv/princess/in story/frame11.png"
#define PRINCESS_STEP_BACK_12 "dat/pv/princess/in story/frame12.png"
#define PRINCESS_STEP_BACK_13 "dat/pv/princess/in story/frame13.png"
#define PRINCESS_STEP_BACK_14 "dat/pv/princess/in story/frame14.png"
#define PRINCESS_STEP_BACK_15 "dat/pv/princess/in story/frame15.png"
#define PRINCESS_LOOK_DOWN_16 "dat/pv/princess/in story/frame16.png"
#define PRINCESS_LOOK_DOWN_17 "dat/pv/princess/in story/frame17.png"

/* functions */
void load_princess (void);
void unload_princess (void);
void draw_princess_normal (void);
void draw_princess_turn (void);
void draw_princess_step_back (void);
void draw_princess_look_down (void);

/* variables */
extern void (*draw_princess) (void); /* princess mutable draw function */
extern struct anim princess; /* princess animation object */

extern ALLEGRO_BITMAP *princess_normal,
  *princess_turn_02, *princess_turn_03, *princess_turn_04, *princess_turn_05,
  *princess_turn_06, *princess_turn_07, *princess_turn_08, *princess_turn_09,
  *princess_step_back_10, *princess_step_back_11, *princess_step_back_12,
  *princess_step_back_13, *princess_step_back_14, *princess_step_back_15,
  *princess_look_down_16, *princess_look_down_17;

#endif	/* FREEPOP_PRINCESS_H */

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

#ifndef MININIM_JAFFAR_H
#define MININIM_JAFFAR_H

#define JAFFAR_NORMAL_00 "data/jaffar/normal/00.png"
#define JAFFAR_WALK_00 "data/jaffar/walk/00.png"
#define JAFFAR_WALK_01 "data/jaffar/walk/01.png"
#define JAFFAR_WALK_02 "data/jaffar/walk/02.png"
#define JAFFAR_WALK_03 "data/jaffar/walk/03.png"
#define JAFFAR_WALK_04 "data/jaffar/walk/04.png"
#define JAFFAR_WALK_05 "data/jaffar/walk/05.png"
#define JAFFAR_WALK_06 "data/jaffar/walk/06.png"
#define JAFFAR_WALK_07 "data/jaffar/walk/07.png"
#define JAFFAR_TURN_WALK_00 "data/jaffar/turn-walk/00.png"
#define JAFFAR_TURN_WALK_01 "data/jaffar/turn-walk/01.png"
#define JAFFAR_TURN_WALK_02 "data/jaffar/turn-walk/02.png"
#define JAFFAR_TURN_WALK_03 "data/jaffar/turn-walk/03.png"
#define JAFFAR_TURN_WALK_04 "data/jaffar/turn-walk/04.png"
#define JAFFAR_TURN_WALK_05 "data/jaffar/turn-walk/05.png"
#define JAFFAR_TURN_WALK_06 "data/jaffar/turn-walk/06.png"
#define JAFFAR_TURN_WALK_07 "data/jaffar/turn-walk/07.png"
#define JAFFAR_TURN_WALK_08 "data/jaffar/turn-walk/08.png"
#define JAFFAR_TURN_WALK_09 "data/jaffar/turn-walk/09.png"
#define JAFFAR_OPEN_ARMS_00 "data/jaffar/open-arms/00.png"
#define JAFFAR_OPEN_ARMS_01 "data/jaffar/open-arms/01.png"
#define JAFFAR_RAISE_ARMS_00 "data/jaffar/raise-arms/00.png"
#define JAFFAR_RAISE_ARMS_01 "data/jaffar/raise-arms/01.png"
#define JAFFAR_RAISE_ARMS_02 "data/jaffar/raise-arms/02.png"
#define JAFFAR_RAISE_ARMS_03 "data/jaffar/raise-arms/03.png"
#define JAFFAR_RAISE_ARMS_04 "data/jaffar/raise-arms/04.png"
#define JAFFAR_RAISE_ARMS_05 "data/jaffar/raise-arms/05.png"
#define JAFFAR_RAISE_ARMS_06 "data/jaffar/raise-arms/06.png"
#define JAFFAR_RAISE_ARMS_07 "data/jaffar/raise-arms/07.png"
#define JAFFAR_RAISE_ARMS_08 "data/jaffar/raise-arms/08.png"
#define JAFFAR_RAISE_ARMS_09 "data/jaffar/raise-arms/09.png"
#define JAFFAR_RAISE_ARMS_10 "data/jaffar/raise-arms/10.png"
#define JAFFAR_LOWER_ARMS_00 "data/jaffar/lower-arms/00.png"
#define JAFFAR_LOWER_ARMS_01 "data/jaffar/lower-arms/01.png"
#define JAFFAR_LOWER_ARMS_02 "data/jaffar/lower-arms/02.png"
#define JAFFAR_LOWER_ARMS_03 "data/jaffar/lower-arms/03.png"
#define JAFFAR_LOWER_ARMS_04 "data/jaffar/lower-arms/04.png"
#define JAFFAR_LOWER_ARMS_05 "data/jaffar/lower-arms/05.png"

/* functions */
void load_jaffar (void);
void unload_jaffar (void);
void jaffar_normal (struct anim *jaffar);
void jaffar_walk (struct anim *jaffar);
void jaffar_open_arms (struct anim *jaffar);
void jaffar_raise_arms (struct anim *jaffar);
void jaffar_lower_arms (struct anim *jaffar);
void jaffar_turn_walk (struct anim *jaffar);
void draw_jaffar_frame (ALLEGRO_BITMAP *bitmap, struct anim *j,
                        enum vm vm);

/* variables */
extern struct anim jaffar; /* animation object */

extern ALLEGRO_BITMAP *jaffar_normal_00,
  *jaffar_walk_00, *jaffar_walk_01, *jaffar_walk_02, *jaffar_walk_03, *jaffar_walk_04,
  *jaffar_walk_05, *jaffar_walk_06, *jaffar_walk_07,
  *jaffar_turn_walk_00, *jaffar_turn_walk_01, *jaffar_turn_walk_02,
  *jaffar_turn_walk_03, *jaffar_turn_walk_04, *jaffar_turn_walk_05,
  *jaffar_turn_walk_06, *jaffar_turn_walk_07, *jaffar_turn_walk_08,
  *jaffar_turn_walk_09,
  *jaffar_open_arms_00, *jaffar_open_arms_01,
  *jaffar_raise_arms_00, *jaffar_raise_arms_01, *jaffar_raise_arms_02,
  *jaffar_raise_arms_03, *jaffar_raise_arms_04, *jaffar_raise_arms_05,
  *jaffar_raise_arms_06, *jaffar_raise_arms_07, *jaffar_raise_arms_08,
  *jaffar_raise_arms_09, *jaffar_raise_arms_10,
  *jaffar_lower_arms_00, *jaffar_lower_arms_01, *jaffar_lower_arms_02,
  *jaffar_lower_arms_03, *jaffar_lower_arms_04, *jaffar_lower_arms_05;

#endif	/* MININIM_JAFFAR_H */

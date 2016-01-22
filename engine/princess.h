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

#ifndef MININIM_PRINCESS_H
#define MININIM_PRINCESS_H

#define PRINCESS_NORMAL_00 "data/princess/normal/00.png"
#define PRINCESS_TURN_00 "data/princess/turn/00.png"
#define PRINCESS_TURN_01 "data/princess/turn/01.png"
#define PRINCESS_TURN_02 "data/princess/turn/02.png"
#define PRINCESS_TURN_03 "data/princess/turn/03.png"
#define PRINCESS_TURN_04 "data/princess/turn/04.png"
#define PRINCESS_TURN_05 "data/princess/turn/05.png"
#define PRINCESS_TURN_06 "data/princess/turn/06.png"
#define PRINCESS_TURN_07 "data/princess/turn/07.png"
#define PRINCESS_STEP_BACK_00 "data/princess/step-back/00.png"
#define PRINCESS_STEP_BACK_01 "data/princess/step-back/01.png"
#define PRINCESS_STEP_BACK_02 "data/princess/step-back/02.png"
#define PRINCESS_STEP_BACK_03 "data/princess/step-back/03.png"
#define PRINCESS_STEP_BACK_04 "data/princess/step-back/04.png"
#define PRINCESS_STEP_BACK_05 "data/princess/step-back/05.png"
#define PRINCESS_LOOK_DOWN_00 "data/princess/look-down/00.png"
#define PRINCESS_LOOK_DOWN_01 "data/princess/look-down/01.png"
#define PRINCESS_REST_00 "data/princess/rest/00.png"
#define PRINCESS_COUCH_00 "data/princess/couch/00.png"
#define PRINCESS_COUCH_01 "data/princess/couch/01.png"
#define PRINCESS_COUCH_02 "data/princess/couch/02.png"
#define PRINCESS_COUCH_03 "data/princess/couch/03.png"
#define PRINCESS_COUCH_04 "data/princess/couch/04.png"
#define PRINCESS_COUCH_05 "data/princess/couch/05.png"
#define PRINCESS_COUCH_06 "data/princess/couch/06.png"
#define PRINCESS_COUCH_07 "data/princess/couch/07.png"
#define PRINCESS_COUCH_08 "data/princess/couch/08.png"
#define PRINCESS_COUCH_09 "data/princess/couch/09.png"
#define PRINCESS_COUCH_10 "data/princess/couch/10.png"
#define PRINCESS_STROKE_00 "data/princess/stroke/00.png"
#define PRINCESS_STROKE_01 "data/princess/stroke/01.png"
#define PRINCESS_STROKE_02 "data/princess/stroke/02.png"
#define PRINCESS_TURN_EMBRACE_00 "data/princess/turn-embrace/00.png"
#define PRINCESS_TURN_EMBRACE_01 "data/princess/turn-embrace/01.png"
#define PRINCESS_TURN_EMBRACE_02 "data/princess/turn-embrace/02.png"
#define PRINCESS_TURN_EMBRACE_03 "data/princess/turn-embrace/03.png"
#define PRINCESS_TURN_EMBRACE_04 "data/princess/turn-embrace/04.png"
#define PRINCESS_TURN_EMBRACE_05 "data/princess/turn-embrace/05.png"
#define PRINCESS_TURN_EMBRACE_06 "data/princess/turn-embrace/06.png"
#define PRINCESS_TURN_EMBRACE_07 "data/princess/turn-embrace/07.png"
#define PRINCESS_TURN_EMBRACE_08 "data/princess/turn-embrace/08.png"
#define PRINCESS_TURN_EMBRACE_09 "data/princess/turn-embrace/09.png"
#define PRINCESS_TURN_EMBRACE_10 "data/princess/turn-embrace/10.png"
#define PRINCESS_TURN_EMBRACE_11 "data/princess/turn-embrace/11.png"
#define PRINCESS_TURN_EMBRACE_12 "data/princess/turn-embrace/12.png"
#define PRINCESS_TURN_EMBRACE_13 "data/princess/turn-embrace/13.png"

/* functions */
void load_princess (void);
void unload_princess (void);
void princess_normal (struct anim *princess);
void princess_turn (struct anim *princess);
void princess_step_back (struct anim *princess);
void princess_look_down (struct anim *princess);
void princess_rest (struct anim *princess);
void princess_couch (struct anim *princess);
void princess_uncouch (struct anim *princess);
void princess_stroke (struct anim *princess);
void princess_turn_embrace (struct anim *princess);
void draw_princess_frame (ALLEGRO_BITMAP *bitmap, struct anim *p,
                          enum vm vm);
palette get_princess_palette (enum vm vm);
ALLEGRO_COLOR c_princess_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR e_princess_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR v_princess_palette (ALLEGRO_COLOR c);

/* variables */
extern struct anim princess; /* princess animation object */

extern ALLEGRO_BITMAP *princess_normal_00,
  *princess_turn_00, *princess_turn_01, *princess_turn_02, *princess_turn_03,
  *princess_turn_04, *princess_turn_05, *princess_turn_06, *princess_turn_07,
  *princess_step_back_00, *princess_step_back_01, *princess_step_back_02,
  *princess_step_back_03, *princess_step_back_04, *princess_step_back_05,
  *princess_look_down_00, *princess_look_down_01, *princess_rest_00,
  *princess_couch_00, *princess_couch_01, *princess_couch_02,
  *princess_couch_03, *princess_couch_04, *princess_couch_05,
  *princess_couch_06, *princess_couch_07, *princess_couch_08,
  *princess_couch_09, *princess_couch_10,
  *princess_stroke_00, *princess_stroke_01, *princess_stroke_02,
  *princess_turn_embrace_00, *princess_turn_embrace_01, *princess_turn_embrace_02,
  *princess_turn_embrace_03, *princess_turn_embrace_04, *princess_turn_embrace_05,
  *princess_turn_embrace_06, *princess_turn_embrace_07, *princess_turn_embrace_08,
  *princess_turn_embrace_09, *princess_turn_embrace_10, *princess_turn_embrace_11,
  *princess_turn_embrace_12, *princess_turn_embrace_13;

#endif	/* MININIM_PRINCESS_H */

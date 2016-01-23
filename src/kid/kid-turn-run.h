/*
  kid-turn-run.h -- kid turn run module;

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

#ifndef MININIM_KID_TURN_RUN_H
#define MININIM_KID_TURN_RUN_H

/* bitmaps */
#define KID_TURN_RUN_00 "data/kid/turn-run/00.png"
#define KID_TURN_RUN_01 "data/kid/turn-run/01.png"
#define KID_TURN_RUN_02 "data/kid/turn-run/02.png"
#define KID_TURN_RUN_03 "data/kid/turn-run/03.png"
#define KID_TURN_RUN_04 "data/kid/turn-run/04.png"
#define KID_TURN_RUN_05 "data/kid/turn-run/05.png"
#define KID_TURN_RUN_06 "data/kid/turn-run/06.png"
#define KID_TURN_RUN_07 "data/kid/turn-run/07.png"
#define KID_TURN_RUN_08 "data/kid/turn-run/08.png"

#define KID_TURN_RUN_FRAMESET_NMEMB 9
extern struct frameset kid_turn_run_frameset[KID_TURN_RUN_FRAMESET_NMEMB];

void load_kid_turn_run (void);
void unload_kid_turn_run (void);
void kid_turn_run (struct anim *k);
bool is_kid_turn_run (struct frame *f);

extern ALLEGRO_BITMAP *kid_turn_run_00, *kid_turn_run_01, *kid_turn_run_02,
  *kid_turn_run_03, *kid_turn_run_04, *kid_turn_run_05, *kid_turn_run_06,
  *kid_turn_run_07, *kid_turn_run_08;

#endif	/* MININIM_KID_TURN_RUN_H */

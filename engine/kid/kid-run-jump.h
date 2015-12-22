/*
  kid-run-jump.h -- kid run jump module;

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

#ifndef MININIM_KID_RUN_JUMP_H
#define MININIM_KID_RUN_JUMP_H

/* bitmaps */
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

#define KID_RUN_JUMP_FRAMESET_NMEMB 11
extern struct frameset kid_run_jump_frameset[KID_JUMP_FRAMESET_NMEMB];

void load_kid_run_jump (void);
void unload_kid_run_jump (void);
void kid_run_jump (struct anim *kid);
bool is_kid_run_jump_running (struct frame *f);
bool is_kid_run_jump_landing (struct frame *f);

extern ALLEGRO_BITMAP *kid_run_jump_01, *kid_run_jump_02, *kid_run_jump_03,
  *kid_run_jump_04, *kid_run_jump_05, *kid_run_jump_06,
  *kid_run_jump_07, *kid_run_jump_08, *kid_run_jump_09,
  *kid_run_jump_10, *kid_run_jump_11;

#endif	/* MININIM_KID_RUN_JUMP_H */

/*
  kid-run.h -- kid run module;

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

#ifndef MININIM_KID_RUN_H
#define MININIM_KID_RUN_H

/* bitmaps */
#define KID_RUN_00 "data/kid/run/00.png"
#define KID_RUN_01 "data/kid/run/01.png"
#define KID_RUN_02 "data/kid/run/02.png"
#define KID_RUN_03 "data/kid/run/03.png"
#define KID_RUN_04 "data/kid/run/04.png"
#define KID_RUN_05 "data/kid/run/05.png"
#define KID_RUN_06 "data/kid/run/06.png"
#define KID_RUN_07 "data/kid/run/07.png"

#define KID_RUN_FRAMESET_NMEMB 8
extern struct frameset kid_run_frameset[KID_RUN_FRAMESET_NMEMB];

void load_kid_run (void);
void unload_kid_run (void);
void kid_run (struct anim *k);
bool is_kid_run (struct frame *f);

extern ALLEGRO_BITMAP *kid_run_00, *kid_run_01, *kid_run_02,
  *kid_run_03, *kid_run_04, *kid_run_05, *kid_run_06, *kid_run_07;

#endif	/* MININIM_KID_RUN_H */

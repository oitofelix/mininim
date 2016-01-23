/*
  kid-start-run.h -- kid start run module;

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

#ifndef MININIM_KID_START_RUN_H
#define MININIM_KID_START_RUN_H

/* bitmaps */
#define KID_START_RUN_00 "data/kid/start-run/00.png"
#define KID_START_RUN_01 "data/kid/start-run/01.png"
#define KID_START_RUN_02 "data/kid/start-run/02.png"
#define KID_START_RUN_03 "data/kid/start-run/03.png"
#define KID_START_RUN_04 "data/kid/start-run/04.png"
#define KID_START_RUN_05 "data/kid/start-run/05.png"

#define KID_START_RUN_FRAMESET_NMEMB 6
extern struct frameset kid_start_run_frameset[KID_START_RUN_FRAMESET_NMEMB];

void load_kid_start_run (void);
void unload_kid_start_run (void);
void kid_start_run (struct anim *k);
bool is_kid_start_run (struct frame *f);

extern ALLEGRO_BITMAP *kid_start_run_00, *kid_start_run_01, *kid_start_run_02,
  *kid_start_run_03, *kid_start_run_04, *kid_start_run_05;

#endif	/* MININIM_KID_START_RUN_H */

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

#ifndef FREEPOP_KID_START_RUN_H
#define FREEPOP_KID_START_RUN_H

/* bitmaps */
#define KID_START_RUN_01 "dat/kid/running/frame01.png"
#define KID_START_RUN_02 "dat/kid/running/frame02.png"
#define KID_START_RUN_03 "dat/kid/running/frame03.png"
#define KID_START_RUN_04 "dat/kid/running/frame04.png"
#define KID_START_RUN_05 "dat/kid/running/frame05.png"
#define KID_START_RUN_06 "dat/kid/running/frame06.png"

#define KID_START_RUN_FRAMESET_NMEMB 6
extern struct frameset kid_start_run_frameset[KID_START_RUN_FRAMESET_NMEMB];

void load_kid_start_run (void);
void unload_kid_start_run (void);
void kid_start_run (void);

extern ALLEGRO_BITMAP *kid_start_run_01, *kid_start_run_02, *kid_start_run_03,
  *kid_start_run_04, *kid_start_run_05, *kid_start_run_06;

#endif	/* FREEPOP_KID_START_RUN_H */

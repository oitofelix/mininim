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
#define KID_RUN_07 "dat/kid/running/frame07.png"
#define KID_RUN_08 "dat/kid/running/frame08.png"
#define KID_RUN_09 "dat/kid/running/frame09.png"
#define KID_RUN_10 "dat/kid/running/frame10.png"
#define KID_RUN_11 "dat/kid/running/frame11.png"
#define KID_RUN_12 "dat/kid/running/frame12.png"
#define KID_RUN_13 "dat/kid/running/frame13.png"
#define KID_RUN_14 "dat/kid/running/frame14.png"

#define KID_RUN_FRAMESET_NMEMB 8
extern struct frameset kid_run_frameset[KID_RUN_FRAMESET_NMEMB];

void load_kid_run (void);
void unload_kid_run (void);
void kid_run (struct anim *k);
bool is_kid_run (struct frame *f);

extern ALLEGRO_BITMAP *kid_run_07, *kid_run_08, *kid_run_09,
  *kid_run_10, *kid_run_11, *kid_run_12, *kid_run_13, *kid_run_14;

#endif	/* MININIM_KID_RUN_H */

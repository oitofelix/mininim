/*
  kid.h -- kid module;

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

#ifndef FREEPOP_KID_H
#define FREEPOP_KID_H

/* bitmap resources */
#define KID_NORMAL "dat/kid/normal.png"
#define KID_START_RUN_01 "dat/kid/running/frame01.png"
#define KID_START_RUN_02 "dat/kid/running/frame02.png"
#define KID_START_RUN_03 "dat/kid/running/frame03.png"
#define KID_START_RUN_04 "dat/kid/running/frame04.png"
#define KID_START_RUN_05 "dat/kid/running/frame05.png"
#define KID_START_RUN_06 "dat/kid/running/frame06.png"
#define KID_START_RUN_07 "dat/kid/running/frame07.png"
#define KID_RUN_08 "dat/kid/running/frame08.png"
#define KID_RUN_09 "dat/kid/running/frame09.png"
#define KID_RUN_10 "dat/kid/running/frame10.png"
#define KID_RUN_11 "dat/kid/running/frame11.png"
#define KID_RUN_12 "dat/kid/running/frame12.png"
#define KID_RUN_13 "dat/kid/running/frame13.png"
#define KID_RUN_14 "dat/kid/running/frame14.png"
#define KID_TURN_01 "dat/kid/turning/frame01.png"
#define KID_TURN_02 "dat/kid/turning/frame02.png"
#define KID_TURN_03 "dat/kid/turning/frame03.png"
#define KID_TURN_04 "dat/kid/turning/frame04.png"
#define KID_STABILIZE_05 "dat/kid/turning/frame05.png"
#define KID_STABILIZE_06 "dat/kid/turning/frame06.png"
#define KID_STABILIZE_07 "dat/kid/turning/frame07.png"
#define KID_STABILIZE_08 "dat/kid/turning/frame08.png"
#define KID_STOP_RUN_01 "dat/kid/turn running/frame01.png"
#define KID_STOP_RUN_02 "dat/kid/turn running/frame02.png"
#define KID_STOP_RUN_03 "dat/kid/turn running/frame03.png"
#define KID_STOP_RUN_04 "dat/kid/turn running/frame04.png"
#define KID_TURN_RUN_05 "dat/kid/turn running/frame05.png"
#define KID_TURN_RUN_06 "dat/kid/turn running/frame06.png"
#define KID_TURN_RUN_07 "dat/kid/turn running/frame07.png"
#define KID_TURN_RUN_08 "dat/kid/turn running/frame08.png"
#define KID_TURN_RUN_09 "dat/kid/turn running/frame09.png"
#define KID_TURN_RUN_10 "dat/kid/turn running/frame10.png"
#define KID_TURN_RUN_11 "dat/kid/turn running/frame11.png"
#define KID_TURN_RUN_12 "dat/kid/turn running/frame12.png"
#define KID_TURN_RUN_13 "dat/kid/turn running/frame13.png"
#define KID_WALK_01 "dat/kid/walking a step/frame01.png"
#define KID_WALK_02 "dat/kid/walking a step/frame02.png"
#define KID_WALK_03 "dat/kid/walking a step/frame03.png"
#define KID_WALK_04 "dat/kid/walking a step/frame04.png"
#define KID_WALK_05 "dat/kid/walking a step/frame05.png"
#define KID_WALK_06 "dat/kid/walking a step/frame06.png"
#define KID_WALK_07 "dat/kid/walking a step/frame07.png"
#define KID_WALK_08 "dat/kid/walking a step/frame08.png"
#define KID_WALK_09 "dat/kid/walking a step/frame09.png"
#define KID_WALK_10 "dat/kid/walking a step/frame10.png"
#define KID_WALK_11 "dat/kid/walking a step/frame11.png"
#define KID_WALK_12 "dat/kid/walking a step/frame12.png"

/* functions */
void load_kid (void);
void unload_kid (void);

/* variables */
extern void (*draw_kid) (void); /* mutable kid draw function */

#endif	/* FREEPOP_KID_H */

/*
  fire.h -- fire module;

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

#ifndef FREEPOP_FIRE_H
#define FREEPOP_FIRE_H

/* macros */
#define FIRE_RANDOM_SEED_0 234423479
#define FIRE_RANDOM_SEED_1 501477214

/* bitmap resources */
#define FIRE_01 "dat/prince/fire/frame1.png"
#define FIRE_02 "dat/prince/fire/frame2.png"
#define FIRE_03 "dat/prince/fire/frame3.png"
#define FIRE_04 "dat/prince/fire/frame4.png"
#define FIRE_05 "dat/prince/fire/frame5.png"
#define FIRE_06 "dat/prince/fire/frame6.png"
#define FIRE_07 "dat/prince/fire/frame7.png"
#define FIRE_08 "dat/prince/fire/frame8.png"
#define FIRE_09 "dat/prince/fire/frame9.png"

void load_fire (void);
void unload_fire (void);
void draw_fire (ALLEGRO_BITMAP* bitmap, int room);
void draw_princess_room_fire (void);
struct coord fire_coord (struct pos p);

#endif	/* FREEPOP_FIRE_H */

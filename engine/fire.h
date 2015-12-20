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
#define FIRE_01 "data/fire/01.png"
#define FIRE_02 "data/fire/02.png"
#define FIRE_03 "data/fire/03.png"
#define FIRE_04 "data/fire/04.png"
#define FIRE_05 "data/fire/05.png"
#define FIRE_06 "data/fire/06.png"
#define FIRE_07 "data/fire/07.png"
#define FIRE_08 "data/fire/08.png"
#define FIRE_09 "data/fire/09.png"

void load_fire (void);
void unload_fire (void);
void draw_fire (ALLEGRO_BITMAP* bitmap, struct pos *p, int i,
                enum em em, enum vm vm);
void draw_princess_room_fire (void);
struct coord *fire_coord (struct pos *p, struct coord *c);

#endif	/* FREEPOP_FIRE_H */

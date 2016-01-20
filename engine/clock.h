/*
  clock.h -- clock module;

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

#ifndef MININIM_CLOCK_H
#define MININIM_CLOCK_H

#define CLOCK_00 "data/clock/00.png"
#define CLOCK_01 "data/clock/01.png"
#define CLOCK_02 "data/clock/02.png"
#define CLOCK_03 "data/clock/03.png"
#define CLOCK_04 "data/clock/04.png"
#define CLOCK_05 "data/clock/05.png"
#define CLOCK_06 "data/clock/06.png"
#define CLOCK_SAND_00 "data/clock/sand-00.png"
#define CLOCK_SAND_01 "data/clock/sand-01.png"
#define CLOCK_SAND_02 "data/clock/sand-02.png"

/* functions */
void load_clock (void);
void unload_clock (void);
void draw_clock (void);

/* variables */
extern ALLEGRO_BITMAP *princess_room_clock;

extern ALLEGRO_BITMAP *clock_00, *clock_01, *clock_02, *clock_03, *clock_04, *clock_05,
  *clock_06, *clock_sand_00, *clock_sand_01, *clock_sand_02;

#endif	/* MININIM_CLOCK_H */

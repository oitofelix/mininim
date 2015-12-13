/*
  level.h -- level module;

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

#ifndef FREEPOP_LEVEL_H
#define FREEPOP_LEVEL_H

#include "anim.h"

/* current level */
extern struct level *level;
extern int room_view, draw_cycle;
extern struct anim *current_kid;

void play_level (struct level *level);
void register_cons (void);

#endif	/* FREEPOP_LEVEL_H */

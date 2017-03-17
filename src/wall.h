/*
  wall.h -- wall module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

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

#ifndef MININIM_WALL_H
#define MININIM_WALL_H

enum should_draw should_draw_face (struct pos *p, struct frame *f);

void draw_wall (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_wall_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f);

enum wall_correlation wall_correlation (struct pos *p);

#endif	/* MININIM_WALL_H */

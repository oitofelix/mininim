/*
  carpet.h -- carpet module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

#ifndef MININIM_CARPET_H
#define MININIM_CARPET_H

void draw_carpet (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_carpet_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     struct frame *f);

#endif	/* MININIM_CARPET_H */

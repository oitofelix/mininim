/*
  kid-climb.h -- kid climb module;

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

#ifndef MININIM_KID_CLIMB_H
#define MININIM_KID_CLIMB_H

void kid_climb (struct actor *k);
bool is_kid_climb (struct actor *k);
bool is_kid_successfully_climbing_at_pos (struct actor *k,
                                          struct pos *hang_pos,
                                          struct pos *p);
bool is_kid_successfully_climbing (struct actor *k);

#endif	/* MININIM_KID_CLIMB_H */

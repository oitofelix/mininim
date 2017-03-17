/*
  kid-die.h -- kid die module;

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

#ifndef MININIM_KID_DIE_H
#define MININIM_KID_DIE_H

void kid_resurrect (struct actor *k);
void kid_die (struct actor *k);
void kid_die_spiked (struct actor *k);
void kid_die_chomped (struct actor *k);
void kid_die_suddenly (struct actor *k);
bool is_kid_dead (struct actor *k);
void kill_kid_shadows (struct actor *k);
void kid_die_properly (struct actor *k);

#endif	/* MININIM_KID_DIE_H */

/*
  guard-die.h -- guard die module;

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

#ifndef MININIM_GUARD_DIE_H
#define MININIM_GUARD_DIE_H

void guard_resurrect (struct actor *g);
void guard_die (struct actor *g);
void guard_die_spiked (struct actor *g);
void guard_die_chomped (struct actor *g);
void guard_die_suddenly (struct actor *g);
void raise_skeleton (struct actor *s);
bool is_guard_dead (struct actor *g);
void guard_die_properly (struct actor *g);

#endif	/* MININIM_GUARD_DIE_H */

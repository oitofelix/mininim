/*
  kid-run-jump.h -- kid run jump module;

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

#ifndef MININIM_KID_RUN_JUMP_H
#define MININIM_KID_RUN_JUMP_H

void kid_run_jump (struct actor *k);
bool is_kid_run_jump_running (struct actor *k);
bool is_kid_run_jump_air (struct actor *k);
bool is_kid_run_jump_landing (struct actor *k);

#endif	/* MININIM_KID_RUN_JUMP_H */

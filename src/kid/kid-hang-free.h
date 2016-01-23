/*
  kid-hang-free.h -- kid hang free module;

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

#ifndef MININIM_KID_HANG_FREE_H
#define MININIM_KID_HANG_FREE_H

void load_kid_hang_free (void);
void unload_kid_hang_free (void);
void kid_hang_free (struct anim *k);

#endif	/* MININIM_KID_HANG_FREE_H */

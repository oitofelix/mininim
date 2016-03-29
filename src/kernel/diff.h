/*
  diff.h -- diff module;

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

#ifndef MININIM_DIFF_H
#define MININIM_DIFF_H

void add_diffset_diff (struct diffset *diffset, uint8_t *ptr0, uint8_t *ptr1,
                       size_t size, char *desc);
bool apply_diffset_diff (struct diffset *diffset, uint8_t *base, int dir,
                         char **desc);
void free_diffset (struct diffset *diffset);
void free_diff (struct diff *d);
void apply_diff (struct diff *d, uint8_t *base, int dir);
struct diff *diff (uint8_t *ptr0, uint8_t *ptr1, size_t size, struct diff *d,
                   char *desc);
void add_diff_line_byte (struct diff_line *l, uint8_t b0, uint8_t b1, size_t offset);

#endif	/* MININIM_DIFF_H */

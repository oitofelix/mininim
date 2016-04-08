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

void add_diffset_diff (struct diffset *diffset, void *ptr0, void *ptr1,
                       size_t size, size_t unit_size, char *desc);
bool can_apply_diffset (struct diffset *diffset, int dir);
bool apply_diffset_diff (struct diffset *diffset, void *base, size_t size,
                         int dir, char **desc);
void free_diffset (struct diffset *diffset);
void free_diff (struct diff *d);
void apply_diff (struct diff *d, void *base, size_t size, int dir);
struct diff *diff (void *ptr0, void *ptr1, size_t size, size_t unit_size,
                   struct diff *d, char *desc);
void add_diff_line_unit (struct diff_line *l, void *b0, void *b1,
                         size_t unit_size);

#endif	/* MININIM_DIFF_H */

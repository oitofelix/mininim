/*
  array.h -- array module;

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

#ifndef MININIM_ARRAY_H
#define MININIM_ARRAY_H

void * add_to_array (void *s_base, size_t s_nmemb,
                     void *d_base, size_t *d_nmemb, size_t d_index,
                     size_t size);
void * remove_from_array (void *base, size_t *nmemb, size_t index,
                          size_t count, size_t size);
void destroy_array (void **base, size_t *nmemb);

#endif	/* MININIM_ARRAY_H */
